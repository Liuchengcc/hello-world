#include "commondialog.h"
#include "ui_commondialog.h"
#include "qlabel.h"

CommonDialog::CommonDialog(const QString& header, const QStringList& keyList, QWidget *parent):
    QDialog(parent),
    ui(new Ui::CommonDialog)
{
    ui->setupUi(this);
    this->setWindowTitle(header);
    for(int i = 0; i < keyList.size(); i++)
    {
        QLabel* tempLabel = new QLabel(keyList[i], this);
        ui->gridLayout_main->addWidget(tempLabel,i,0);
        QLineEdit* edit = new QLineEdit(this);
        lineditList_.append(edit);
        ui->gridLayout_main->addWidget(edit, i, 1);
    }
}

CommonDialog::~CommonDialog()
{
    delete ui;
}

QStringList CommonDialog::getDialogData()
{
    QStringList dataList;
    for(int i = 0; i < lineditList_.size(); i++)
    {
        dataList.append(lineditList_[i]->text());
    }
    return dataList;
}

void CommonDialog::on_pushButton_ok_clicked()
{
    accept();
}

void CommonDialog::on_pushButton_cancel_clicked()
{
    reject();
}
