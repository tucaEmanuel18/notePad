#ifndef MAINMENU_H
#define MAINMENU_H

#include "documentstable.h"
#include "ServerConnection.h"
#include <QTableWidgetItem>
#include <QWidget>
#include <QTableWidget>
#include <vector>

using std::vector;
using std::string;

class MainMenu : public QWidget
{
    Q_OBJECT
public:
    ~MainMenu() {
        this->server->CloseServerConnection();
    }

    explicit MainMenu(QWidget *parent = 0);
private:

    DocumentsTable * _table;
    vector<QTableWidgetItem*> _documentWidgets;
    vector<string*> _documents;
    ServerConnection *server;
    static const int PORT = 2000;
    //static const int PORT = 8585;


signals:

public slots:
    void onCreatePressed(bool enabled);
    void onOpenPressed(bool enabled);
    void onDeletePressed(bool enabled);
    void refreshTable();
};

#endif // MAINMENU_H
