#include "client_interface.h"
#include "ui_client_interface.h"

Client_Interface::Client_Interface(QWidget *parent) : QWidget(parent), ui(new Ui::Client_Interface), client_socket(new QTcpSocket(this))
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);

    chatModel = new QStandardItemModel(this);

    chatModel->insertColumn(0);

    ui->listView_chatView->setModel(chatModel);

    connect(client_socket, &QTcpSocket::disconnected, this, &Client_Interface::disconnectedSocket);
    connect(client_socket, &QTcpSocket::readyRead, this, &Client_Interface::onReadyRead);
    connect(client_socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &Client_Interface::error);
    connect(ui->pushButton_sendButton, &QPushButton::clicked, this, &Client_Interface::sendPrivateMessage);
    ui->lineEdit_chatBox->setEnabled(true);
    ui->pushButton_sendButton->setEnabled(true);
}

Client_Interface::~Client_Interface()
{
    delete ui;
}

void Client_Interface::onReadyRead()
{
    QString signal;
    int command = 0;

    QByteArray data = client_socket->readAll();
    qDebug() << "Data In: " << data;

    QStringList dataRead = QString(data).split(":");
    qDebug() <<"Read Data: " << dataRead;

    signal = dataRead[0];

    if(signal == "LOG_SUC")
        command = 1;
    else if (signal == "LOG_FAIL")
        command = 2;

    else if (signal == "LOG_DUP")
        command = 3;

    else if (signal == "ACTIVE_USERS")
        command = 4;

    else if (signal == "PRIV_MSG_RCV")
        command = 5;

//    else if (signal == "PRIV_MSG_FAIL")
//        command = 6;

//    else if (signal == "PRIV_MSG_PASS")
//        command = 7;

    switch(command)
    {
    case 1:
    {
        login_Success(dataRead[1],dataRead[2]);
        break;
    }
    case 2:
    {
        login_Fail(dataRead[1],dataRead[2]);
        break;
    }
    case 3:
    {
        login_Dup(dataRead[1],dataRead[2]);
        break;
    }
    case 4:
    {
        listActiveUsers(dataRead);
        break;
    }
    case 5:
    {
        readPrivateMessage(dataRead[1],dataRead[2]);
        break;
    }

    }
}

void Client_Interface::login_Success(QString &uName, QString &status)
{
    QString username = uName;
    QString stat = status;

    qDebug() << "Username: " << username << "\nStatus: " << stat;
    setUserName(username);
    display_homePage();
}

void Client_Interface::login_Fail(QString &creds, QString &status)
{
    QString cred = creds;
    QString stat = status;

    qDebug() << "Credentials: " << cred << "\nStatus: " << stat;
    display_loginPage();
}

void Client_Interface::login_Dup(QString &creds, QString &status)
{
    QString cred = creds;
    QString stat = status;

    qDebug() << "Credentials: " << cred << "\nStatus: " << stat << "\nDuplicate found.";
    display_loginPage();
}

void Client_Interface::setUserName(QString &username)
{
    uname = username;
}


QString Client_Interface::getUserName(QString username)
{
    return uname;
}

void Client_Interface::display_welcomePage()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void Client_Interface::display_loginPage()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void Client_Interface::display_homePage()
{
    getActiveUsers();
    ui->stackedWidget->setCurrentIndex(2);
}


