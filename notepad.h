#ifndef NOTEPAD_H
#define NOTEPAD_H

#include <QWidget>
#include <QString>
#include <string.h>
#include <list>
#include "operation.h"
#include "ServerConnection.h"

using namespace std;


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
    void signal_peerOp();

protected:
    void closeEvent ( QCloseEvent * event )
    {
        this->server->WriteCommand("close");
        this->server->CloseServerConnection();
    }
private:
    static const int PORT = 2000;
    ServerConnection *server;
    QString previousContent;
    list<Operation> toApply;
    list<Operation> history;
    list<Operation> waitingOpList; // in this list we will retain the operations send to server but not confirmed (without an id)
    void PeerThreadLoop();

private slots:
    void on_editBox_textChanged();
    void on_downloadButton();

    void slot_peerOp();


private:
    int lastId;
    Ui::Notepad *ui;

};

#endif // NOTEPAD_H
