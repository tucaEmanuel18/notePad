#ifndef CONNECT_PAGE_H
#define CONNECT_PAGE_H
#include "ServerConnection.h"
#include <QTableWidgetItem>
#include <QWidget>
#include <QTableWidget>
#include <vector>

using std::vector;
using std::string;

namespace Ui {
class connect_page;
}

class connect_page : public QWidget
{
    Q_OBJECT

public:
    explicit connect_page(QWidget *parent = nullptr);
    ~connect_page();

private:
    Ui::connect_page *ui;
    ServerConnection *server;
    static const int PORT = 2000;

private slots:
    void on_logButton_clicked();
    void on_signButton_clicked();
};
#endif // CONNECT_PAGE_H
