#include "menu.h"
#include "ui_menu.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QString>
#include "connect_page.h"
#include "notepad.h"

menu::menu(QWidget *parent, ServerConnection* sv) : QWidget(parent), ui(new Ui::menu)
{
    ui->setupUi(this);
    this->setWindowTitle("Menu");
    QStringList list;
    list << "Documents";
    ui->documentsTable->setColumnCount(1);
    ui->documentsTable->setColumnWidth(0, 100);
    ui->documentsTable->setHorizontalHeaderLabels(list);
    ui->documentsTable->setRowCount(5);
    ui->documentsTable->setColumnWidth(0, 500);

    connect(ui->logOutButton, SIGNAL(clicked(bool)), this, SLOT(logOutClicked()));
    connect(ui->createButton, SIGNAL(clicked(bool)), this, SLOT(CreateClicked(bool)));
    connect(ui->openButton, SIGNAL(clicked(bool)), this, SLOT(OpenClicked(bool)));
    connect(ui->deleteButton, SIGNAL(clicked(bool)), this, SLOT(DeleteClicked(bool)));
    connect(ui->shareButton, SIGNAL(clicked(bool)), this, SLOT(ShareClicked(bool)));
    connect(ui->refreshButton, SIGNAL(clicked(bool)), this, SLOT(refreshTable()));
    this->server = sv;
    this->refreshTable();
}

menu::~menu()
{
    delete ui;
}

void menu::CreateClicked(bool enabled)
{
    bool wait;
    do {
        bool ok;
        wait=false;
        QString inputResult = QInputDialog::getText(this, "Notepad", "Insert a name for the file!", QLineEdit::Normal, "", &ok);
        if(inputResult != NULL)
        {
            string createCommand = "create ";
            createCommand += inputResult.toStdString();
            this->server->SendCommand(createCommand);

            string response;
            response = this->server->ReceiveCommand();
            std::size_t foundError = response.find("ERROR");
            if(foundError == 0)
            {
                QMessageBox msgForUser;
                msgForUser.critical(0, "Error", "File with the same name already exists! \n Choose another name!");
                continue;
            }
            else
            {
                this->documents.push_back(new std::string (inputResult.toStdString()));
            }
        }
        else if(ok)
        {
            wait = true;
        }
    }while(wait);
    this->refreshTable();
}

void menu::OpenClicked(bool enabled)
{
    QModelIndexList select = ui->documentsTable->selectionModel()->selectedRows();

    if(select.empty())
    {
        QMessageBox msgForUser;
        msgForUser.critical(0, "Error", "Please select a document!");
    }
    else if(select.count() > 1)
    {
        QMessageBox msgForUser;
        msgForUser.critical(0,"Error", "Please select just a document!");
    }
    else
    {
        QModelIndex index = select.at(0);
        int row = index.row();
        string docName = this->documents[row]->c_str();

        Notepad *notepad = new Notepad();
        string response = notepad->Open(docName);
        std::size_t foundError = response.find("ERROR");
        if(foundError == 0)
        {
            QMessageBox msgForUser;
            msgForUser.critical(0, "Error", "File is already edited by two clients!");
            delete notepad;
            return;
        }
        QString docTitle = QString(docName.c_str());
        notepad->setWindowTitle(docTitle);
        notepad->show();
    }
}

void menu::DeleteClicked(bool enabled)
{
    QModelIndexList select = ui->documentsTable->selectionModel()->selectedRows();
    if(select.empty())
    {
        QMessageBox msgForUser;
        msgForUser.critical(0, "Error", "Please select a document!");
    }
    else if(select.count() > 1)
    {
        QMessageBox msgForUser;
        msgForUser.critical(0,"Error", "Please select just a document!");
    }
    else
    {
        string deleteCommand = "remove ";
        QModelIndex index = select.at(0);
        QString docName = ui->documentsTable->model()->data(index).toString();
        deleteCommand += docName.toStdString();
        this->server->SendCommand(deleteCommand);

        int rowIndex = index.row();
        delete this->documents[rowIndex];
        this->documents.erase(this->documents.begin() + rowIndex);
        this->refreshTable();
   }
}

void menu::ShareClicked(bool enabled)
{
    QModelIndexList select = ui->documentsTable->selectionModel()->selectedRows();
    if(select.empty())
    {
        QMessageBox msgForUser;
        msgForUser.critical(0, "Error", "Please select a document!");
    }
    else if(select.count() > 1)
    {
        QMessageBox msgForUser;
        msgForUser.critical(0, "Error", "Please select just a document!");
    }
    else
    {
        bool wait;
        do
        {
            bool ok;
            wait = false;
            QString inputResult = QInputDialog::getText(this, "Share", "Insert the username of the person you want to share the file with!", QLineEdit::Normal, "", &ok);
            if(inputResult != NULL)
            {
                string shareCommand = "share ";
                // obtain the file name
                QModelIndex index = select.at(0);
                QString str = ui->documentsTable->model()->data(index).toString();
                shareCommand += str.toStdString();
                shareCommand += " ";
                // add the friend userName
                shareCommand += inputResult.toStdString();
                this->server->SendCommand(shareCommand);

                string response = this->server->ReceiveCommand();
                size_t foundError = response.find("ERROR");
                if(foundError == 0)
                {
                    QMessageBox msgForUser;
                    char ch = response.at(5);
                    if(ch == '1')
                    {
                        msgForUser.critical(0, "Error", "There is no user with the specified username.!");
                    }
                    else
                    {
                        msgForUser.critical(0, "Error", "The specified user has already access to this file.");
                    }
                }
            }
            else if(ok)
            {
                wait = true;
            }
        }while(wait);
    }
}

void menu::refreshTable()
{
    for(auto doc : this->documents)
    {
        delete doc;
    }
    this->documents.clear();

    int errCod;
    if ((errCod = this->server->SendCommand("list")) != 0)
    {
        QMessageBox msgForUser;
        msgForUser.critical(0,"Error", strerror(errCod));
        return;
    }
    int length = atoi(this->server->ReceiveCommand().c_str());
    for(int i = 0; i < length; ++i)
    {
        string docName = this->server->ReceiveCommand();
        string * ptrDocName = new string;
        ptrDocName->append(docName);
        this->documents.push_back(ptrDocName);
    }

    for(auto w : this->documentTableItem)
    {
        delete w; // w - widget
    }
    this->documentTableItem.clear();

    if(this->documents.empty())
    {
        ui->documentsTable->setRowCount(0);
        return;
    }

    ui->documentsTable->setRowCount(this->documents.size());
    ui->documentsTable->setColumnWidth(0, 500);
    for(unsigned int i = 0; i < this->documents.size(); i++)
    {
        QTableWidgetItem *tableItem = new QTableWidgetItem(this->documents[i]->c_str());
        ui->documentsTable->setItem(i, 0, tableItem);
        this->documentTableItem.push_back(tableItem);
    }
}

void menu::logOutClicked()
{
    connect_page *c_page = new connect_page();
    c_page->show();
    close();
}
