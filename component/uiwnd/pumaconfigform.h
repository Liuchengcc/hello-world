#ifndef PUMACONFIGFORM_H
#define PUMACONFIGFORM_H

#include <QWidget>
#include <basewidget.h>

namespace Ui {
class PumaConfigForm;
}

class PumaConfigForm : public BaseWidget
{
    Q_OBJECT

public:
    explicit PumaConfigForm(QString filePath, QWidget *parent = nullptr);
    ~PumaConfigForm();

private:
    Ui::PumaConfigForm *ui;
};

#endif // PUMACONFIGFORM_H
