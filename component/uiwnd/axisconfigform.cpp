#include "axisconfigform.h"
#include "ui_axisconfigform.h"
#include "DockManager.h"
#include "globalfunction.h"
#include "json/value.h"
#include "programsctrl.h"
#include "program.h"
#include "controller.h"
#include "iconhelper.h"

const QString dockHeadName("轴配置");
const QString c_axisConfigName("aixs.json");
const QString c_axisFileType("axis");
const QString c_defaultAxisConfigPath("./" + c_axisConfigName);
#define AXIS_SUM 32
#define AXIS_STRUCT_COMMENT "减速比 分辨率 当量比 绝对零点 零点偏移 电机方向 模型方向 正极限 负极限 速度极限 加速度极限"
#define COMMENT_SPLIT " "

AxisConfigForm::AxisConfigForm(QString filePath, QWidget *parent) :
    BaseWidget(parent),
    filePath_(filePath),
    ui(new Ui::AxisConfigForm)
{
    ui->setupUi(this);
    for(int i = 0; i <= AXIS_SUM; i++)
    {
        ui->comboBox_axisSum->addItem(QString::number(i));
    }
    AxisVec_ = QVector<Axis>(AXIS_SUM);
    initDock();
    set_fileName_(filePath.split("/").last());
    set_fileFolderPath_(filePath.left(filePath.size() - filePath.split("/").last().size()));
    ProgramsCtrl::getInstance()->getProgram(fileFolderPath_.split("/", Qt::SkipEmptyParts).last())->readAndParserAxisesFile();
    AxisVec_ = ProgramsCtrl::getInstance()->getProgram(fileFolderPath_.split("/", Qt::SkipEmptyParts).last())->get_AxisVec_();
    axisesSum_ = ProgramsCtrl::getInstance()->getProgram(fileFolderPath_.split("/", Qt::SkipEmptyParts).last())->get_axisesSum_();
    connect(ui->pushButton_getEncoder, &QPushButton::clicked, this, []()
    {
        Controller::getInstance()->getCurrentEncorder();
    });
    connect(Controller::getInstance(), &Controller::receiveGetEncorder, this,[this](QStringList recvData)
    {
      QList<QTableWidgetItem*> itemList = ui->tableWidget_axis->selectedItems();
          int row = QString(AXIS_STRUCT_COMMENT).split(COMMENT_SPLIT).indexOf(tr("绝对零点"));
          foreach (auto item, itemList)
          {
              if(item->row() == row && recvData.size() > item->column()){item->setText(recvData.at(item->column()));}
          }
    });
    connect(ui->pushButton_apply, &QPushButton::clicked, this, [this]()
    {
#define SAVE_AXIS_FROM_UI(axisVec, data) \
    axisVec[i].data = ui->tableWidget_axis->item(j, i)->text();j++;
    for(int i = 0; i < AXIS_SUM; i++)
        {
           int j = 0;
           DO_AXIS_STRUCT(SAVE_AXIS_FROM_UI, AxisVec_);
        }
    QString axisesSum = ui->comboBox_axisSum->currentText();
    ProgramsCtrl::getInstance()->getProgram(fileFolderPath_.split("/", Qt::SkipEmptyParts).last())->saveAxisDataAndToFile(axisesSum, AxisVec_);
#undef SAVE_AXIS_FROM_UI
    GlobalFunction::showLogMsg(tr("应用轴配置，写入文件..."));
    });
    connect(ui->pushButton_mapAxis, &QPushButton::clicked, this, [this]()
    {
        int originAxisIndex = ui->comboBox_originAxis->currentIndex();
        int dstAxisIndex = ui->comboBox_dstAxis->currentIndex();
        for(int i = 0;i < ui->tableWidget_axis->rowCount(); i++)
        {
            ui->tableWidget_axis->item(i, dstAxisIndex)->setText(ui->tableWidget_axis->item(i, originAxisIndex)->text());
        }
    });
    updateUi();
}

void AxisConfigForm::updateUi()
{
    //初始化
    IconHelper::Instance()->setIcon(ui->pushButton_mapAxis, QChar(0xf101),12);
    ui->tableWidget_axis->setRowCount(QString(AXIS_STRUCT_COMMENT).split(" ").size());
    ui->tableWidget_axis->setColumnCount(AXIS_SUM);
    ui->tableWidget_axis->setVerticalHeaderLabels(QString(AXIS_STRUCT_COMMENT).split(" "));
    for(int i = 0; i < AXIS_SUM; i++)
    {
        ui->tableWidget_axis->setHorizontalHeaderItem(i, new QTableWidgetItem(QString(tr("轴%1")).arg(i)));
        ui->comboBox_dstAxis->addItem(QString(tr("轴%1")).arg(i));
        ui->comboBox_originAxis->addItem(QString(tr("轴%1")).arg(i));
    }
    //更新值
    ui->comboBox_axisSum->setCurrentText(axisesSum_);
#define UPDATE_AXIS_TO_UI(axisVec, data) \
    ui->tableWidget_axis->setItem(j, i, new QTableWidgetItem(axisVec.at(i).data));j++;
    for(int i = 0; i < AXIS_SUM; i++)
    {
        int j = 0;
        DO_AXIS_STRUCT(UPDATE_AXIS_TO_UI, AxisVec_);
    }
#undef UPDATE_AXIS_TO_UI
    //隐藏或显示轴
    QList<bool> columShowList;
    auto appendState = [&columShowList](int times, QCheckBox* box)
    {
        for(int i = 0; i < times; i++)
        {
            columShowList.append(box->isChecked());
        }
    };
    appendState(8, ui->checkBox8);
    appendState(8, ui->checkBox16);
    appendState(8, ui->checkBox24);
    appendState(8, ui->checkBox32);
    for(int i = 0; i < AXIS_SUM; i++)
    {
        if(columShowList[i]){ui->tableWidget_axis->showColumn(i);}
        else{ui->tableWidget_axis->hideColumn(i);}
    }
}


AxisConfigForm::~AxisConfigForm()
{
    delete ui;
}

void AxisConfigForm::initDock()
{
    area_ = ads::CenterDockWidgetArea;
    dock_ = new ads::CDockWidget(dockHeadName);
    dock_->setWidget(this);
    dock_->setFeatures(ads::CDockWidget::AllDockWidgetFeatures);
    connect(ui->checkBox8, &QCheckBox::stateChanged,this,[this]{updateUi();});
    connect(ui->checkBox16, &QCheckBox::stateChanged,this,[this]{updateUi();});
    connect(ui->checkBox24, &QCheckBox::stateChanged,this,[this]{updateUi();});
    connect(ui->checkBox32, &QCheckBox::stateChanged,this,[this]{updateUi();});
    connect(dock_, &ads::CDockWidget::visibilityChanged, this, [](bool visable)
    {
       qDebug()<<"visable changed!" << visable;
    });
    connect(dock_, &ads::CDockWidget::closeRequested, [this]()
    {
        qDebug()<<"dock requested" ;
        dock_->closeDockWidget();
    });
    if(!QFile::exists(c_defaultAxisConfigPath))
    {
        Json::Value root;
        root["type"] = c_axisFileType.toStdString();
        root["axisesSum"] = "4";
#define DEFAULT_AXIS(axisVec,data) root["axises"][i][#data] = axisVec[i].data.toStdString();
        for(int i = 0; i < AXIS_SUM; i++){DO_AXIS_STRUCT(DEFAULT_AXIS, AxisVec_);}
    #undef DEFAULT_AXIS
        GlobalFunction::WriteJsonToJsonfile(root, c_defaultAxisConfigPath);
    }
}
