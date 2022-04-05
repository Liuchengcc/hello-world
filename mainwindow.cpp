#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qsplitter.h"
#include "DockManager.h"
#include "qlabel.h"
#include "qsciscintilla.h"
#include "windowsmanager.h"
#include "qfiledialog.h"
#include "createprjdialog.h"
#include <QPainter>
#include <config.h>
#include <QComboBox>
#include <controller.h>
#include <iconhelper.h>
#include <ftpmanager.h>
#include <globalfunction.h>
#include <QDir>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("LHControllerStudio");
//    setStyle(Style_FlatWhite);
    wm_ = new WindowsManager(this);
    config_ = Config::getInstance();
    controller_ = Controller::getInstance();
    initMenu();
    initToolBar();
    initFolder();
    initStatusBar();
    connect(controller_, &Controller::updateMainWindState, this, [this]
    {
        auto checkNetState = [this](Controller::NetWorkState rightState, QString text, QString color)
        {
            if(controller_->get_networkState_() != rightState)
            {
                return;
            }
            toolbar_netstate_->setText(text);
            toolbar_netstate_->setStyleSheet(tr("color:%1").arg(color));
        };
        checkNetState(Controller::net_connected, "connected", "green");
        checkNetState(Controller::net_connectingError, "connecting...", "orange");
        checkNetState(Controller::net_unConnected, "unconnected", "red");
    });
    if(!config_->get_defaultPrjPath_().isEmpty())
    {
        wm_->loadProgram(config_->get_defaultPrjPath_());
    }
    controller_->setIPAndPort(config_->get_ip_(), config_->get_port_());
    FtpManager::getInstance()->setFtpHostIPAndPort(config_->get_ftpIp_(), config_->get_ftpPort_());
    GlobalFunction::showLogMsg(tr("初始化完成！"));
}

void MainWindow::setStyle(MainWindow::Style style)
{
    QString qssFile = ":/qss/blue.css";

    if (style == Style_Silvery) {
        qssFile = ":/qss/silvery.css";
    } else if (style == Style_Blue) {
        qssFile = ":/qss/blue.css";
    } else if (style == Style_LightBlue) {
        qssFile = ":/qss/lightblue.css";
    } else if (style == Style_DarkBlue) {
        qssFile = ":/qss/darkblue.css";
    } else if (style == Style_Gray) {
        qssFile = ":/qss/gray.css";
    } else if (style == Style_LightGray) {
        qssFile = ":/qss/lightgray.css";
    } else if (style == Style_DarkGray) {
        qssFile = ":/qss/darkgray.css";
    } else if (style == Style_Black) {
        qssFile = ":/qss/black.css";
    } else if (style == Style_LightBlack) {
        qssFile = ":/qss/lightblack.css";
    } else if (style == Style_DarkBlack) {
        qssFile = ":/qss/darkblack.css";
    } else if (style == Style_PSBlack) {
        qssFile = ":/qss/psblack.css";
    } else if (style == Style_FlatBlack) {
        qssFile = ":/qss/flatblack.css";
    } else if (style == Style_FlatWhite) {
        qssFile = ":/qss/flatwhite.css";
    }

    QFile file(qssFile);

    if (file.open(QFile::ReadOnly)) {
        QString qss = QLatin1String(file.readAll());
        QString paletteColor = qss.mid(20, 7);
        qApp->setPalette(QPalette(QColor(paletteColor)));
        qApp->setStyleSheet(qss);
        file.close();
    }
}
#define BOUND_ACTION_SHOW_WND(x, y) connect(x, &QAction::triggered, this, [this](bool status){wm_->showWnd(y,"", status);});
void MainWindow::initMenu()
{
//    connect(ui->action_viewPg, &QAction::triggered, this, [this](bool status){wm_->showWnd(WindowsManager::WndProgramBar, status);});
    BOUND_ACTION_SHOW_WND(ui->action_axis, WindowsManager::WndAxisCfg);
    BOUND_ACTION_SHOW_WND(ui->action_viewPg, WindowsManager::WndProgramBar);
    connect(ui->action_newPg, &QAction::triggered, [this]
    {
         CreatePrjDialog dia;
         if(QDialog::Accepted == dia.exec())
         {
            QString path = dia.get_path_();
            QString name = dia.get_name_();
            wm_->newProgram(path, name);
         }
    });
    connect(ui->action_loadPg, &QAction::triggered, this, [this]
    {
       QString fileName = QFileDialog::getOpenFileName(this, tr("打开项目"), tr("C:/"), "*.lhprj");
       if(!fileName.isEmpty())
       {
           wm_->loadProgram(fileName);
       }
       qDebug()<<fileName;
    });
}

