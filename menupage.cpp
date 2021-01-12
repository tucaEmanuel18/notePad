#include "menupage.h"
#include "ui_menupage.h"
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
#include "menu.h"
#include "notepad.h"

MenuPage::MenuPage(QWidget *parent, ServerConnection* sv) :
    QMainWindow(parent),
    ui(new Ui::MenuPage)
{
    ui->setupUi(this);
    this->menuWidget = new menu(nullptr, sv);
    this->setWindowTitle("Menu");
    this->close();
    this->menuWidget->show();


    //connect(menuWidget, SIGNAL(menuWidget->logOut()), this, SLOT(on_menuLogOut()));
}

MenuPage::~MenuPage()
{
    delete ui;
}

void MenuPage::on_menuLogOut()
{
    close();
}
