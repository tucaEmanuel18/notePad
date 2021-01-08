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

using std::string;
using std::vector;
using std::stringstream;


Notepad::Notepad(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Notepad) {
    ui->setupUi(this);
    this->setWindowTitle("New Document");
    this->server = new ServerConnection(Notepad::PORT);
    this->prevId = 0;
    connect(this, SIGNAL(onPeerOperation()), this, SLOT(slot_onPeerOperation()));
    int code;
    if((code = this->server->Connect()) != 0) {
        QMessageBox messageBox;
        messageBox.critical(0,"Error",strerror(code));
        // close this window
        return;
    }
}

Notepad::~Notepad() {
    delete ui;
}

string Notepad::Open(string documentName) {
    string openCommand = "open " + documentName;
    this->server->WriteCommand(openCommand);

    string answer = this->server->ReadCommand();

    string errorMessage = "ERROR";
    std::size_t found = answer.find(errorMessage);

    if(found == string::npos) {
        new std::thread([this](){
            this->ClientThreadLoop();
        });

        string space = " ";
        auto found = answer.find(space);
        string prevIdStr = answer.substr(0, found);
        string text = answer.substr(found + 1);
        this->ui->textEdit->setReadOnly(true);
        this->ui->textEdit->setText(QString(text.c_str()));
        this->ui->textEdit->setReadOnly(false);

        stringstream ss(prevIdStr);
        ss >> this->prevId;
    }

    return answer;
}


void Notepad::on_pushButton_clicked()
{
    QString filename;
    QString defaulFileName = windowTitle();
    defaulFileName.append(".txt");

    filename = QFileDialog::getSaveFileName(this, "Save Document", defaulFileName);

    QFile saveFile(filename);
    if(saveFile.open(QFile::WriteOnly | QFile::Text)) {

        QTextStream out(&saveFile);
        out << ui->textEdit->toPlainText();
        saveFile.flush();
        saveFile.close();
    }

}



void Notepad::on_textEdit_textChanged()
{
    QString currentText;
    currentText = ui->textEdit->toPlainText();

    if(this->ui->textEdit->isReadOnly()) {
        this->previousText = currentText;
        return;
    }

    vector<Operation> operations;

    for(int i=0; i<this->previousText.size() && i<currentText.size(); i++) {
        if(this->previousText[i] != currentText[i]) {
            Operation deleteOperation = Operation(false, this->prevId, i, this->previousText[i].toLatin1());
            this->pendingOperations.push_back(deleteOperation);
            this->server->WriteCommand(deleteOperation.toStr());


            Operation insertOperation = Operation(true, this->prevId, i, currentText[i].toLatin1());
            this->pendingOperations.push_back(insertOperation);
            this->server->WriteCommand(insertOperation.toStr());
        }


    }
    if(this->previousText.size() > currentText.size()) {
        for(int i = currentText.size(); i < this->previousText.size(); i++) {

            Operation deleteOperation = Operation(false, this->prevId, currentText.size(), this->previousText[i].toLatin1());
            this->pendingOperations.push_back(deleteOperation);
            this->server->WriteCommand(deleteOperation.toStr());

        }
    }
    if(this->previousText.size() < currentText.size()) {

        for(int i = this->previousText.size(); i < currentText.size(); i++) {

            Operation insertOperation = Operation(true, this->prevId, i, currentText[i].toLatin1());
            this->pendingOperations.push_back(insertOperation);
            this->server->WriteCommand(insertOperation.toStr());

        }

    }
    this->previousText = currentText;
}

void Notepad::ClientThreadLoop() {

    while(1) {
        string msg;
        try {
            msg = this->server->ReadCommand();
        }
        catch(std::ios_base::failure error){
            QMessageBox messageBox;
            messageBox.critical(0,"Error", error.what());

        }
        if(msg.compare(0, 3, "add") ==0 || (msg.compare(0, 3, "del")) == 0) {
            Operation operation = Operation(msg);
            if(updateOperation(this->history, operation)) {
                this->toApply.push_back(operation);
                this->onPeerOperation();
            }
        }
        else {
            stringstream ss(msg);

            int id, serverId;
            bool hasPeer;
            ss >> id >> serverId >> hasPeer;

            Operation op = *this->pendingOperations.begin();
            op.id = id;
            op.serverId = serverId;

            if(hasPeer) {
                // we should remember the operations if there is a peer
                // because he isn't up to date
                this->history.push_back(op);
            }
            else {
                // if there is no peer then there is no need to remember what we did
                this->history.clear();
            }

            this->pendingOperations.pop_front();
        }
    }
}


void Notepad::slot_onPeerOperation() {
    this->ui->textEdit->setReadOnly(true);

    QString qtext = this->ui->textEdit->toPlainText();
    string text = qtext.toStdString();

    while(!this->toApply.empty()) {
        Operation op = *this->toApply.begin();
        if (updateOperation(this->history, op, true)) {
            op.applyOperation(text); //apply operation on current text
        }
        this->prevId = op.id;
        this->toApply.pop_front();
    }

    this->ui->textEdit->setText(QString(text.c_str()));

    this->ui->textEdit->setReadOnly(false); //allow user to continue editing
}

