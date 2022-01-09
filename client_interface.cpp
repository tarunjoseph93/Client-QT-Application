#include "client_interface.h"
#include "ui_client_interface.h"

Client_Interface::Client_Interface(QWidget *parent) : QWidget(parent), ui(new Ui::Client_Interface), client_socket(new QTcpSocket(this)), registerInfo(new Registration(this))
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
    connect(registerInfo, &Registration::newRegister, this, &Client_Interface::newRegister);
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

    else if (signal == "PROF_INFO")
        command = 6;

    else if (signal == "GROUP_CHAT_CREATED:")
        command = 7;

    else if (signal == "GROUP_CHAT_ADD:")
        command = 8;
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
    case 6:
    {
        getProfInfo(dataRead);
        break;
    }
    case 7:
    {
        setGroupChatCreate();
        break;
    }
    case 8:
    {
        setGroupChatAdd();
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
    QString header = "Welcome " + uname + " to your Chat Client!";
    ui->label_userNameHeader->setText(header);
}

void Client_Interface::newRegister(QString &firstName,QString &lastName,QString &username,QString &password,QString &age,QString &city,QString &sex)
{
    qDebug() << "New registration details: " << firstName << lastName << username << password << age << city << sex;
    QByteArray ba;
    ba.append("REGISTER:" + firstName.toUtf8() + ":" + lastName.toUtf8() + ":" + username.toUtf8() + ":" + password.toUtf8() + ":" + age.toUtf8() + ":" + city.toUtf8() + ":" + sex.toUtf8());
    client_socket->write(ba);
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
    for(int i=0;i<activeUsersList.length();i++)
    {
        if(activeUsersList[i] == uname)
        {
            activeUsersList.removeAt(i);
        }
        else
            continue;
    }

    ui->listWidget_usersActive->addItems(activeUsersList);
}

//void Client_Interface::getProfInfo(QStringList &data)
//{
//    QString profInfo = data.join(":");
//    qDebug() << "Profile Info: " << profInfo;
//    QStringList profInfoList = profInfo.split(":");
//    profInfoList.removeAt(0);
//    emit myProfileInfo(profInfoList);

//}

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
    registerInfo->setModal(true);
    registerInfo->exec();
}

void Client_Interface::on_pushButton_logOut_clicked()
{
    client_socket->close();
    display_welcomePage();
}

void Client_Interface::on_pushButton_profileInfo_clicked()
{
    QByteArray ba;
    ba.append("GET_PROF_INFO:");
    client_socket->write(ba);
    ui->stackedWidget->setCurrentIndex(3);
}

void Client_Interface::getProfInfo(QStringList &data)
{
    QString profInfo = data.join(":");
    qDebug() << "Profile Info: " << profInfo;
    QStringList profInfoList = profInfo.split(":");
    profInfoList.removeAt(0);
    QString firstName = profInfoList[0];
    QString lastName = profInfoList[1];
    QString userName = profInfoList[2];
    QString password = profInfoList[3];
    QString age = profInfoList[4];
    QString city = profInfoList[5];
    QString sex = profInfoList[6];

    qDebug() << firstName << lastName << userName << password << age << city << sex;

    ui->lineEdit_profInfoFirstName->setPlaceholderText(firstName);
    ui->lineEdit_profInfoLastName->setPlaceholderText(lastName);
    ui->lineEdit_profInfoUserName->setPlaceholderText(userName);
    ui->lineEdit_profInfoPassword->setPlaceholderText(password);
    ui->lineEdit_profInfoAge->setPlaceholderText(age);
    ui->lineEdit_profInfoCity->setPlaceholderText(city);
    ui->lineEdit_profInfoSex->setPlaceholderText(sex);

}



void Client_Interface::on_pushButton_edit_clicked()
{
    QString firstName = ui->lineEdit_profInfoFirstName->text();
    QString lastName = ui->lineEdit_profInfoLastName->text();
    QString username = ui->lineEdit_profInfoUserName->text();
    QString password = ui->lineEdit_profInfoPassword->text();
    QString age =ui->lineEdit_profInfoAge->text();
    QString city = ui->lineEdit_profInfoCity->text();
    QString sex = ui->lineEdit_profInfoSex->text();

    if(firstName.isEmpty() || lastName.isEmpty() || username.isEmpty() || password.isEmpty() || age.isEmpty() || city.isEmpty() || sex.isEmpty())
    {
        QMessageBox::critical(this, tr("All fields required"), tr("Some fields are empty. If no edit done, please close."));
    }
    else
    {
        qDebug() << firstName << lastName << username << password << age << city << sex;
        QByteArray ba;
        ba.append("NEW_PROF_INFO:" + firstName.toUtf8() + ":" + lastName.toUtf8() + ":" + username.toUtf8() + ":" + password.toUtf8() + ":" + age.toUtf8() + ":" + city.toUtf8() + ":" + sex.toUtf8());
        client_socket->write(ba);
    }
    ui->stackedWidget->setCurrentIndex(2);
}



void Client_Interface::on_pushButton_closeProfile_clicked()
{
    display_homePage();
}

void Client_Interface::on_pushButton_viewContacts_clicked()
{
    QByteArray ba;
    ba.append("GET_CONTACTS:");
    client_socket->write(ba);
    ui->stackedWidget->setCurrentIndex(4);
}

void Client_Interface::on_pushButton_groupChat_clicked()
{
    QString groupChatName = QInputDialog::getText(
            this
            , tr("Enter Group Chat Name")
            , tr("Group Chat Name")
            , QLineEdit::Normal
        );
    qDebug() << "Group Chat Name: " << groupChatName;
    QByteArray ba;
    ba.append("GROUP_CHAT_REQ:" + groupChatName.toUtf8());
    qDebug() << ba;
    client_socket->write(ba);
}

void Client_Interface::setGroupChatCreate()
{
//    QMessageBox::information(this,tr("Group Chat Created"),tr("Created a group chat"));
    groupChat = true;
    qDebug() << "Grooup Chat Flag: " << groupChat;
}

void Client_Interface::setGroupChatAdd()
{
//    QMessageBox::information(this,tr("Added to group Chat"),tr("You've been added to the chat!"));
    groupChat = true;
    qDebug() << "Grooup Chat Flag: " << groupChat;
}