void MainWindow::initToolBar()
{
    auto setToolBarStyleSheet = [](QToolBar* tbar){tbar->setStyleSheet(".QComboBox,.QPushButton,.QToolButton{padding:5px;margin:2px;border-radius:2px;}");};
    setToolBarStyleSheet(ui->toolBar_run);
    setToolBarStyleSheet(ui->toolBar_robot);
    setToolBarStyleSheet(ui->toolBar_stand);
    setToolBarStyleSheet(ui->toolBar_edit);
    {
        QWidgetAction *tempAction = new QWidgetAction(this);
        QComboBox *tempCombo = new QComboBox(this);
        tempCombo->addItems(tr("offline online").split(" "));
        tempAction->setDefaultWidget(tempCombo);
        connect(tempCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index)
        {
            controller_->setTcpLink((bool)index);
        });
        ui->toolBar_robot->addAction(tempAction);
    }
    {
        QWidgetAction* tempAction = new QWidgetAction(this);
        toolbar_netstate_ = new QLabel("offLine", this);
        toolbar_netstate_->setMinimumSize(80, 24);
        tempAction->setDefaultWidget(toolbar_netstate_);
        ui->toolBar_robot->addAction(tempAction);
    }

#define INIT_TOOLBAR_ICON_BTN_FUNC(TOOLBAR, ICON_CHAR, ICON_SIZE, FUNC,HOVER_COLOR){\
     QWidgetAction *tempAction = new QWidgetAction(this);\
    QPushButton* tempLabel = new QPushButton;\
    tempLabel->setMinimumSize(36,36);\
    tempLabel->setMaximumSize(36,36);\
    tempAction->setDefaultWidget(tempLabel);\
    TOOLBAR->addAction(tempAction);\
    connect(tempLabel, &QPushButton::clicked, this, [this]FUNC);\
    IconHelper::Instance()->setIcon(tempLabel, QChar(ICON_CHAR), ICON_SIZE);\
    tempLabel->setStyleSheet(tr("QPushButton:hover{color: %1;}").arg(HOVER_COLOR));}

    //保存 全部保存 下载 剪贴 复制 粘贴  撤销 重做
    ui->toolBar_edit->setWindowTitle(tr("编辑"));
    INIT_TOOLBAR_ICON_BTN_FUNC(ui->toolBar_edit, 0xf0a0,12,{wm_->saveAllCurrentWnd();}, "darkgreen");
    INIT_TOOLBAR_ICON_BTN_FUNC(ui->toolBar_edit, 0xf019,12,{wm_->downloadAllCurrentWnd();}, "darkgreen");
    INIT_TOOLBAR_ICON_BTN_FUNC(ui->toolBar_edit, 0xf0c4,12,{wm_->cutCurrentFile();}, "darkgreen");
    INIT_TOOLBAR_ICON_BTN_FUNC(ui->toolBar_edit, 0xf0c5,12,{wm_->copyCurrentFile();}, "darkgreen");
    INIT_TOOLBAR_ICON_BTN_FUNC(ui->toolBar_edit, 0xf0ea,12,{wm_->parserCurrentFile();}, "darkgreen");
    INIT_TOOLBAR_ICON_BTN_FUNC(ui->toolBar_edit, 0xf0e2,12,{wm_->undoCurrentFile();}, "darkgreen");
    INIT_TOOLBAR_ICON_BTN_FUNC(ui->toolBar_edit, 0xf01e,12,{wm_->redoCurrentFile();}, "darkgreen");
        //上电 下电 清错
    ui->toolBar_robot->setWindowTitle(tr("操作"));
    INIT_TOOLBAR_ICON_BTN_FUNC(ui->toolBar_robot, 0xf0e7,12,{controller_->buildPacketSend(ROBOT_ID, FUNC_TEACH, TEACH_SERVO_ON, 0, TYPE, "63,");}, "darkgreen");
    INIT_TOOLBAR_ICON_BTN_FUNC(ui->toolBar_robot, 0xf127,12,{controller_->buildPacketSend(ROBOT_ID, FUNC_TEACH, TEACH_SERVO_ON, 0, TYPE, "0,");}, "darkred");
    INIT_TOOLBAR_ICON_BTN_FUNC(ui->toolBar_robot, 0xf0a2,12,{controller_->buildPacketSend(ROBOT_ID, FUNC_SYS, SYS_CLEAR_FAULT, 0, TYPE, "ErrType,");}, "red");
    //启动 debug 停止 暂停
    ui->toolBar_run->setWindowTitle(tr("操与运行"));
    INIT_TOOLBAR_ICON_BTN_FUNC(ui->toolBar_run, 0xf04b,12,{controller_->programStart();}, "darkgreen");
    INIT_TOOLBAR_ICON_BTN_FUNC(ui->toolBar_run, 0xf188,12,{controller_->programDebugStart();}, "darkgreen");
    INIT_TOOLBAR_ICON_BTN_FUNC(ui->toolBar_run, 0xf04d,12,{controller_->programStop();}, "darkred");
    INIT_TOOLBAR_ICON_BTN_FUNC(ui->toolBar_run, 0xf04c,12,{controller_->programPause();}, "orange");

