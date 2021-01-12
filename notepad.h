#ifndef NOTEPAD_H
#define NOTEPAD_H

#include <QWidget>
#include <QString>
#include <string.h>
#include <list>
#include "operation.h"
#include "ServerConnection.h"

using std::list;


namespace Ui {
class Notepad;
}

class Notepad : public QWidget
{
    Q_OBJECT

public:
    explicit Notepad(QWidget *parent = 0);
    ~Notepad();
    string Open(string documentName);

signals:
    void onPeerOperation(); //sends received operation from peer

protected:
    void closeEvent ( QCloseEvent * event )
    {
        this->server->WriteCommand("close");
        this->server->CloseServerConnection();
    }

private:
    static const int PORT = 2000;
    ServerConnection *server;
    QString previousText;
    void ClientThreadLoop();
    list<Operation> toApply;
    list<Operation> history;
    list<Operation> pendingOperations;// operation send to server but not confirmed (without an id)

private slots:

    void slot_onPeerOperation();

    void on_pushButton_clicked();

    void on_textEdit_textChanged();

private:
    Ui::Notepad *ui;
    int prevId;
};

#endif // NOTEPAD_H
