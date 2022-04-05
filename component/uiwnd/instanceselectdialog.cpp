#include "instanceselectdialog.h"
#include "ui_instanceselectdialog.h"

InstanceSelectDialog::InstanceSelectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InstanceSelectDialog)
{
    ui->setupUi(this);
}

InstanceSelectDialog::~InstanceSelectDialog()
{
    delete ui;
}

QString InstanceSelectDialog::getFileName()
{
    if(fileName_.split(".").last() != "ist")
    {
        fileName_ += ".ist";
    }
    return fileName_;
}

InstanceSelectDialog::InstanceType InstanceSelectDialog::getInstanceType()
{
   return type_;
}

void InstanceSelectDialog::on_pushButton_ok_clicked()
{
    fileName_ = ui->lineEdit_name->text();
    type_ = (InstanceType)ui->comboBox->currentIndex();
    accept();
}

void InstanceSelectDialog::on_pushButton_cancel_clicked()
{
    reject();
}
