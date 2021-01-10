#include "connect_page.h"
#include "ui_connect_page.h"
#include "menupage.h"
#include <QMessageBox>

connect_page::connect_page(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::connect_page)
{
    ui->setupUi(this);
    this->setWindowTitle("New Document");
    this->server = new ServerConnection(connect_page::PORT);

    int code;
    if((code = this->server->Connect()) != 0)
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Error",strerror(code));
        // close this window
        return;
    }
}

connect_page::~connect_page()
{
    this->server->CloseServerConnection();
    delete ui;
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
        string createCommand = "sign ";
        createCommand += userName.toStdString();
        createCommand += " ";
        createCommand += password.toStdString();
        this->server->WriteCommand(createCommand);

        string answer;
        answer = this->server->ReadCommand();
        string errorMessage = "ERROR";
        std::size_t found = answer.find(errorMessage);

        if(found == 0)
        {
            QMessageBox messageBox;
            messageBox.critical(0, "Error", " There is already an account with this name! \n Choose another name!");
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

        string createCommand = "log ";
        createCommand += userName.toStdString();
        createCommand += " ";
        createCommand += password.toStdString();
        this->server->WriteCommand(createCommand);

        string answer;
        answer = this->server->ReadCommand();
        string errorMessage = "ERROR";
        std::size_t found = answer.find(errorMessage);

        if(found == 0)
        {
            char ch = answer[5];
            QMessageBox messageBox;
            if(ch == '1')
            {
                messageBox.critical(0, "Error", " This name doesn't exists! \n If you don't have accont please sign-in!");
            }
            else
            {
                messageBox.critical(0, "Error", " The password is wrong!");

            }
        }
        else
        {
            MenuPage *menu = new MenuPage(nullptr, this->server);
            menu->show();
        }

    }
}


