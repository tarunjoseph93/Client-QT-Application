#ifndef CONTACTSPANE_H
#define CONTACTSPANE_H

#include <QDialog>

namespace Ui {
class ContactsPane;
}

class ContactsPane : public QDialog
{
    Q_OBJECT

public:
    explicit ContactsPane(QWidget *parent = nullptr);
    ~ContactsPane();

private slots:
    void on_pushButton_contactsOk_clicked();

private:
    Ui::ContactsPane *ui;
};

#endif // CONTACTSPANE_H
