#include "windowsmanager.h"
#include "DockManager.h"
#include "DockAreaWidget.h"
#include "axisconfigform.h"
#include "axisteachform.h"
#include "programbar.h"
#include "qlabel.h"
#include "toolswidget.h"
#include "userswidget.h"
#include "controlwidget.h"
#include "globalfunction.h"
#include "codeeditor.h"
#include "instanceinterface.h"
#include "pointsform.h"
#include "syncfileform.h"
#include "logoutform.h"
#include "logfileform.h"
#include "iostateform.h"
#include "taskmonitor.h"


#define ADD_DOCK_TO_DOCKMANAGER(baseWind)\
          dockAreaMap_[baseWind->getArea()] = dockMng_->addDockWidget(baseWind->getArea(), baseWind->getDock(), dockAreaMap_[baseWind->getArea()]);

using namespace ads;
WindowsManager* WindowsManager::instance_ = nullptr;
WindowsManager* WindowsManager::getInstance(QObject* parent)
{
    if(!instance_)
    {
        instance_ = new WindowsManager(parent);
    }
    return instance_;
}

WindowsManager::WindowsManager(QObject *parent) : QObject(parent)
{
    commonWidgetMap_.clear();
    singleWndMap_.clear();
    parent_ = qobject_cast<QWidget*>(parent);
    pgCtrl_ = ProgramsCtrl::getInstance();
    initDockMng();
    initInstanceWnd();
    showInstanceWnd(WndProgramBar);
    showInstanceWnd(WndTeach);
    showInstanceWnd(WndStateLog);
}

CodeEditor *WindowsManager::getCurrentCodeEditor()
{
    if(!dockMng_->focusWidget())
    {
        return nullptr;
    }
//如果editor，则保存
    qDebug()<<dockMng_->focusWidget()->metaObject()->className();
    if(dockMng_->focusWidget()->metaObject()->className() == QStringLiteral("QsciScintilla"))
    {
        return  qobject_cast<CodeEditor*>(dockMng_->focusWidget()->parent());
    }
    return nullptr;
}

void WindowsManager::saveCurrentWnd(){if(getCurrentCodeEditor())getCurrentCodeEditor()->saveFile();}

void WindowsManager::cutCurrentFile(){if(getCurrentCodeEditor())getCurrentCodeEditor()->cut();}

void WindowsManager::copyCurrentFile(){if(getCurrentCodeEditor())getCurrentCodeEditor()->copy();}

void WindowsManager::parserCurrentFile(){if(getCurrentCodeEditor())getCurrentCodeEditor()->paste();}

void WindowsManager::undoCurrentFile(){if(getCurrentCodeEditor())getCurrentCodeEditor()->undo();}

void WindowsManager::redoCurrentFile(){if(getCurrentCodeEditor())getCurrentCodeEditor()->redo();}

void WindowsManager::saveAllCurrentWnd()
{
    foreach (auto wind, commonWidgetMap_.values())
    {
        if(wind->objectName() == "CodeEditor")
        {
            CodeEditor* tempEditor = qobject_cast<CodeEditor*>(wind);
            tempEditor->saveFile();
        }
    }
}

void WindowsManager::downloadAllCurrentWnd()
{
    saveAllCurrentWnd();
    if(!pgCtrl_->getActiveProgram())
    {
        GlobalFunction::showLogMsg(tr("请先加载活动工程！"));
    }
    QString pgname = pgCtrl_->getActiveProgram()->get_programName_();
    pgCtrl_->downLoadToControllerFile(pgname);
}

void WindowsManager::initDockMng()
{
    dockMng_ = new ads::CDockManager(qobject_cast<QWidget*>(parent()));
    QLabel *label = new QLabel("this is background");
    label->setStyleSheet("background-color:gray;");
    CDockWidget* centralWidget = new CDockWidget("centralWidget");
    centralWidget->setWidget(label);
    centralWidget->setFeature(ads::CDockWidget::NoTab, true);
    dockAreaMap_[CenterDockWidgetArea] = dockMng_->addDockWidget(CenterDockWidgetArea, centralWidget);
    connect(dockMng_, &ads::CDockManager::dockAreasRemoved,[this]()
    {
        QList<ads::CDockAreaWidget *> areaList;
        int areaSum = dockMng_->dockAreaCount();
        for(int i = 0; i < areaSum; i++)
        {
            areaList.append(dockMng_->dockArea(i));
        }
        foreach (auto key, dockAreaMap_.keys())
        {
            if(!areaList.contains(dockAreaMap_[key]))
            {
                qDebug()<<"erase area"<<dockAreaMap_[key];
                dockAreaMap_.remove(key);
                return;
            }
        }
    });
}

