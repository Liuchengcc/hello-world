#ifndef WINDOWSMANAGER_H
#define WINDOWSMANAGER_H

#include <QObject>
#include <DockWidget.h>
#include <DockManager.h>
#include <programbar.h>
#include <axisconfigform.h>
#include <basewidget.h>
#include <DockAreaWidget.h>

class CodeEditor;
class WindowsManager : public QObject
{
    Q_OBJECT
public:
    enum WndType
    {
        WndProgramBar,
        WndAxisCfg,
        WndConfig,
        WndAxisTeach,
        WndTeach,
        WndIOState,
        WndControllerCfg,
        WndRobotCfg,
        WndInstanceCfg,
        WndPoint,
        WndTool,
        WndUser,
        WndNormalFile,
        WndLog,
        WndSyncFile,
        WndRunningPrintLog,
        WndStateLog,
        WndTaskMonitor
    };
    static WindowsManager *getInstance(QObject* parent = nullptr);
    explicit WindowsManager(QObject *parent = nullptr);
    CodeEditor* getCurrentCodeEditor();
    void saveCurrentWnd();
    void saveAllCurrentWnd();
    void downloadAllCurrentWnd();
    void cutCurrentFile();
    void copyCurrentFile();
    void parserCurrentFile();
    void undoCurrentFile();
    void redoCurrentFile();

    void initDockMng();
    void initInstanceWnd();
    int newProgram(QString path, QString name);
    int loadProgram(QString);
    int showInstanceWnd(WndType);
    int showCommonWnd(QString);
    int hideInstanceWnd(WndType);
    BaseWidget* WidFactory(WndType type, QString pgfilePath = "");

signals:
public slots:
    void showWnd(WndType, QString pgName = "", bool status = true);
private:
    static WindowsManager* instance_;
    QWidget* parent_;
    ads::CDockManager* dockMng_;
    ProgramBar* pgBar_;
    ProgramsCtrl* pgCtrl_;
    AxisConfigForm* AxisCfg_;



    QMap<QString, BaseWidget*> commonWidgetMap_;
    QMap<WndType, BaseWidget*> singleWndMap_;
    QMap<ads::DockWidgetArea, ads::CDockAreaWidget*> dockAreaMap_;

};

#endif // WINDOWSMANAGER_H