#define INIT_TOOLBAR_ICON(TOOLBAR, ICON_CHAR, ICON_SIZE, WND_TYPE, HOVER_COLOR) {\
    QWidgetAction *tempAction = new QWidgetAction(this);\
    QPushButton* tempLabel = new QPushButton;\
    tempLabel->setMinimumSize(36,36);\
    tempLabel->setMaximumSize(36,36);\
    tempLabel->setGeometry(0, 0, ICON_SIZE+3, ICON_SIZE+3);\
    tempAction->setDefaultWidget(tempLabel);\
    TOOLBAR->addAction(tempAction);\
    connect(tempLabel, &QPushButton::clicked, this, [this](bool status){wm_->showWnd(WND_TYPE,"", status);});\
    IconHelper::Instance()->setIcon(tempLabel, QChar(ICON_CHAR), ICON_SIZE);\
    tempLabel->setStyleSheet(tr("QPushButton:hover{color: %1;}").arg(HOVER_COLOR));}

    //导航bar 示教 轴示教 设置 传输文件 日志 运行打印 状态输出 IO状态
    ui->toolBar_stand->setWindowTitle(tr("机械手工具栏"));
    INIT_TOOLBAR_ICON(ui->toolBar_stand, 0xf277, 12, WindowsManager::WndProgramBar, "blue");
    INIT_TOOLBAR_ICON(ui->toolBar_stand, 0xf047, 12, WindowsManager::WndTeach, "blue");
    INIT_TOOLBAR_ICON(ui->toolBar_stand, 0xf07e, 12, WindowsManager::WndAxisTeach, "blue");
    INIT_TOOLBAR_ICON(ui->toolBar_stand, 0xf013, 12, WindowsManager::WndConfig, "black");
    INIT_TOOLBAR_ICON(ui->toolBar_stand, 0xf0ec, 12, WindowsManager::WndSyncFile, "blue");
    INIT_TOOLBAR_ICON(ui->toolBar_stand, 0xf03a, 12, WindowsManager::WndLog, "blue");
    INIT_TOOLBAR_ICON(ui->toolBar_stand, 0xf18d, 12, WindowsManager::WndRunningPrintLog, "blue");
    INIT_TOOLBAR_ICON(ui->toolBar_stand, 0xf05a, 12, WindowsManager::WndStateLog, "blue");
    INIT_TOOLBAR_ICON(ui->toolBar_stand, 0xf2a1, 12, WindowsManager::WndIOState, "blue");
    INIT_TOOLBAR_ICON(ui->toolBar_stand, 0xf2ad, 12, WindowsManager::WndTaskMonitor, "blue");
}

