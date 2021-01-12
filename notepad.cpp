#include <string.h>
#include <QFileDialog>
#include <QString>
#include <QTextStream>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QMessageBox>
#include <sstream>
#include <thread>
#include "ServerConnection.h"
#include "notepad.h"
#include "ui_notepad.h"
#include "hack_connect.h"
#include "operation.h"

using namespace std;


Notepad::Notepad(QWidget *parent) : QWidget(parent), ui(new Ui::Notepad) 
{
    ui->setupUi(this);
    this->setWindowTitle("New Text Document");
    this->server = new ServerConnection(Notepad::PORT);
    this->lastId = 0;
    connect(this, SIGNAL(signal_peerOp()), this, SLOT(slot_peerOp()));

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
    string errMsg = "ERROR";
    std::size_t foundError = responseMsg.find(errMsg);

    if(foundError == string::npos)
    {
        new std::thread([this](){this->PeerThreadLoop();});

        auto pos = responseMsg.find(" ");
        string lastIdStr = responseMsg.substr(0, pos);
        string content = responseMsg.substr(pos + 1);
        this->ui->textEdit->setReadOnly(true);
        this->ui->textEdit->setText(QString(content.c_str()));
        this->ui->textEdit->setReadOnly(false);

        stringstream ss(lastIdStr);
        ss >> this->lastId;
    }
    return responseMsg;
}


void Notepad::on_pushButton_clicked()
{
    QString docName;
    QString defDocName = windowTitle();
    defDocName.append(".txt");
    docName = QFileDialog::getSaveFileName(this, "Save Document", defDocName);
    QFile saveDoc(docName);
    if(saveDoc.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream content(&saveDoc);
        content << ui->textEdit->toPlainText();
        saveDoc.flush();
        saveDoc.close();
    }
}

void Notepad::on_textEdit_textChanged()
{
    QString currentContent;
    currentContent = ui->textEdit->toPlainText();

    if(this->ui->textEdit->isReadOnly())
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
    this->ui->textEdit->setReadOnly(true);
    QString qContent = this->ui->textEdit->toPlainText();
    string content = qContent.toStdString();

    while(!this->toApply.empty())
    {
        Operation operation = *this->toApply.begin();
        if (updateOperation(this->history, operation, true))
        {
            operation.applyOperation(content); //apply operation on current text
        }
        this->lastId = operation.id;
        this->toApply.pop_front();
    }

    this->ui->textEdit->setText(QString(content.c_str()));
    this->ui->textEdit->setReadOnly(false); //allow user to continue editing
}

