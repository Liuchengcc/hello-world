#ifndef LOGFILEFORM_H
#define LOGFILEFORM_H

#include <QWidget>
#include <basewidget.h>
#include <marcroly.h>
#include <QMap>

#define OP_LOGFILE_ALL_OBJ(OP_MARCRO)\
    OP_MARCRO(QString, filePath_)\
    OP_MARCRO(QByteArray, logData_)\
    OP_MARCRO(QStringList, logDataList_)\
    OP_MARCRO(ErrorMap, ErrorMap_)

namespace Ui {
class logFileForm;
}

class logFileForm : public BaseWidget
{
    Q_OBJECT

public:
    static logFileForm* getInstance();
    explicit logFileForm(QWidget *parent = nullptr);
    void initErrorMap();
    void uploadLogFile();
    void parserLogFile();
    void initDock() override;
    ~logFileForm();
    typedef QMap<QString, QString> ErrorMap ;

signals:
    OP_LOGFILE_ALL_OBJ(DECL_SIGNAL_METADATA);
    OP_LOGFILE_ALL_OBJ(REGISTER_METADATA);
private:
    static logFileForm* instance_;
    Ui::logFileForm *ui;
};

#endif // LOGFILEFORM_H
