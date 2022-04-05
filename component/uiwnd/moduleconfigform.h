#ifndef MODULECONFIGFORM_H
#define MODULECONFIGFORM_H

#include <QWidget>
#include <basewidget.h>

namespace Ui {
class ModuleConfigForm;
}

class ModuleConfigForm : public BaseWidget
{
    Q_OBJECT

public:
    explicit ModuleConfigForm(QString path, QWidget *parent = nullptr);
    ~ModuleConfigForm();

private:
    Ui::ModuleConfigForm *ui;
};

#endif // MODULECONFIGFORM_H
