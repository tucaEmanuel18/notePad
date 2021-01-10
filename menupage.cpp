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
//#include "documentstable.h"
#include "menu.h"
#include "notepad.h"

MenuPage::MenuPage(QWidget *parent, ServerConnection* sv) :
    QMainWindow(parent),
    ui(new Ui::MenuPage)
{
    ui->setupUi(this);
    menu *menuWidget = new menu(nullptr, sv);
    this->setCentralWidget(menuWidget);
    this->setWindowTitle("Menu");
}

MenuPage::~MenuPage()
{
    delete ui;
}
