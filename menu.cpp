#include "menu.h"
#include "ui_menu.h"
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
#include "notepad.h"
#include "connect_page.h"
menu::menu(QWidget *parent, ServerConnection* sv) :
    QWidget(parent),
    ui(new Ui::menu)
{
    ui->setupUi(this);
    this->setWindowTitle("Menu");
    QStringList list;
    list << "Documents";
    ui->documentsTable->setColumnCount(1);
    ui->documentsTable->setColumnWidth(0, 100);
    ui->documentsTable->setHorizontalHeaderLabels(list);
    ui->documentsTable->setRowCount(5);

    connect(ui->createButton, SIGNAL(clicked(bool)), this, SLOT(onCreatePressed(bool)));
    connect(ui->openButton, SIGNAL(clicked(bool)), this, SLOT(onOpenPressed(bool)));
    connect(ui->deleteButton, SIGNAL(clicked(bool)), this, SLOT(onDeletePressed(bool)));
    connect(ui->shareButton, SIGNAL(clicked(bool)), this, SLOT(onSharePressed(bool)));
    connect(ui->refreshButton, SIGNAL(clicked(bool)), this, SLOT(refreshTable()));
    this->server = sv;
    this->refreshTable();
}

menu::~menu()
{
    delete ui;
}


void menu::onCreatePressed(bool enabled)
{
    bool hasError;
    do {
        bool ok;
        hasError=false;

        QString result = QInputDialog::getText(this, "Notepad", "Insert a name for the file!", QLineEdit::Normal, "", &ok);
        if(result != NULL)
        {
            string createCommand = "create ";
            createCommand += result.toStdString();
            this->server->WriteCommand(createCommand);

            string answer;
            answer = this->server->ReadCommand();
            string errorMessage = "ERROR";
            std::size_t found = answer.find(errorMessage);

            if(found == 0)
            {
                QMessageBox messageBox;
                messageBox.critical(0, "Error", "File with the same name already exists! \n Choose another name!");
                continue;
            }
            else
            {
                  this->documents.push_back(new std::string (result.toStdString()));
            }
        }
        else if(ok)
        {
            hasError = true;
        }
    }while(hasError);
    this->refreshTable();
}

void menu::onOpenPressed(bool enabled)
{
    QModelIndexList selection = ui->documentsTable->selectionModel()->selectedRows();

    if(selection.empty())
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Error", "Please select a document!");
    }
    else
    {
        for(int i = 0; i < selection.count(); i++)
        {
            QModelIndex index = selection.at(i);
            int row = index.row();
            string documentName = this->documents[row]->c_str();
            Notepad *notepad = new Notepad();

            string answer = notepad->Open(documentName);
            string errorMessage = "ERROR";
            std::size_t found = answer.find(errorMessage);
            if(found == 0)
            {
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

void menu::onDeletePressed(bool enabled)
{

    QModelIndexList selection = ui->documentsTable->selectionModel()->selectedRows();
    if(selection.empty())
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Error", "Please select a document!");
    }
    else if(selection.count() > 1)
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Error", "Please select just a document!");
    }
    else
    {
        string deleteCommand = "remove ";
        QModelIndex index = selection.at(0);
        QString str = ui->documentsTable->model()->data(index).toString();
        deleteCommand += str.toStdString();
        this->server->WriteCommand(deleteCommand);

        int row = index.row();
        delete this->documents[row];
        this->documents.erase(this->documents.begin() + row);
        this->refreshTable();
   }
}

void menu::onSharePressed(bool enabled)
{
    QModelIndexList selection = ui->documentsTable->selectionModel()->selectedRows();
    if(selection.empty())
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Error", "Please select a document!");
    }
    else if(selection.count() > 1)
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Error", "Please select just a document!");
    }
    else
    {
        bool hasError;
        do
        {
            bool ok;
            hasError = false;
            QString result = QInputDialog::getText(this, "Share", "Insert the username of the person you want to share the file with!", QLineEdit::Normal, "", &ok);
            if(result != NULL)
            {
                string shareCommand = "share ";
                // obtain the file name
                QModelIndex index = selection.at(0);
                QString str = ui->documentsTable->model()->data(index).toString();
                shareCommand += str.toStdString();

                shareCommand += " ";

                // add the friend userName
                shareCommand += result.toStdString();
                this->server->WriteCommand(shareCommand);

                string answer;
                answer = this->server->ReadCommand();
                string errorMessage = "ERROR";
                std::size_t found = answer.find(errorMessage);

                if(found == 0)
                {
                   QMessageBox messageBox;
                   messageBox.critical(0, "Error", "There is no user with the specified username or specified user has already acces to this file.");
                }
            }
            else if(ok)
            {
                hasError = true;
            }
        }while(hasError);
    }
}

void menu::refreshTable()
{
    for(auto documentFile : this->documents) {
        delete documentFile;
    }

    this->documents.clear();
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
        this->documents.push_back(documentName);
    }

    for(auto widget : this->documentTableItem) {
        delete widget;
    }
    this->documentTableItem.clear();

    if(this->documents.empty()){
        ui->documentsTable->setRowCount(0);
        return;
    }

    ui->documentsTable->setRowCount(this->documents.size());
    ui->documentsTable->setColumnWidth(0, 500);
    for(unsigned int i = 0; i < this->documents.size(); i++){
        QTableWidgetItem *item = new QTableWidgetItem(this->documents[i]->c_str());
        ui->documentsTable->setItem(i, 0, item);
        this->documentTableItem.push_back(item);
    }
}

void menu::on_logOutButton_clicked()
{
    connect_page *c_page = new connect_page();
    c_page->show();
    close();
}