void WindowsManager::initInstanceWnd()
{
    auto addBaseInstanceWnd = [this](WindowsManager::WndType type, BaseWidget * widget)
    {
        singleWndMap_.insert(type, widget);
    };
    addBaseInstanceWnd(WndProgramBar, dynamic_cast<BaseWidget*>(ProgramBar::getInstance()));
    addBaseInstanceWnd(WndStateLog, dynamic_cast<BaseWidget*>(LogoutForm::getInstance()));
    addBaseInstanceWnd(WndTeach,  dynamic_cast<BaseWidget*>(ControlWidget::getInstance()));
    addBaseInstanceWnd(WndAxisTeach,  dynamic_cast<BaseWidget*>(AxisTeachForm::getInstance()));
    addBaseInstanceWnd(WndSyncFile,  dynamic_cast<BaseWidget*>(SyncFileForm::getInstance()));
    addBaseInstanceWnd(WndLog, dynamic_cast<BaseWidget*>(logFileForm::getInstance()));
    addBaseInstanceWnd(WndIOState, dynamic_cast<BaseWidget*>(IOStateForm::getInstance()));
    addBaseInstanceWnd(WndTaskMonitor, dynamic_cast<BaseWidget*>(TaskMonitor::getInstance()));


    connect(ProgramBar::getInstance(), &ProgramBar::openTools, [this](QString pgName){showWnd(WndTool, pgName);});
    connect(ProgramBar::getInstance(), &ProgramBar::openUsers, [this](QString pgName){showWnd(WndUser, pgName);});
    connect(ProgramBar::getInstance(), &ProgramBar::openPoints, [this](QString pgName){showWnd(WndPoint, pgName);});
    connect(ProgramBar::getInstance(), &ProgramBar::openluaFile, [this](QString pgName){showWnd(WndNormalFile, pgName);});
    connect(ProgramBar::getInstance(), &ProgramBar::openInstance, [this](QString pgName){showWnd(WndInstanceCfg, pgName);});
    connect(ProgramBar::getInstance(), &ProgramBar::openConfig, [this](QString pgName){showWnd(WndConfig, pgName);});
    connect(ProgramBar::getInstance(), &ProgramBar::openOthers, [this](QString pgName){showWnd(WndNormalFile, pgName);});
}

int WindowsManager::newProgram(QString prjPath, QString prjName)
{
    //todo
    ProgramBar::getInstance()->newProgram(prjPath, prjName);
    Q_UNUSED(prjName)
    Q_UNUSED(prjPath)
    return 0;
}

int WindowsManager::loadProgram(QString filePath)
{
    QString prjName = filePath.split("/").last();
    ProgramBar::getInstance()->loadProgram(filePath);
    return 0;
}

int WindowsManager::showInstanceWnd(WindowsManager::WndType type)
{
    assert(singleWndMap_.contains(type));
    bool isVisable = singleWndMap_[type]->getDock()->isVisible();
    if(isVisable)
    {
        return 0;
    }
    ADD_DOCK_TO_DOCKMANAGER(singleWndMap_[type]);
    return 0;
}

int WindowsManager::showCommonWnd(QString filePath)
{
    assert(commonWidgetMap_.contains(filePath));
    ADD_DOCK_TO_DOCKMANAGER(commonWidgetMap_[filePath]);
    return 0;
}

int WindowsManager::hideInstanceWnd(WindowsManager::WndType type)
{
    assert(singleWndMap_.contains(type));
    dockMng_->removeDockWidget(singleWndMap_[type]->dock_);
    return 0;
}


BaseWidget *WindowsManager::WidFactory(WindowsManager::WndType originType, QString filePath)
{
    if(filePath.isEmpty())
    {
        return nullptr;
    }
#define GET_WIND_BY_CHECK_TYPE(ORIGINTYPE, WINDTYPE, WINDCLASS)	 \
    if(ORIGINTYPE == WINDTYPE)\
    {\
    return new WINDCLASS(filePath); \
    }
    GET_WIND_BY_CHECK_TYPE(originType, WndTool, ToolsWidget);
    GET_WIND_BY_CHECK_TYPE(originType, WndUser, UsersWidget);
    GET_WIND_BY_CHECK_TYPE(originType, WndNormalFile, CodeEditor);
    GET_WIND_BY_CHECK_TYPE(originType, WndPoint, PointsForm);

    return InstanceInterface::getInstance(filePath);
#undef GET_WIND_BY_CHECK_TYPE
}

void WindowsManager::showWnd(WindowsManager::WndType type,QString pgfilePath, bool status)
{
    Q_UNUSED(status);
    if(singleWndMap_.contains(type))
    {
        showInstanceWnd(type);
    }
    else if(commonWidgetMap_.contains(pgfilePath))
    {
        showCommonWnd(pgfilePath);
    }
    else
    {
        BaseWidget* baseWind = WidFactory(type, pgfilePath);
        if(!baseWind)
        {
            qDebug()<<"wind is not exist!";
            GlobalFunction::showLogMsg(tr("暂不支持此功能！"));

        }
        else
        {
            connect(baseWind->getDock(), &ads::CDockWidget::closed, this, [this, pgfilePath]()
            {
                commonWidgetMap_.remove(pgfilePath);
            });
            commonWidgetMap_[pgfilePath] = baseWind;
            ADD_DOCK_TO_DOCKMANAGER(baseWind);
        }
    }
}
