#ifndef POINTSFORM_H
#define POINTSFORM_H

#include <QWidget>
#include <basewidget.h>
#include <controller.h>

#define OP_POINTS_ALL_OBJ(OP_MARCRO)\
    OP_MARCRO(QString, filePath_)\
    OP_MARCRO(QList<PointValue>, pointsList_)

namespace Ui {
class PointsForm;
}

class PointsForm : public BaseWidget
{
    Q_OBJECT

public:

    explicit PointsForm(QString filePath, QWidget *parent = nullptr);
    ~PointsForm();
    void updateUi();
    void initDock() override;
signals:
OP_POINTS_ALL_OBJ(DECL_SIGNAL_METADATA);
OP_POINTS_ALL_OBJ(REGISTER_METADATA);
private:
    Ui::PointsForm *ui;
};

#endif // POINTSFORM_H
