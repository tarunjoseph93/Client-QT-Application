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
    close();
}

void Registration::on_pushButton_Cancel_clicked()
{
    close();
}
