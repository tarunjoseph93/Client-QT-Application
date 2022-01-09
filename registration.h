#ifndef REGISTRATION_H
#define REGISTRATION_H

#include <QDialog>
#include <QMessageBox>
#include <QDebug>

#include "client_interface.h"

namespace Ui {
class Registration;
}

class Registration : public QDialog
{
    Q_OBJECT

public:
    explicit Registration(QWidget *parent);
    ~Registration();

signals:
    void newRegister(QString &firstName,QString &lastName,QString &username,QString &password,QString &age,QString &city,QString &sex);

private slots:
    void on_pushButton_registerOk_clicked();

    void on_pushButton_Cancel_clicked();

private:
    Ui::Registration *ui;
};

#endif // REGISTRATION_H
