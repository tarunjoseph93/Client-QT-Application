#ifndef CLIENT_INTERFACE_H
#define CLIENT_INTERFACE_H

#include <QtCore>
#include <QtGui>
#include <QWidget>
#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QMessageBox>
#include <QDebug>
#include <QStandardItemModel>
#include <QAbstractSocket>
#include <QInputDialog>

#include "registration.h"

class QStandardItemModel;
class Registration;

QT_BEGIN_NAMESPACE
namespace Ui { class Client_Interface; }
QT_END_NAMESPACE

class Client_Interface : public QWidget
{
    Q_OBJECT

public:
    Client_Interface(QWidget *parent = nullptr);
    ~Client_Interface();
    void disconnectedSocket();
    void display_homePage();
    void display_loginPage();
    void display_welcomePage();
    void listActiveUsers(QStringList &data);
    void setUserName(QString &username);

public slots:
    void login_Success(QString &username, QString &status);
    void login_Fail(QString &creds, QString &status);
    void login_Dup(QString &creds, QString &status);

private slots:
    void on_pushButton_connect_clicked();
    void error(QAbstractSocket::SocketError socketError);
    void checkLoginCreds(QString &uname, QString &password);
    void on_pushButton_login_clicked();
    void onReadyRead();
    void getActiveUsers();
    void sendPrivateMessage();
    void readPrivateMessage(QString &sender, QString &text);
    void newRegister(QString &firstName,QString &lastName,QString &username,QString &password,QString &age,QString &city,QString &sex);
    void getProfInfo(QStringList &data);
    void getContactList(QStringList &data);
    void on_pushButton_refreshList_clicked();
    void on_pushButton_register_clicked();
    void on_pushButton_logOut_clicked();
    void on_pushButton_profileInfo_clicked();
    void on_pushButton_viewContacts_clicked();
    void on_pushButton_edit_clicked();
    void on_pushButton_closeProfile_clicked();
    void on_pushButton_closeContactPane_clicked();

signals:
    void myProfileInfo(QStringList &list);

private:
    Ui::Client_Interface *ui;
    QTcpSocket *client_socket;
    QString uname;
    QString password;
    QString lastUname;
    QStandardItemModel *chatModel;
    QString privateReceiver;
    Registration *registerInfo;

};
#endif // CLIENT_INTERFACE_H
