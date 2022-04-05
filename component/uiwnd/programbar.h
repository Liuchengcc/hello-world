#ifndef PROGRAMBAR_H
#define PROGRAMBAR_H

#include <QWidget>
#include <DockWidget.h>
#include <QMutex>
#include <program.h>
#include <qtreewidget.h>
#include <programfile.h>
#include <basewidget.h>

namespace Ui {
class ProgramBar;
}

class ProgramBar : public BaseWidget
{
    Q_OBJECT

public:
    friend class Program;
    friend class ProgramFile;

    explicit ProgramBar(QWidget *parent = nullptr);
    enum itemType
    {
        Root = ProgramFile::FileTypeSum,
        Folder,
        itemTypeSum
    };

    static ProgramBar* getInstance()
    {
        static QMutex mutex;
        if(!instance_)
        {
            QMutexLocker locker(&mutex);
            instance_ = new ProgramBar;
        }
        return instance_;
    }
    ~ProgramBar();
    void updateUi();
    int newProgram(QString folderPath, QString pgName);
    int loadProgram(Program *);
    int loadProgram(QString);
    void initDock() override;
    QString getItemRootName(QTreeWidgetItem*);
    QString getItemFileName(QTreeWidgetItem*);
signals:
    void openluaFile(QString);
    void openInstance(QString);
    void openConfig(QString);
    void openTools(QString);
    void openPoints(QString);
    void openUsers(QString);
    void openOthers(QString);
private:
    Ui::ProgramBar *ui;
    ProgramsCtrl* pgCtrl_;
    QMap<QString, QString> nameMap_;
    static ProgramBar* instance_;

};

#endif // PROGRAMBAR_H
