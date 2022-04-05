#include "programbar.h"
#include "ui_programbar.h"
#include "qdebug.h"
#include "programfile.h"
#include "qtreewidget.h"
#include "qstring.h"
#include "DockWidget.h"
#include "DockAreaWidget.h"
#include "DockManager.h"
#include <functional>
#include <QMenu>
#include <QMessageBox>
#include <instanceselectdialog.h>
#include <commondialog.h>
#include <globalfunction.h>
#include <QDesktopServices> //win32
#include <iconhelper.h>


#define DEFAULT_SCARA_APTH ":/res/scara.ist"
#define DEFAULT_PUMA_APTH ":/res/puma.ist"
#define DEFAULT_MODULE_APTH ":/res/module.ist"
#define DEFAULT_AXIS_APTH ":/res/axis.json"

const QString c_dockHeadName("项目");
ProgramBar* ProgramBar::instance_ = nullptr;
ProgramBar::ProgramBar(QWidget *parent) :
    BaseWidget(parent),
    ui(new Ui::ProgramBar)
{
    ui->setupUi(this);
    nameMap_["axis.json"] = tr("轴配置");
    nameMap_["Points.pt"] = tr("点位配置");
    nameMap_["Tools.cr"] = tr("工具坐标系");
    nameMap_["Users.cr"] = tr("用户坐标系");
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget, &QTreeWidget::itemDoubleClicked, this, [this]()
    {
        ui->action_openFile->triggered();
    });
    connect(ui->action_newFile, &QAction::triggered, this, [this]()
    {
        QString fileName;
        CommonDialog dia(tr("创建新文件"), QStringList(tr("文件名").split(" ")), this);
        if(CommonDialog::Accepted ==  dia.exec())
        {
            fileName = dia.getDialogData()[0];
            fileName = fileName.right(3) == tr(".lh") ? fileName:fileName+".lh";
        }
        pgCtrl_->getProgram(getItemRootName(ui->treeWidget->currentItem()))->addOrOverwriteNewProgramFile(fileName, QByteArray().append("main(){}"));
        updateUi();
    });
    connect(ui->action_delFile, &QAction::triggered, this, [this]()
    {
        QString fileName = getItemFileName(ui->treeWidget->currentItem());
        if(QMessageBox::Ok == QMessageBox::information(this,"提示", "确认删除文件？", QMessageBox::Ok|QMessageBox::Cancel))
        {
            pgCtrl_->getProgram(getItemRootName(ui->treeWidget->currentItem()))->deleteProgramFile(fileName);
            updateUi();
        }
    });
    connect(ui->action_newInstance, &QAction::triggered, this, [this]()
    {
        InstanceSelectDialog tempDialog(this);
        if(tempDialog.exec() == QDialog::Accepted)
        {
            QMap<InstanceSelectDialog::InstanceType, QString> tempTypeMap;
            tempTypeMap[InstanceSelectDialog::scara] = DEFAULT_SCARA_APTH;
            tempTypeMap[InstanceSelectDialog::puma] = DEFAULT_PUMA_APTH;
            tempTypeMap[InstanceSelectDialog::module] = DEFAULT_MODULE_APTH;
            QString fileName = tempDialog.getFileName();
            QString programName = getItemRootName(ui->treeWidget->currentItem());
            if(pgCtrl_->getProgram(programName)->containFile(fileName))
            {
                qDebug()<<__FILE__<<__LINE__<<"contain file:"<<fileName;
                return;
            }
            QByteArray content;
            GlobalFunction::ReadDocFileToByteArray(tempTypeMap[tempDialog.getInstanceType()], content);
            pgCtrl_->getProgram(programName)->addOrOverwriteNewProgramFile(fileName, content);
            updateUi();
            emit openInstance(pgCtrl_->getProgram(programName)->get_path_()+fileName);
        }
    });
    connect(ui->action_openFile, &QAction::triggered, this, [this]
    {
        QTreeWidgetItem* item = ui->treeWidget->currentItem();
        QString programName = getItemRootName(item);
        QString programFileName = getItemFileName(item);
        QString programFileAbsPath = pgCtrl_->getProgram(programName)->get_path_()+programFileName;
        QMap<int, std::function<void(void)>> actionMap;
        actionMap.clear();
        //动态生成的界面要知道是文件的path。
        actionMap[ProgramFile::JobFile] = [this, programFileAbsPath]{emit openluaFile(programFileAbsPath);};
        actionMap[ProgramFile::ToolFile] = [this, programFileAbsPath]{emit openTools(programFileAbsPath);};
        actionMap[ProgramFile::UserFile] = [this, programFileAbsPath]{emit openUsers(programFileAbsPath);};
        actionMap[ProgramFile::PointFile] = [this, programFileAbsPath]{emit openPoints(programFileAbsPath);};
        actionMap[ProgramFile::InstanceFile] = [this, programFileAbsPath]{emit openInstance(programFileAbsPath);};
        actionMap[ProgramFile::ConfigFile] = [this, programFileAbsPath]{emit openConfig(programFileAbsPath);};
        actionMap[ProgramFile::OtherFile] = [this, programFileAbsPath]{emit openOthers(programFileAbsPath);};
        if(actionMap.contains(item->type()))actionMap[item->type()]();
    });
    connect(ui->action_setActive, &QAction::triggered, this, [this]
    {
        QTreeWidgetItem* item = ui->treeWidget->currentItem();
        QTreeWidgetItem* rootItem = item;
        while (rootItem->type() != Root)
        {
            rootItem = rootItem->parent();
        }
        pgCtrl_->setActiveProgram(rootItem->text(0));
        updateUi();
    });
    connect(ui->action_openFolder, &QAction::triggered, this, [this]
    {//win32
        QTreeWidgetItem* item = ui->treeWidget->currentItem();
        QTreeWidgetItem* rootItem = item;
        while (rootItem->type() != Root)
        {
            rootItem = rootItem->parent();
        }
        QString dir = pgCtrl_->getProgram(rootItem->text(0))->get_path_();
        QDesktopServices::openUrl(QUrl::fromLocalFile(dir));
    });

    connect(ui->treeWidget, &QTreeWidget::customContextMenuRequested, this, [this](QPoint pos)
    {
        QTreeWidgetItem* item = ui->treeWidget->itemAt(pos);
        if(!item)
        {
            return;
        }
        qDebug()<< item->type();
        QMenu *popMenu =new QMenu(this);//定义一个右键弹出菜单
#define CHECK_TYPE_AND_ADD_ACTION(FILE_TYPE,ACTION) if(item->type() == FILE_TYPE){popMenu->addAction(ACTION);};
#define CHECK_TYPE_AND_ADD_SEPARATOR(FILE_TYPE) if(item->type() == FILE_TYPE){popMenu->addSeparator();};
        CHECK_TYPE_AND_ADD_ACTION(Root, ui->action_newPg);
        CHECK_TYPE_AND_ADD_SEPARATOR(Root);
        CHECK_TYPE_AND_ADD_ACTION(Root, ui->action_newFile);
        CHECK_TYPE_AND_ADD_ACTION(Root, ui->action_newInstance);
        CHECK_TYPE_AND_ADD_SEPARATOR(Root);
        CHECK_TYPE_AND_ADD_ACTION(Root, ui->action_setActive);
        CHECK_TYPE_AND_ADD_SEPARATOR(Root);
        CHECK_TYPE_AND_ADD_ACTION(Root, ui->action_openFolder);
        CHECK_TYPE_AND_ADD_ACTION(ProgramFile::JobFile, ui->action_openFile);
        CHECK_TYPE_AND_ADD_ACTION(ProgramFile::JobFile, ui->action_newFile);
        CHECK_TYPE_AND_ADD_ACTION(ProgramFile::JobFile, ui->action_delFile);
        CHECK_TYPE_AND_ADD_ACTION(ProgramFile::UserFile, ui->action_openFile);
        CHECK_TYPE_AND_ADD_ACTION(ProgramFile::ToolFile, ui->action_newFile);
        CHECK_TYPE_AND_ADD_ACTION(ProgramFile::ToolFile, ui->action_openFile);
        CHECK_TYPE_AND_ADD_ACTION(ProgramFile::PointFile, ui->action_openFile);
        CHECK_TYPE_AND_ADD_ACTION(ProgramFile::ConfigFile, ui->action_openFile);
        CHECK_TYPE_AND_ADD_ACTION(ProgramFile::ConfigFile, ui->action_delFile);
        CHECK_TYPE_AND_ADD_ACTION(ProgramFile::InstanceFile, ui->action_openFile);
        CHECK_TYPE_AND_ADD_ACTION(ProgramFile::InstanceFile, ui->action_delFile);
        CHECK_TYPE_AND_ADD_ACTION(ProgramFile::OtherFile, ui->action_openFile);
        CHECK_TYPE_AND_ADD_ACTION(ProgramFile::OtherFile, ui->action_delFile);
        popMenu->exec(QCursor::pos());//弹出右键菜单，菜单位置为光标位置
    });
    initDock();
    pgCtrl_ = ProgramsCtrl::getInstance();
    connect(pgCtrl_, &ProgramsCtrl::updateProgramFileList, this, [this](){updateUi();});
}

