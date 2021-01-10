#ifndef MENU_H
#define MENU_H

#include <QWidget>
#include "documentstable.h"
#include "ServerConnection.h"
#include <QTableWidgetItem>
#include <vector>

using std::vector;
using std::string;
namespace Ui {
class menu;
}

class menu : public QWidget
{
    Q_OBJECT

public:
    explicit menu(QWidget *parent = nullptr, ServerConnection *sv = nullptr);
    ~menu();

private:
    Ui::menu *ui;
    vector<QTableWidgetItem*> documentTableItem;
    vector<string*> documents;
    ServerConnection *server;
    static const int PORT = 2000;


signals:

public slots:
    void onCreatePressed(bool enabled);
    void onOpenPressed(bool enabled);
    void onDeletePressed(bool enabled);
    void onSharePressed(bool enabled);
    void refreshTable();
};

#endif // MENU_H
