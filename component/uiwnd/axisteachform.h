#ifndef AXISTEACHFORM_H
#define AXISTEACHFORM_H

#include <QWidget>
#include <basewidget.h>

namespace Ui {
class AxisTeachForm;
}

class AxisTeachForm : public BaseWidget
{
    Q_OBJECT

public:
    static AxisTeachForm* getInstance();
    explicit AxisTeachForm(QWidget *parent = nullptr);
    ~AxisTeachForm();
    void initDock() override;
    void initUi();
protected:
    void timerEvent(QTimerEvent *event) override;
private:
    static AxisTeachForm* instance_;
    Ui::AxisTeachForm *ui;

};

#endif // AXISTEACHFORM_H
