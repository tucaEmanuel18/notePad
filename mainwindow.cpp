#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "connect_page.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect_page *c_page = new connect_page;
    this->setCentralWidget(c_page);
    this->setWindowTitle("Collaborative Notepad");
}

MainWindow::~MainWindow()
{
    delete ui;
}
