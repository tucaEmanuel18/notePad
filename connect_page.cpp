#include "connect_page.h"
#include "ui_connect_page.h"
#include <QMessageBox>
#include "menu.h"

connect_page::connect_page(QWidget *parent) : QWidget(parent), ui(new Ui::connect_page)
{
    ui->setupUi(this);
    this->server = new ServerConnection(connect_page::PORT);
    this->setWindowTitle("Collaborative Notepad");
    int cod;
    if((cod = this->server->Connect()) != 0)
    {
        // if the connection fail throw error and close window
        QMessageBox msgForUser;
        msgForUser.critical(0,"Error",strerror(cod));
        return;
    }
}

connect_page::~connect_page()
{
    delete ui;
    this->server->CloseServerConnection();
}

void connect_page::on_signButton_clicked()
{
    QString userName = ui->userLineEdit->text();
    QString password = ui->passwordLineEdit->text();

    if(userName.length() < 6 || password.length() < 6)
    {
        QMessageBox::information(this, "SignIn", "Username and password must be at least 6 characters long!");
    }
    else
    {
        string signCommand = "sign ";
        signCommand += userName.toStdString();
        signCommand += " ";
        signCommand += password.toStdString();
        this->server->SendCommand(signCommand);

        string response;
        response = this->server->ReceiveCommand();
        string errorMessage = "ERROR";
        std::size_t foundError = response.find(errorMessage);

        if(foundError == 0)
        {
            QMessageBox msgForUser;
            msgForUser.critical(0, "Error", " There is already an account with this name! \n Choose another name!");
        }
        else
        {
            QMessageBox::information(this, "SignIn", "The new account was created!");
        }
    }
}

void connect_page::on_logButton_clicked()
{
    QString userName = ui->userLineEdit->text();
    QString password = ui->passwordLineEdit->text();

    if(userName.length() < 6 || password.length() < 6)
    {
        QMessageBox::information(this, "LogIn", "Username and password must be at least 6 characters long!");
    }
    else
    {

        string logCommand = "log ";
        logCommand += userName.toStdString();
        logCommand += " ";
        logCommand += password.toStdString();
        this->server->SendCommand(logCommand);

        string response;
        response = this->server->ReceiveCommand();
        std::size_t foundError = response.find("ERROR");

        if(foundError == 0)
        {
            char ch = response[5];
            QMessageBox msgForUser;
            if(ch == '1')
            {
                msgForUser.critical(0, "Error", " This name doesn't exists! \n If you don't have accont please sign-in!");
            }
            else
            {
                msgForUser.critical(0, "Error", " The password is wrong!");
            }
        }
        else
        {
            menu *menuWidget = new menu(nullptr, this->server);
            menuWidget->show();
            hide();
        }
    }
}


