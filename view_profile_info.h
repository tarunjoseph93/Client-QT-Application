#ifndef VIEW_PROFILE_INFO_H
#define VIEW_PROFILE_INFO_H

#include <QDialog>

namespace Ui {
class View_Profile_Info;
}

class View_Profile_Info : public QDialog
{
    Q_OBJECT

public:
    explicit View_Profile_Info(QWidget *parent = nullptr);
    ~View_Profile_Info();

private slots:
    void on_pushButton_closeProfile_clicked();

private:
    Ui::View_Profile_Info *ui;
};

#endif // VIEW_PROFILE_INFO_H
