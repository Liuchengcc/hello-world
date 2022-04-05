#ifndef CONTROLWIDGET_H
#define CONTROLWIDGET_H

#include <QWidget>
#include "basewidget.h"


#define AXIS_TOKEN1 J1
#define AXIS_TOKEN2 J2
#define AXIS_TOKEN3 J3
#define AXIS_TOKEN4 J4
#define AXIS_TOKEN5 J5
#define AXIS_TOKEN6 J6

#define AXIS_TOKEN1_VALUE x
#define AXIS_TOKEN2_VALUE y
#define AXIS_TOKEN3_VALUE z
#define AXIS_TOKEN4_VALUE u
#define AXIS_TOKEN5_VALUE v
#define AXIS_TOKEN6_VALUE w

#define OP_ALL_AXIS(FUNC)\
    FUNC(AXIS_TOKEN1);\
    FUNC(AXIS_TOKEN2);\
    FUNC(AXIS_TOKEN3);\
    FUNC(AXIS_TOKEN4);\
    FUNC(AXIS_TOKEN5);\
    FUNC(AXIS_TOKEN6);

class Controller;
namespace Ui {
class ControlWidget;
}

class ControlWidget : public BaseWidget
{
    Q_OBJECT

public:
    static ControlWidget* getInstance();
    explicit ControlWidget(QWidget *parent = nullptr);
    ~ControlWidget();
    void initDock() override;
    void initUi();
    void initNetRecvAndDataChange();

private slots:
    void on_comboBox_Users_currentIndexChanged(const QString &arg1);

    void on_comboBox_Tools_currentIndexChanged(const QString &arg1);

    void on_comboBox_Coordinate_currentIndexChanged(const QString &arg1);

private:
    static ControlWidget* instance_;

    Ui::ControlWidget *ui;
    Controller* ctrller_;
};

#endif // CONTROLWIDGET_H
