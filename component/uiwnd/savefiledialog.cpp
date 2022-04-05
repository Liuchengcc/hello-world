#include "savefiledialog.h"
#include "ui_savefiledialog.h"
#include "qlabel.h"
#include "widget/file/zfilewidget.h"

SaveFileDialog::SaveFileDialog(const QString& header, const QString&fileName, QWidget *parent):
    QDialog(parent),
    ui(new Ui::SaveFileDialog)
{
    ui->setupUi(this);
    ui->label_notify->setText(tr("是否保存文件 %1？").arg(fileName));
    this->setWindowTitle(header);
}

void SaveFileDialog::setDiffData(QByteArray origin, QByteArray modified)
{
    originData_ = origin;
    modifiedData_ = modified;
}

SaveFileDialog::~SaveFileDialog()
{
    delete ui;
}

QStringList SaveFileDialog::getDialogData()
{
    QStringList dataList;
    for(int i = 0; i < lineditList_.size(); i++)
    {
        dataList.append(lineditList_[i]->text());
    }
    return dataList;
}

void SaveFileDialog::on_pushButton_ok_clicked()
{
    accept();
}

void SaveFileDialog::on_pushButton_cancel_clicked()
{
    reject();
}

#define ORG_FILE_PATH "./res/temporgfile"
#define DST_FILE_PATH "./res/tempdstfile"
void SaveFileDialog::on_pushButton_checkModify_clicked()
{
    QFile tempOrgFile(ORG_FILE_PATH);
    QFile tempDstFile(DST_FILE_PATH);
    tempOrgFile.open(QIODevice::WriteOnly|QIODevice::Text);
    tempDstFile.open(QIODevice::WriteOnly|QIODevice::Text);
    tempOrgFile.write(originData_);
    tempDstFile.write(modifiedData_);
    tempOrgFile.close();
    tempDstFile.close();
    ZPathDiffModel pathDiffModel;
    pathDiffModel.setSrcFileInfo(QFileInfo(tempOrgFile));
    pathDiffModel.setDstFileInfo(QFileInfo(tempDstFile));
    ZFileWidget *widget = new ZFileWidget(pathDiffModel);
    widget->resize(1024, 768);
    widget->show();
//     widget->compare();

}
