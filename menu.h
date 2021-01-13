#ifndef MENU_H
#define MENU_H
#include <QWidget>
#include <QTableWidgetItem>
#include <vector>
#include "ServerConnection.h"

using namespace std;
namespace Ui
{
class menu;
}

class menu : public QWidget
{
    Q_OBJECT

public:
    explicit menu(QWidget *parent = nullptr, ServerConnection *sv = nullptr, string userName = nullptr);
    ~menu();

private:
    Ui::menu *ui;
    ServerConnection *server;
    static const int PORT = 2000;
    vector<QTableWidgetItem*> documentTableItem;
    vector<string*> documents;


public slots:
    void logOutClicked();
    void CreateClicked(bool enabled);
    void OpenClicked(bool enabled);
    void DeleteClicked(bool enabled);
    void ShareClicked(bool enabled);
    void refreshTable();

};

#endif // MENU_H
