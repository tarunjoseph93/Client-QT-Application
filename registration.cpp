#include "registration.h"
#include "ui_registration.h"

Registration::Registration(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Registration)
{
    ui->setupUi(this);
}

Registration::~Registration()
{
    delete ui;
}

void Registration::on_pushButton_registerOk_clicked()
{
    QString firstName = ui->lineEdit_firstName->text();
    QString lastName = ui->lineEdit_lastName->text();
    QString username = ui->lineEdit_userName->text();
    QString password = ui->lineEdit_password->text();
    QString age =ui->lineEdit_age->text();
    QString city = ui->lineEdit_city->text();
    QString sex = ui->lineEdit_sex->text();

    if(firstName.isEmpty() || lastName.isEmpty() || username.isEmpty() || password.isEmpty() || age.isEmpty() || city.isEmpty() || sex.isEmpty())
    {
        QMessageBox::critical(this, tr("All fields required"), tr("Some fields are empty. Please enter all details to register yourself!"));
        return;
    }
    else
    {
        qDebug() << firstName << lastName << username << password << age << city << sex;
        emit newRegister(firstName,lastName,username,password,age,city,sex);
    }

    close();
}

void Registration::on_pushButton_Cancel_clicked()
{
    close();
}
