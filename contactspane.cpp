#include "contactspane.h"
#include "ui_contactspane.h"

ContactsPane::ContactsPane(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ContactsPane)
{
    ui->setupUi(this);
}

ContactsPane::~ContactsPane()
{
    delete ui;
}

void ContactsPane::on_pushButton_contactsOk_clicked()
{
    close();
}
