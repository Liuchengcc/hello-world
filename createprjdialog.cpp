#include "createprjdialog.h"
#include "ui_createprjdialog.h"
#include "qfiledialog.h"
#include "globalfunction.h"

CreatePrjDialog::CreatePrjDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreatePrjDialog)
{
    ui->setupUi(this);
    connect(ui->pushButton_selectPath, &QPushButton::clicked, [this]
    {
        QString dirPath = QFileDialog::getExistingDirectory(this, tr("选择目录"), tr("C:/"));
        if(!dirPath.isEmpty()){ui->lineEdit_path->setText(dirPath);}
    });
    connect(ui->pushButton_cancel, &QPushButton::clicked, [this]
    {
        reject();
    });
    connect(ui->pushButton_ok, &QPushButton::clicked, [this]
    {
        if(!GlobalFunction::isNameValid(ui->lineEdit_name->text())){GlobalFunction::showLogMsg("FUCK NAME!");return;};
        if(ui->lineEdit_path->text().isEmpty()){GlobalFunction::showLogMsg("FUCK PATH!");return;};
        set_name_(ui->lineEdit_name->text());
        set_path_(ui->lineEdit_path->text());
        QDir dir(path_);
        if(dir.exists(name_)){SHOW_LOG("EXIST PRJ");return;}
        accept();
    });
}

CreatePrjDialog::~CreatePrjDialog()
{
    delete ui;
}