void Client_Interface::on_pushButton_connect_clicked()
{
    const QString hname = ui->lineEdit_hostAddress->text();
    const quint16 port = ui->lineEdit_portNumber->text().toUInt();

    if (hname.isEmpty())
    {
        QMessageBox::critical(this, tr("Error"), tr("Host Address textbox is empty!\nEnter Host Address (Try 127.0.0.1)."));
        ui->stackedWidget->setCurrentIndex(0);
        return;
    }
    else if(port == 0)
    {
        QMessageBox::critical(this, tr("Error"), tr("Port Number textbox is empty!\nEnter Port Number (Try 9000)."));
        ui->stackedWidget->setCurrentIndex(0);
        return;
    }
    else if (hname.isEmpty() && port == 0)
    {
        QMessageBox::critical(this, tr("Error"), tr("Either Host Address and Port Number textbox is empty!\nEnter Host Address (Try 127.0.0.1) and Port Number (Try 9000)."));
        ui->stackedWidget->setCurrentIndex(0);
        return;
    }
    else if (hname != "127.0.0.1")
    {
        QMessageBox::critical(this, tr("Error"), tr("Host Address is wrong!\nEnter the correct Host Address (Try 127.0.0.1)."));
        ui->stackedWidget->setCurrentIndex(0);
        return;
    }
    else if (port != 9000)
    {
        QMessageBox::critical(this, tr("Error"), tr("Port Number is wrong!\nEnter the correct Port Number (Try 9000)."));
        ui->stackedWidget->setCurrentIndex(0);
        return;
    }
    else
    {
        client_socket->connectToHost(QHostAddress(hname), port);
        display_loginPage();
    }
}

void Client_Interface::disconnectedSocket()
{
    if(client_socket->state() != QAbstractSocket::ConnectedState)
    {
        QMessageBox::critical(this, tr("Error"), tr("Disconnected from host."));
    }
    display_welcomePage();
    client_socket->close();
}

void Client_Interface::on_pushButton_login_clicked()
{
    uname = ui->lineEdit_userName->text();
    password = ui->lineEdit__password->text();

    if(uname.isEmpty())
    {
        QMessageBox::critical(this, tr("Error"), tr("Username field is empty.\nEnter your username."));
        display_loginPage();
        return;
    }
    else if(password.isEmpty())
    {
        QMessageBox::critical(this, tr("Error"), tr("Password field is empty.\nEnter your password."));
        display_loginPage();
        return;
    }
    else if(uname.size() > 10)
    {
        QMessageBox::critical(this, tr("Error"), tr("Username field is only 10 characters long.\nEnter your username again."));
        display_loginPage();
        return;
    }
    else if(password.size() > 10)
    {
        QMessageBox::critical(this, tr("Error"), tr("Password field is only 10 characters long.\nEnter your password again."));
        display_loginPage();
        return;
    }
    else
    {
        checkLoginCreds(uname,password);
    }
}

void Client_Interface::checkLoginCreds(QString &uname, QString &password)
{
    QByteArray ba;

    ba.append("LOGIN_CHECK:" + uname.toUtf8() + ":" + password.toUtf8());

    qDebug() << client_socket->socketDescriptor() << "Data Out: " << ba;
    client_socket->write(ba);
}

void Client_Interface::getActiveUsers()
{
    QByteArray ba;
    ba.append("GETACTIVEUSERS:");
    client_socket->write(ba);
}

void Client_Interface::listActiveUsers(QStringList &data)
{
    QString activeUsers = data.join(":");
    qDebug() << "Active Users String: " << activeUsers;
    QStringList activeUsersList = activeUsers.split(":");
    activeUsersList.removeAt(0);

    ui->listWidget_usersActive->addItems(activeUsersList);
}

void Client_Interface::sendPrivateMessage()
{
    privateReceiver = ui->listWidget_usersActive->currentItem()->text();
    QString text = ui->lineEdit_chatBox->text();
    if(text.isEmpty())
    {
        return;
    }

    QByteArray ba;
    ba.append("PRIVMES:" + uname.toUtf8() + ":" + privateReceiver.toUtf8() + ":" + text.toUtf8());
    qDebug() << "Private message data being sent from: " << uname << " to: " << privateReceiver;
    client_socket->write(ba);

    const int newRow = chatModel->rowCount();
    chatModel->insertRow(newRow);
    chatModel->insertRow(newRow);
    chatModel->setData(chatModel->index(newRow, 0), text);
    chatModel->setData(chatModel->index(newRow, 0), int(Qt::AlignRight | Qt::AlignVCenter), Qt::TextAlignmentRole);
    ui->lineEdit_chatBox->clear();
    ui->listView_chatView->scrollToBottom();
    lastUname.clear();
}