ProgramBar::~ProgramBar()
{
    delete ui;
}

void ProgramBar::updateUi()
{
    ui->treeWidget->clear();
    QList<Program *>* pgList =  pgCtrl_->getProgramList();
    foreach (Program* pg, *pgList)
    {
        loadProgram(pg);
    }
    ui->treeWidget->expandAll();
}

int ProgramBar::newProgram(QString folderPath, QString pgName)
{
    pgCtrl_->newProgram(folderPath, pgName);
    loadProgram(folderPath+pgName +"/"+pgName +".lhprj");
    return 0;
}

int ProgramBar::loadProgram(Program * program)
{
    if(!program)
    {
        qDebug()<<"load file error";
        return -1;
    }
    ui->treeWidget->setColumnCount(1);
    QTreeWidgetItem* baseItem = new QTreeWidgetItem(ui->treeWidget, QStringList()<<program->get_programName_(), Root);

    ui->treeWidget->addTopLevelItem(baseItem);
    {
        if(program == pgCtrl_->getActiveProgram())
        {
            QFont tempfont = baseItem->font(0);
            tempfont.setBold(true);
            baseItem->setFont(0,tempfont);
        }
        else
        {
            QFont tempfont = baseItem->font(0);
            tempfont.setBold(false);
            baseItem->setFont(0,tempfont);
        }
    }
    auto fileTypeName = ProgramFile::getTypeName();
    QMap<int, QChar> fontMap;
    fontMap[ProgramFile::JobFile] = QChar(0xf1c9);
    fontMap[ProgramFile::PointFile] = QChar(0xf1c4);
    fontMap[ProgramFile::ToolFile] = QChar(0xf1c3);
    fontMap[ProgramFile::UserFile] = QChar(0xf1c3);
    fontMap[ProgramFile::LogFile] = QChar(0xf0f6);
    fontMap[ProgramFile::ConfigFile] = QChar(0xf085);
    fontMap[ProgramFile::InstanceFile] = QChar(0xf1b2);
    fontMap[ProgramFile::OtherFile] = QChar(0xf016);
    for(auto it = fileTypeName.begin(); it != fileTypeName.end(); it++)
    {
        QString typeName = it.value();
        QTreeWidgetItem* foldItem =new QTreeWidgetItem(baseItem, QStringList() << typeName, Folder + it.key());

        baseItem->addChild(foldItem);

        for(auto itPg = program->getProgramFileList()->begin(); itPg != program->getProgramFileList()->end(); itPg++)
        {
            QString fileName = (*itPg)->getFileName();
            ProgramFile::FileType type = (*itPg)->getFileType();
            if(it.key() == type)
            {
                if(nameMap_.contains(fileName))
                {
                    fileName = nameMap_[fileName];
                }
                else if(type == ProgramFile::InstanceFile)
                {
                    nameMap_[fileName] = tr("实例 %1").arg(fileName.split(".").first());
                    fileName = nameMap_[fileName];
                }
                QTreeWidgetItem* fileItem = new QTreeWidgetItem(foldItem, QStringList() << fileName, it.key());
                fileItem->setIcon(0, QIcon(IconHelper::Instance()->getPixmap("#57595b", fontMap[type], 24, 24, 24)));
                foldItem->addChild(fileItem);
            }
        }
    }
    return 0;
}

