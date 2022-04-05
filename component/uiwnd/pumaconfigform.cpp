#include "pumaconfigform.h"
#include "ui_pumaconfigform.h"

PumaConfigForm::PumaConfigForm(QString filePath, QWidget *parent) :
    BaseWidget(parent),
    ui(new Ui::PumaConfigForm)
{
    Q_UNUSED(filePath);
    ui->setupUi(this);
}

PumaConfigForm::~PumaConfigForm()
{
    delete ui;
}