void Client_Interface::readPrivateMessage(QString &sender, QString &text)
{

    //START HERE!
    QString user = ui->listWidget_usersActive->currentItem()->text();
    if(user == sender)
    {
        qDebug() << "Received message from: " << sender << ": " << text;
        int newRow = chatModel->rowCount();
        if (lastUname != sender)
        {
            lastUname = sender;
            QFont boldFont;
            boldFont.setBold(true);
            chatModel->insertRows(newRow, 2);
            chatModel->setData(chatModel->index(newRow, 0), sender + QLatin1Char(':'));
            chatModel->setData(chatModel->index(newRow, 0), int(Qt::AlignLeft | Qt::AlignVCenter), Qt::TextAlignmentRole);
            chatModel->setData(chatModel->index(newRow, 0), boldFont, Qt::FontRole);
            ++newRow;
        } else {
            chatModel->insertRow(newRow);
        }
        chatModel->setData(chatModel->index(newRow, 0), text);
        chatModel->setData(chatModel->index(newRow, 0), int(Qt::AlignLeft | Qt::AlignVCenter), Qt::TextAlignmentRole);
        ui->listView_chatView->scrollToBottom();
    }
    else
    {
        qDebug() << sender + " has not been selected.";
    }
}

//void Client_Interface::privateChatFail(QString &sender, QString &receiver)
//{
//    QMessageBox::critical(this, tr("Error"), tr("Client is not online. Failed to start private chat."));
//    qDebug() << "Failed to start private chat from: " << sender << " to: " << receiver;
//}


void Client_Interface::on_pushButton_refreshList_clicked()
{
//    getActiveUsers();
}

void Client_Interface::error(QAbstractSocket::SocketError socketError)
{
    // show a message to the user that informs of what kind of error occurred
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
    case QAbstractSocket::ProxyConnectionClosedError:
        return; // handled by disconnectedFromServer
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::critical(this, tr("Error"), tr("The host refused the connection"));
        break;
    case QAbstractSocket::ProxyConnectionRefusedError:
        QMessageBox::critical(this, tr("Error"), tr("The proxy refused the connection"));
        break;
    case QAbstractSocket::ProxyNotFoundError:
        QMessageBox::critical(this, tr("Error"), tr("Could not find the proxy"));
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::critical(this, tr("Error"), tr("Could not find the server"));
        break;
    case QAbstractSocket::SocketAccessError:
        QMessageBox::critical(this, tr("Error"), tr("You don't have permissions to execute this operation"));
        break;
    case QAbstractSocket::SocketResourceError:
        QMessageBox::critical(this, tr("Error"), tr("Too many connections opened"));
        break;
    case QAbstractSocket::SocketTimeoutError:
        QMessageBox::warning(this, tr("Error"), tr("Operation timed out"));
        return;
    case QAbstractSocket::ProxyConnectionTimeoutError:
        QMessageBox::critical(this, tr("Error"), tr("Proxy timed out"));
        break;
    case QAbstractSocket::NetworkError:
        QMessageBox::critical(this, tr("Error"), tr("Unable to reach the network"));
        break;
    case QAbstractSocket::UnknownSocketError:
        QMessageBox::critical(this, tr("Error"), tr("An unknown error occured"));
        break;
    case QAbstractSocket::UnsupportedSocketOperationError:
        QMessageBox::critical(this, tr("Error"), tr("Operation not supported"));
        break;
    case QAbstractSocket::ProxyAuthenticationRequiredError:
        QMessageBox::critical(this, tr("Error"), tr("Your proxy requires authentication"));
        break;
    case QAbstractSocket::ProxyProtocolError:
        QMessageBox::critical(this, tr("Error"), tr("Proxy comunication failed"));
        break;
    case QAbstractSocket::TemporaryError:
    case QAbstractSocket::OperationError:
        QMessageBox::warning(this, tr("Error"), tr("Operation failed, please try again"));
        return;
    default:
        Q_UNREACHABLE();
    }
}




void Client_Interface::on_pushButton_register_clicked()
{
    Registration reg;
    reg.setModal(true);
    reg.exec();
}

void Client_Interface::on_pushButton_logOut_clicked()
{
    client_socket->close();
    display_welcomePage();
}

void Client_Interface::on_pushButton_profileInfo_clicked()
{
    profInfo = new View_Profile_Info(this);
    profInfo->show();
}
