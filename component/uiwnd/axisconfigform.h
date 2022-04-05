#ifndef AXISCONFIGFORM_H
#define AXISCONFIGFORM_H

#include <QWidget>
#include <DockWidget.h>
#include <basewidget.h>
#include <marcroly.h>
#include <program.h>

#define OP_AXIS_ALL_OBJ(OP_MARCRO)\
    OP_MARCRO(QString, fileName_)\
    OP_MARCRO(QString, filePath_)\
    OP_MARCRO(QString, fileFolderPath_)\
    OP_MARCRO(bool, isModified_)\
    OP_MARCRO(QVector<Axis>, AxisVec_)\
    OP_MARCRO(QString, axisesSum_)
namespace Ui {
class AxisConfigForm;
}
class AxisConfigForm : public BaseWidget
{
    Q_OBJECT
public:
    void updateUi();
    explicit AxisConfigForm(QString filePath, QWidget *parent = nullptr);
    ~AxisConfigForm();
    void initDock() override;
signals:
    OP_AXIS_ALL_OBJ(DECL_SIGNAL_METADATA);
    OP_AXIS_ALL_OBJ(REGISTER_METADATA);
private:
    Ui::AxisConfigForm *ui;
};

#endif // AXISCONFIGFORM_H
