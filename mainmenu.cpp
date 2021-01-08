#include <iostream>
#include <QTableWidget>
#include <QTableView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QInputDialog>
#include <QString>
#include <QTableWidgetItem>
#include <QErrorMessage>
#include <QAbstractItemModel>
#include "documentstable.h"
#include "notepad.h"
#include "mainmenu.h"


MainMenu::MainMenu(QWidget *parent) : QWidget(parent)
{

    QVBoxLayout *vBox = new QVBoxLayout;
    this->_table = new DocumentsTable;
    vBox->addWidget(this->_table);
    QStringList list;
    list << "Documents";
    this->_table->setColumnCount(1);
    this->_table->setHorizontalHeaderLabels(list);
    this->_table->setRowCount(5);
    QHBoxLayout *hBox = new QHBoxLayout;
    QPushButton *createButton = new QPushButton("Create");
    hBox->addWidget(createButton);
    connect(createButton, SIGNAL(clicked(bool)), this, SLOT(onCreatePressed(bool)));

    QPushButton *openButton = new QPushButton("Open");
    hBox->addWidget(openButton);
    connect(openButton, SIGNAL(clicked(bool)), this, SLOT(onOpenPressed(bool)));
    QPushButton *deleteButton = new QPushButton("Delete");
    hBox->addWidget(deleteButton);
    connect(deleteButton, SIGNAL(clicked(bool)), this, SLOT(onDeletePressed(bool)));
    QWidget *hBoxSupport = new QWidget;

    hBoxSupport->setLayout(hBox);
    vBox->addWidget(hBoxSupport);
    this->setLayout(vBox);

    vBox->setAlignment(Qt::AlignCenter);
    this->server = new ServerConnection(MainMenu::PORT);
    this->server->Connect();

    this->refreshTable();
}


void MainMenu::onCreatePressed(bool enabled){


    bool hasError;

    do {
        bool ok;
        hasError=false;

        QString result = QInputDialog::getText(this, "Notepad", "Insert a name for the file!", QLineEdit::Normal, "", &ok);
        if(result!=NULL) {
            string createCommand = "create ";
            createCommand += result.toStdString();
            this->server->WriteCommand(createCommand);

            string answer;
            answer = this->server->ReadCommand();
            string errorMessage = "ERROR";
            std::size_t found = answer.find(errorMessage);

            if(found == 0) {

                QMessageBox messageBox;
                messageBox.critical(0, "Error", "File with the same name already exists! \n Choose another name!");
                continue;
            }


            else {

                  this->_documents.push_back(new std::string (result.toStdString()));
            }
        }

        else if(ok){
            hasError=true;
        }

    } while(hasError);

    this->refreshTable();

}

void MainMenu::onOpenPressed(bool enabled){
    QModelIndexList selection = this->_table->selectionModel()->selectedRows();

    if(selection.empty()) {


        QMessageBox messageBox;
        messageBox.critical(0,"Error", "Please select a document!");

        return;

    }
    else {
        for(int i=0; i< selection.count(); i++) {

            QModelIndex index = selection.at(i);
            int row = index.row();
            string documentName = this->_documents[row]->c_str();
            Notepad *notepad = new Notepad();

            string answer = notepad->Open(documentName);
            string errorMessage = "ERROR";
            std::size_t found = answer.find(errorMessage);
            if(found == 0) {
                QMessageBox messageBox;
                messageBox.critical(0, "Error", "File is already edited by two clients!");
                // close this window

                delete notepad;
                return;
            }

            QString docTitle = QString(documentName.c_str());
            notepad->setWindowTitle(docTitle);
            notepad->show();
        }
    }


}

void MainMenu::onDeletePressed(bool enabled){

    QModelIndexList selection = this->_table->selectionModel()->selectedRows();

    if(selection.empty()) {

        QMessageBox messageBox;
        messageBox.critical(0,"Error", "Please select a document!");

        return;

    }

    else {

        string deleteCommand = "delete ";

        for(int i=selection.count() - 1; i >= 0; i--) {

            QModelIndex index = selection.at(i);
            QString str = this->_table->model()->data(index).toString();
            deleteCommand += str.toStdString();
            this->server->WriteCommand(deleteCommand);

            string answer;
            answer = this->server->ReadCommand();
            string errorMessage = "ERROR";
            std::size_t found = answer.find(errorMessage);

            if(found == 0) {

                QMessageBox messageBox;
                messageBox.critical(0, "Error", "File was already removed by another user!");

            }

        }
    }



    for(int i=selection.count() - 1; i >= 0; i--) {

        QModelIndex index = selection.at(i);
        int row = index.row();
        delete this->_documents[row];
        this->_documents.erase(this->_documents.begin() + row);

    }

    this->refreshTable();

}


void MainMenu::refreshTable(){


    for(auto documentFile : this->_documents) {
        delete documentFile;
    }

    this->_documents.clear();
    int code;
    if ((code = this->server->WriteCommand("list")) != 0) {
        QMessageBox messageBox;
        messageBox.critical(0,"Error", strerror(code));
        return;
    }

    int size = atoi(this->server->ReadCommand().c_str());
    for(int i=0;i<size; i++) {
        string *documentName = new string;
        documentName->append(this->server->ReadCommand());
        this->_documents.push_back(documentName);
    }

    for(auto widget : this->_documentWidgets) {
        delete widget;
    }
    this->_documentWidgets.clear();

    if(this->_documents.empty()){
        this->_table->setRowCount(0);
        return;
    }

    this->_table->setRowCount(this->_documents.size());
    for(unsigned int i=0; i<this->_documents.size(); i++){
        QTableWidgetItem *item = new QTableWidgetItem(this->_documents[i]->c_str());
        this->_table->setItem(i, 0, item);
        this->_documentWidgets.push_back(item);

    }



}
