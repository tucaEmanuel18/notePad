#ifndef MENUPAGE_H
#define MENUPAGE_H

#include <QMainWindow>
#include "ServerConnection.h"
#include <QTableWidgetItem>
#include <QTableWidget>
#include <vector>
#include <menu.h>

using std::vector;
using std::string;

namespace Ui {
class MenuPage;
}

class MenuPage : public QMainWindow
{
    Q_OBJECT

public:
    explicit MenuPage(QWidget *parent = nullptr, ServerConnection* sv = nullptr);
    ~MenuPage();

private:
    Ui::MenuPage *ui;
    menu *menuWidget;
public:
signals:
    void logOut();

private slots:
   void on_menuLogOut();
};

#endif // MENUPAGE_H
