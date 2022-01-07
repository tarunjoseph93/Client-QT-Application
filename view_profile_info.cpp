#include "view_profile_info.h"
#include "ui_view_profile_info.h"

View_Profile_Info::View_Profile_Info(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::View_Profile_Info)
{
    ui->setupUi(this);
}

View_Profile_Info::~View_Profile_Info()
{
    delete ui;
}

void View_Profile_Info::on_pushButton_closeProfile_clicked()
{
    close();
}
