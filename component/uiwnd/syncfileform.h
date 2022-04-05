#ifndef SYNCFILEFORM_H
#define SYNCFILEFORM_H

#include <QWidget>
#include <basewidget.h>


class ProgramsCtrl;
namespace Ui {
class SyncFileForm;
}

class SyncFileForm : public BaseWidget
{
    Q_OBJECT

public:
    static SyncFileForm* getInstance();
    explicit SyncFileForm(QWidget *parent = nullptr);
    ~SyncFileForm();
    void initDock() override;
    void updateUi();

private:
    void setFilelistFilter();
    ProgramsCtrl* pgctrl_;
    QStringList currentPrjFileList_;
    QStringList controllerFileList_;
    static SyncFileForm* instance_;
    Ui::SyncFileForm *ui;
};

#endif // SYNCFILEFORM_H
