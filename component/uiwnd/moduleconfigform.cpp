#include "moduleconfigform.h"
#include "ui_moduleconfigform.h"

ModuleConfigForm::ModuleConfigForm(QString path, QWidget *parent) :
    BaseWidget(parent),
    ui(new Ui::ModuleConfigForm)
{
    Q_UNUSED(path);
    ui->setupUi(this);
}

ModuleConfigForm::~ModuleConfigForm()
{
    delete ui;
}
