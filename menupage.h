#ifndef MENUPAGE_H
#define MENUPAGE_H

#include <QMainWindow>
#include "documentstable.h"
#include "ServerConnection.h"
#include <QTableWidgetItem>
#include <QTableWidget>
#include <vector>

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
};

#endif // MENUPAGE_H