void MainWindow::initStatusBar()
{
#define LABEL_SYS label_sys
#define LABEL_SERVO label_servo
#define LABEL_MODE label_mode

#define INIT_STATUS_LABEL(DES_LABEL)\
    QLabel* DES_LABEL = new QLabel(this);\
        DES_LABEL->setMinimumSize(60, 24);\
        DES_LABEL->setFrameShape(QFrame::WinPanel);\
    DES_LABEL->setFrameShadow(QFrame::Sunken);\
        DES_LABEL->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);\
        ui->statusBar->addPermanentWidget(DES_LABEL);
    INIT_STATUS_LABEL(LABEL_MODE)
    INIT_STATUS_LABEL(LABEL_SERVO)
    INIT_STATUS_LABEL(LABEL_SYS)

    connect(controller_, &Controller::change_modeState_, [this, LABEL_MODE]
    {
# define UPDATE_COLLTROLLER_STATE(STATE, MATCH_STATE, DES_LABEL, SHOW_STRING, STRING_COLOR)\
    if(STATE == MATCH_STATE)\
        {\
    DES_LABEL->setText(SHOW_STRING); \
    DES_LABEL->setStyleSheet(tr("color:%1").arg(STRING_COLOR));\
    }
        UPDATE_COLLTROLLER_STATE(Controller::mode_teach, controller_->get_modeState_(), LABEL_MODE, "示教", "green");
        UPDATE_COLLTROLLER_STATE(Controller::mode_auto, controller_->get_modeState_(), LABEL_MODE, "自动", "green");
        UPDATE_COLLTROLLER_STATE(Controller::mode_remote, controller_->get_modeState_(),  LABEL_MODE, "远程", "green");
        UPDATE_COLLTROLLER_STATE(Controller::mode_unkown, controller_->get_modeState_(),  LABEL_MODE, "unkown", "gray");
    });
    connect(controller_, &Controller::change_sysState_, [this, LABEL_SYS]
    {
        auto getError = [this]()
        {
            QString error = "未知错误";
            if(controller_->get_servoError_() != "0"){error = controller_->get_servoError_();}
            else if(controller_->get_ethercatError_() != "0"){error = controller_->get_ethercatError_();}
            else if(controller_->get_motionError_() != "0"){error = controller_->get_motionError_();}
            else if(controller_->get_interpreterError_() != "0"){error = controller_->get_interpreterError_();}
            QString rightError = QString("0x%1").arg(error.toInt(), 4, 16, QLatin1Char('0'));
            return rightError;
        };

        UPDATE_COLLTROLLER_STATE(Controller::sys_STANDSTILL, controller_->get_sysState_(), LABEL_SYS, "就绪", "green");
        UPDATE_COLLTROLLER_STATE(Controller::sys_STOP, controller_->get_sysState_(), LABEL_SYS, "停止", "red");
        UPDATE_COLLTROLLER_STATE(Controller::sys_ERROR, controller_->get_sysState_(), LABEL_SYS, getError(), "red");
        UPDATE_COLLTROLLER_STATE(Controller::sys_NORMAL_RUN, controller_->get_sysState_(), LABEL_SYS, "自动运行", "green");
        UPDATE_COLLTROLLER_STATE(Controller::sys_DEBUG_RUN, controller_->get_sysState_(), LABEL_SYS, "调试运行", "green");
        UPDATE_COLLTROLLER_STATE(Controller::sys_DEBUG_PAUSE, controller_->get_sysState_(), LABEL_SYS, "调试暂停", "green");
        UPDATE_COLLTROLLER_STATE(Controller::sys_NORMAL_PAUSE, controller_->get_sysState_(), LABEL_SYS, "自动暂停", "green");
        UPDATE_COLLTROLLER_STATE(Controller::sys_unkown, controller_->get_sysState_(), LABEL_SYS, "unkown", "gray");

    });
    connect(controller_, &Controller::change_servoState_, [this, LABEL_SERVO]
    {

        UPDATE_COLLTROLLER_STATE(Controller::servo_on, controller_->get_servoState_(), LABEL_SERVO, "上电", "green");
        UPDATE_COLLTROLLER_STATE(Controller::servo_off, controller_->get_servoState_(), LABEL_SERVO, "下电", "red");
        UPDATE_COLLTROLLER_STATE(Controller::servo_unkown, controller_->get_servoState_(), LABEL_SERVO, "unkown", "gray");
    });
}

#define BASE_DIR "/res"
#define ERR_DIR "/res/chs"
#define DEFAULT_DIR "/res/default"
#define ORIGIN_TOKEN ":"
void MainWindow::initFolder()
{
    auto checkAndCreateFolder = [](QString path, bool isCopy)
    {

        QString curPath = QDir::currentPath();
        QDir dir(curPath + path);
        if(!dir.exists())
        {
            dir.mkpath(curPath + path);
            if(isCopy)
            {
                QString originPath = ORIGIN_TOKEN;
                GlobalFunction::copyDirectoryFiles(originPath + path, curPath + path,true);
            }
        }
    };
    checkAndCreateFolder(BASE_DIR, false);
    checkAndCreateFolder(ERR_DIR, true);
    checkAndCreateFolder(DEFAULT_DIR, true);
}

MainWindow::~MainWindow()
{
    delete ui;
}
