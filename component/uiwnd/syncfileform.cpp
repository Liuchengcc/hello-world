#include "syncfileform.h"
#include "ui_syncfileform.h"
#include "DockManager.h"
#include "programsctrl.h"
#include "iconhelper.h"
#include "globalfunction.h"

const QString dockHeadName("同步文件");

SyncFileForm* SyncFileForm::instance_ = nullptr;
SyncFileForm *SyncFileForm::getInstance()
{
    if(!instance_)
    {
        instance_ = new SyncFileForm;
    }
    return instance_;
}

SyncFileForm::SyncFileForm(QWidget *parent) :
    BaseWidget(parent),
    ui(new Ui::SyncFileForm)
{
    ui->setupUi(this);
    IconHelper::Instance()->setIcon(ui->pushButton_toController, QChar(0xf105),12);
    IconHelper::Instance()->setIcon(ui->pushButton_toControllerAll, QChar(0xf101),12);
    IconHelper::Instance()->setIcon(ui->pushButton_fromController, QChar(0xf104),12);
    IconHelper::Instance()->setIcon(ui->pushButton_fromControllerAll, QChar(0xf100),12);
    initDock();
    pgctrl_ = ProgramsCtrl::getInstance();
    connect(ui->lineEdit_filter, &QLineEdit::textChanged, this, [this]()
    {
        setFilelistFilter();
    });
    connect(ui->pushButton_toController, &QPushButton::clicked, this, [this]()
    {
        QString pgName = ui->comboBox_programs->currentText();
        if(ui->listWidget_localFiles->currentItem() == nullptr || pgName.isEmpty())return;
        QString fileName = ui->listWidget_localFiles->currentItem()->text();
        pgctrl_->downLoadToControllerFile(pgName, fileName);
    });
    connect(ui->pushButton_toControllerAll, &QPushButton::clicked, this, [this]()
    {
        QString pgName = ui->comboBox_programs->currentText();
        pgctrl_->downLoadToControllerFile(pgName);
    });
    connect(ui->pushButton_fromController, &QPushButton::clicked, this, [this]()
    {
        if(ui->comboBox_programs->currentText().isEmpty() || ui->listWidget_controllerFiles->currentItem() == nullptr)return;
        pgctrl_->upLoadFromControllerFile(ui->comboBox_programs->currentText(), ui->listWidget_controllerFiles->currentItem()->text()) ;
    });
    connect(ui->pushButton_fromControllerAll, &QPushButton::clicked, this, [this]()
    {
        pgctrl_->upLoadFromControllerFile(ui->comboBox_programs->currentText());
    });
    connect(ui->comboBox_programs, &QComboBox::currentTextChanged, this, [this](QString pgname)
    {
        ui->listWidget_localFiles->clear();
        ui->listWidget_localFiles->addItems(pgctrl_->getProgram(pgname)->get_fileNamesList_());
        currentPrjFileList_.clear();
        currentPrjFileList_.append(pgctrl_->getProgram(pgname)->get_fileNamesList_());
    });
    connect(ui->pushButton_delLocalFile, &QPushButton::clicked, this, [this]
    {
        QString pgName = ui->comboBox_programs->currentText();
        QString fileName = ui->listWidget_localFiles->currentItem()->text();
        if(pgName.isEmpty() || fileName.isEmpty())
        {
            return;
        }
        pgctrl_->delLocalFile(pgName, fileName);
    });
    connect(ui->pushButton_delLocalGarbageFile, &QPushButton::clicked, this, [this]
    {
        if(ui->comboBox_programs->currentText().isEmpty())
        {
            return;
        }
        QString pgName = ui->comboBox_programs->currentText();
        pgctrl_->delLocalGarbageFiles(pgName);

    });
    connect(ui->pushButton_delControllerFile, &QPushButton::clicked, this, [this]
    {
        if(!ui->listWidget_controllerFiles->currentItem())
        {
            return;
        }
        QString fileName = ui->listWidget_controllerFiles->currentItem()->text();
        pgctrl_->delControllerFile(fileName);

    });
    connect(ui->pushButton_delGarbageFile, &QPushButton::clicked, [this]
    {
        pgctrl_->delControllerGarbageFiles();
    });
    connect(ui->pushButton_reflashControllerFile, &QPushButton::clicked, [this]
    {
        ui->listWidget_controllerFiles->clear();
        pgctrl_->updateControllerFileList();
    });
    connect(pgctrl_, &ProgramsCtrl::change_programNameList_, this, [this]()
    {
        QString currentPg = ui->comboBox_programs->currentText();
        QStringList pglist = pgctrl_->get_programNameList_();

        ui->comboBox_programs->blockSignals(true);
        ui->comboBox_programs->clear();
        ui->comboBox_programs->blockSignals(false);
        ui->comboBox_programs->addItems(pglist);
        if(pglist.contains(currentPg))
        {
            ui->comboBox_programs->setCurrentText(currentPg);
        }
    });
    connect(pgctrl_,&ProgramsCtrl::updateProgramFileList, this, [this](QString pgname, QStringList pgfilesName)
    {
        Q_UNUSED(pgfilesName);
        if(ui->comboBox_programs->currentText() == pgname)
        {
            ui->listWidget_localFiles->clear();
            ui->listWidget_localFiles->addItems(pgctrl_->getProgram(pgname)->get_fileNamesList_());
            currentPrjFileList_.clear();
            currentPrjFileList_.append(pgctrl_->getProgram(pgname)->get_fileNamesList_());
        }
        setFilelistFilter();
    });
    connect(pgctrl_, &ProgramsCtrl::change_controllerFileList_, this, [this]()
    {
        ui->listWidget_controllerFiles->clear();
        ui->listWidget_controllerFiles->addItems(pgctrl_->get_controllerFileList_());
        controllerFileList_.clear();
        controllerFileList_.append(pgctrl_->get_controllerFileList_());
        setFilelistFilter();
    });
}

SyncFileForm::~SyncFileForm()
{
    delete ui;
}

void SyncFileForm::initDock()
{
    area_ = ads::CenterDockWidgetArea;
    dock_ = new ads::CDockWidget(dockHeadName);
    dock_->setWidget(this);
    dock_->setFeatures(ads::CDockWidget::AllDockWidgetFeatures);
    connect(dock_, &ads::CDockWidget::closeRequested, [this]()
    {
        qDebug()<<"dock requested" ;
        dock_->dockManager()->removeDockWidget(dock_);
    });
}

void SyncFileForm::updateUi()
{
    ProgramsCtrl* pgctrl =pgctrl_;
    if(ui->comboBox_programs->count() != pgctrl->get_programNameList_().size())
    {
        ui->comboBox_programs->clear();
        QString pgName = ui->comboBox_programs->currentText();
        ui->comboBox_programs->addItems(pgctrl->get_programNameList_());
        ui->comboBox_programs->setCurrentText(pgName);
    }
}

void SyncFileForm::setFilelistFilter()
{
    QString filter = ui->lineEdit_filter->text();
    QStringList locallist = currentPrjFileList_.filter(filter);
    QStringList controllerList = controllerFileList_.filter(filter);
    ui->listWidget_localFiles->clear();
    ui->listWidget_localFiles->addItems(locallist);
    ui->listWidget_controllerFiles->clear();
    ui->listWidget_controllerFiles->addItems(controllerList);
}
