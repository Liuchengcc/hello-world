#ifndef SCARACONFIGFORM_H
#define SCARACONFIGFORM_H

#include <QWidget>
#include <DockWidget.h>
#include <basewidget.h>
#include <instanceinterface.h>
#include <marcroly.h>
#include <program.h>
#include <qmap.h>

#define OP_SCARA_ALL_OBJ(OP_MARCRO)\
    OP_MARCRO(QString, filePath_)\
    OP_MARCRO(QString, pgName_)\
    OP_MARCRO(Scara, scara_)\
    OP_MARCRO(QByteArray, md5Data_)\
    OP_MARCRO(QVector<Axis>, AxisVec_)

namespace Ui {
class ScaraConfigForm;
}

class ScaraConfigForm : public BaseWidget
{
    Q_OBJECT

public:
    ScaraConfigForm(QString filePath, QWidget *parent = nullptr);
    ~ScaraConfigForm();
    void initDock() override;
    void initUi();
    void updateUi();
    void readAndParserFile();
    void saveDataToFile();
    QByteArray calAllUiData();
    static void getDefaultDocData(QByteArray&);
signals:
    OP_SCARA_ALL_OBJ(DECL_SIGNAL_METADATA);
    OP_SCARA_ALL_OBJ(REGISTER_METADATA);
private:
    QMap<QString, QWidget*> uiMap_;

    Ui::ScaraConfigForm *ui;
};

#endif // SCARACONFIGFORM_H
