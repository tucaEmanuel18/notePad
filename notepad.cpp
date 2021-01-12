#include <QFileDialog>
#include <QString>
#include <QTextStream>
#include <QMessageBox>
#include <sstream>
#include <string.h>
#include <thread>
#include "ServerConnection.h"
#include "notepad.h"
#include "ui_notepad.h"
#include "operation.h"

using namespace std;

Notepad::Notepad(QWidget *parent) : QWidget(parent), ui(new Ui::Notepad) 
{
    ui->setupUi(this);
    this->setWindowTitle("New Text Document");
    this->server = new ServerConnection(Notepad::PORT);
    this->lastId = 0;
    connect(this, SIGNAL(signal_peerOp()), this, SLOT(slot_peerOp()));
    connect(ui->downloadButton, SIGNAL(clicked(bool)), this, SLOT(on_downloadButton()));
    int cod;
    if((cod = this->server->Connect()) != 0)
    {
        // if the connection fail throw error and close window
        QMessageBox msgForUser;
        msgForUser.critical(0,"Error",strerror(cod));
        return;
    }
}

Notepad::~Notepad()
{
    delete ui;
}

string Notepad::Open(string docName)
{
    // build and send command for server
    string openCommand = "open " + docName;
    this->server->WriteCommand(openCommand);

    string responseMsg = this->server->ReadCommand();

    // receive response from server
    std::size_t foundError = responseMsg.find("ERROR");

    if(foundError == string::npos)
    {
        new std::thread([this](){this->PeerThreadLoop();});

        auto pos = responseMsg.find(" ");
        string lastIdStr = responseMsg.substr(0, pos);
        string content = responseMsg.substr(pos + 1);
        this->ui->editBox->setReadOnly(true);
        this->ui->editBox->setText(QString(content.c_str()));
        this->ui->editBox->setReadOnly(false);

        stringstream ss(lastIdStr);
        ss >> this->lastId;
    }
    return responseMsg;
}


void Notepad::on_downloadButton()
{
    QString docName;
    QString defDocName = windowTitle();
    defDocName.append(".txt");
    docName = QFileDialog::getSaveFileName(this, "Save Document", defDocName);
    QFile saveDoc(docName);
    if(saveDoc.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream content(&saveDoc);
        content << ui->editBox->toPlainText();
        saveDoc.flush();
        saveDoc.close();
    }
}

void Notepad::on_editBox_textChanged()
{
    QString currentContent;
    currentContent = ui->editBox->toPlainText();

    if(this->ui->editBox->isReadOnly())
    {
        this->previousContent = currentContent;
        return;
    }
    vector<Operation> operations;

    for(int index = 0; index < this->previousContent.size() && index < currentContent.size(); index++)
    {
        // cazul in care s-a inlocuit un caracter cu alt caracter
        if(this->previousContent[index] != currentContent[index])
        {
            Operation deleteOp = Operation(false, this->lastId, index, this->previousContent[index].toLatin1());
            this->waitingOpList.push_back(deleteOp);
            this->server->WriteCommand(deleteOp.toStr());

            Operation insertOp = Operation(true, this->lastId, index, currentContent[index].toLatin1());
            this->waitingOpList.push_back(insertOp);
            this->server->WriteCommand(insertOp.toStr());
        }
    }

    if(this->previousContent.size() > currentContent.size())
    {
        // cazul in care s-au sters caractere de la final
        for(int index = currentContent.size(); index < this->previousContent.size(); index++)
        {
            Operation deleteOp = Operation(false, this->lastId, currentContent.size(), this->previousContent[index].toLatin1());
            this->waitingOpList.push_back(deleteOp);
            this->server->WriteCommand(deleteOp.toStr());
        }
    }
    if(this->previousContent.size() < currentContent.size())
    {
        // cazul in care s-au adaugat caractere la finalul contentului
        for(int index = this->previousContent.size(); index < currentContent.size(); index++)
        {
            Operation insertOp = Operation(true, this->lastId, index, currentContent[index].toLatin1());
            this->waitingOpList.push_back(insertOp);
            this->server->WriteCommand(insertOp.toStr());
        }

    }
    this->previousContent = currentContent;
}

void Notepad::PeerThreadLoop()
{
    while(1)
    {
        string fromPeerMsg;
        try
        {
            fromPeerMsg = this->server->ReadCommand();
        }
        catch(std::ios_base::failure err)
        {
            QMessageBox msgForUser;
            msgForUser.critical(0, "Error", err.what());
        }

        if(fromPeerMsg.compare(0, 6, "insert") == 0 || (fromPeerMsg.compare(0, 6, "delete")) == 0)
        {
            Operation operation = Operation(fromPeerMsg);
            if(updateOperation(this->history, operation))
            {
                this->toApply.push_back(operation);
                this->signal_peerOp();
            }
        }
        else
        {
            stringstream ss(fromPeerMsg);
            bool hasPeer;
            int serverId, id;
            ss >> id >> serverId >> hasPeer;

            Operation op = *this->waitingOpList.begin();
            op.id = id;
            op.serverId = serverId;

            if(hasPeer) // if there is a peer we should remember this op because he isn't up to date
            {
                this->history.push_back(op);
            }
            else // We no need to remember what we did if there is no peer
            {
                this->history.clear();
            }

            this->waitingOpList.pop_front();
        }
    }
}


void Notepad::slot_peerOp()
{
    this->ui->editBox->setReadOnly(true);
    QString qContent = this->ui->editBox->toPlainText();
    string content = qContent.toStdString();

    while(!this->toApply.empty())
    {
        Operation operation = *this->toApply.begin();
        if (updateOperation(this->history, operation, true))
        {
            //apply operation on current text
            operation.applyOperation(content);
        }
        this->lastId = operation.id;
        this->toApply.pop_front();
    }

    this->ui->editBox->setText(QString(content.c_str()));
    //allow user to continue editing
    this->ui->editBox->setReadOnly(false);
}