int ProgramBar::loadProgram(QString path)
{
    if(pgCtrl_->loadProgram(path))
    {
        return -1;
    }
    loadProgram(pgCtrl_->getProgram(path.split("/").last().split(".").at(0)));
    return 0;
}

void ProgramBar::initDock()
{
    area_ = ads::LeftDockWidgetArea;
    dock_ = new ads::CDockWidget(c_dockHeadName);
    dock_->setWidget(this);
    dock_->setFeatures(ads::CDockWidget::AllDockWidgetFeatures);
    connect(dock_, &ads::CDockWidget::closeRequested, [this]()
    {
        qDebug()<<"dock requested" ;
        dock_->dockManager()->removeDockWidget(dock_);
//        dock_->dockAreaWidget()->hide();
    });
    connect(dock_, &ads::CDockWidget::closed, []()
    {
        qDebug()<<"dock is closed";
    });
    connect(dock_, &ads::CDockWidget::visibilityChanged, [](bool status)
    {
        qDebug()<<"dock is visibility" << status;
    });
}

QString ProgramBar::getItemRootName(QTreeWidgetItem *item)
{
    while (item->type() != Root)
    {
        item = item->parent();
    }
    return item->text(0);
}

QString ProgramBar::getItemFileName(QTreeWidgetItem *item)
{
    if(nameMap_.values().contains(item->text(0)))
    {
        return nameMap_.key(item->text(0));
    }
    return item->text(0);
}
