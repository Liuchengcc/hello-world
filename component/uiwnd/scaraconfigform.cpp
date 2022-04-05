#include "scaraconfigform.h"
#include "ui_scaraconfigform.h"
#include "DockManager.h"
#include "json/value.h"
#include "globalfunction.h"
#include "qgridlayout.h"
#include "qgroupbox.h"
#include "qlabel.h"
#include "marcroly.h"
#include "qlineedit.h"
#include "qmap.h"
#include <qtimer.h>

#define C_TYPE "scara"
#define AXIS_MAX_SUM 9
#define TABLE_MINI_HEIGHT 32
const QString c_split_token = ";";
const QString c_dockHeadName("Scara配置");
const QString c_type(C_TYPE);
const QString c_scaraTableCommonHorison = ("关节停止减速度;连杆参数");
const QString c_scaraAxisNames = ("J1;J2;J3;J4;J5;J6;J7;J8;J9");
const QString c_scaraAxisSumComboDatas= ("0;1;2;3;4;5;6;7;8;9");
const QString c_scaraTableCoupleHorison = ("M1;M2;M3;M4;M5;M6;M7;M8;M9");
const QString c_scaraTableWithAxisHorizon = ("关节最大速度限制;关节最大加速度限制;关节正极限;关节负极限");
const QString c_scaraComboboxList = ("轴0;轴1;轴2;轴3;轴4;轴5;轴6;轴7;轴8;轴9;轴10;轴11;轴12;轴13;轴14;轴15;轴16;轴17;轴18;轴19;轴20;轴21;轴22;轴23;轴24;轴25;轴26;轴27;轴28;轴29;轴30;轴31");

#define DO_FOREACH_IN_AXIS_AND_INDEX(NUM, FUN);

class IconHelper;
ScaraConfigForm::ScaraConfigForm(QString filePath, QWidget *parent):
    BaseWidget(parent),
    ui(new Ui::ScaraConfigForm)
{
    ui->setupUi(this);

    set_filePath_(filePath);
    pgName_ = filePath.split("/").at(filePath.split("/").size()-2);
    AxisVec_ = ProgramsCtrl::getInstance()->getProgram(pgName_)->get_AxisVec_();
    initDock();
    readAndParserFile();
    initUi();
    updateUi();
    set_md5Data_(calAllUiData());
}

ScaraConfigForm::~ScaraConfigForm()
{
    delete ui;
}

void ScaraConfigForm::initDock()
{
    area_ = ads::CenterDockWidgetArea;
    QString headName = tr("%1 配置").arg(filePath_.split("/").last().split(".").at(0));
    dock_ = new ads::CDockWidget(headName);
    dock_->setWidget(this);
    connect(dock_, &ads::CDockWidget::visibilityChanged, this, [](bool visable)
    {
       qDebug()<<"visable changed!" << visable;
    });
    connect(dock_, &ads::CDockWidget::closeRequested, [this]()
    {
        qDebug()<<"dock requested" ;
        dock_->closeDockWidget();

    });

}

void ScaraConfigForm::initUi()
{
    ui->comboBox_axisSum->clear();
    ui->comboBox_axisSum->addItems(c_scaraAxisSumComboDatas.split(c_split_token));
//初始化单个参数
#define CREATE_UI_LABEL(OBJ_NAME, OBJ_COMMENT) uiMap_[STRING_LATER(CAT_2_TOKEN(label_,OBJ_NAME))] = new QLabel(tr(OBJ_COMMENT), this);
#define CREATE_UI_LINEEDIT(OBJ_NAME) uiMap_[STRING_LATER(CAT_2_TOKEN(lineedit_,OBJ_NAME))] = new QLineEdit(this);
#define CREATE_UI_LABEL_WITH_LINEEDIT(LAYOUT_PTR, ROW, COLUMN, OBJ_NAME, OBJ_COMMENT)\
    CREATE_UI_LABEL(OBJ_NAME, OBJ_COMMENT)\
    LAYOUT_PTR->addWidget(uiMap_[STRING_LATER(CAT_2_TOKEN(label_,OBJ_NAME))], ROW, COLUMN*2);\
    CREATE_UI_LINEEDIT(OBJ_NAME)\
    LAYOUT_PTR->addWidget(uiMap_[STRING_LATER(CAT_2_TOKEN(lineedit_,OBJ_NAME))], ROW, COLUMN*2 + 1);
    CREATE_UI_LABEL_WITH_LINEEDIT(ui->gridLayout_common, 0, 0, RobotType, "机器人类型");
    CREATE_UI_LABEL_WITH_LINEEDIT(ui->gridLayout_common, 0, 1, Car_MaxPVel, "笛卡尔位置最大速度mm/s");
    CREATE_UI_LABEL_WITH_LINEEDIT(ui->gridLayout_common, 1, 1, Car_MaxGVel, "笛卡尔姿态最大速度deg/s");
    CREATE_UI_LABEL_WITH_LINEEDIT(ui->gridLayout_common, 0, 2, Car_MaxPAcc, "笛卡尔位置加速度");
    CREATE_UI_LABEL_WITH_LINEEDIT(ui->gridLayout_common, 1, 2, Car_MaxGAcc, "笛卡尔姿态加速度");
    CREATE_UI_LABEL_WITH_LINEEDIT(ui->gridLayout_common, 0, 3, Car_StopPDec, "笛卡尔位置停止减速度");
    CREATE_UI_LABEL_WITH_LINEEDIT(ui->gridLayout_common, 1, 3, Car_StopGDec, "笛卡尔姿态停止减速度");

    //初始化表格参数
    auto initTableWidget = [](QTableWidget *table, QString horizonHeader, QString verticalHeader)
    {
        int columnCount = horizonHeader.split(c_split_token).size();
        int rowCount = verticalHeader.split(c_split_token).size();
        table->setRowCount(rowCount);
        table->setColumnCount(columnCount);
        table->setHorizontalHeaderLabels(horizonHeader.split(c_split_token));
        table->setVerticalHeaderLabels(verticalHeader.split(c_split_token));
        for(int i = 0; i < rowCount; i++)
        {
            for(int j = 0; j < columnCount; j++)
            {
                table->setItem(i, j, new QTableWidgetItem);
            }
        }
    };
    initTableWidget(ui->tableWidget_common, c_scaraTableCommonHorison, c_scaraAxisNames);
    initTableWidget(ui->tableWidget_couple, c_scaraTableCoupleHorison, c_scaraAxisNames);
    for(int row = 0; row < ui->tableWidget_couple->rowCount(); row++)
    {
        for(int column = 0; column < ui->tableWidget_couple->columnCount(); column++)
        {
           if(row != column){ui->tableWidget_couple->item(row, column)->setFlags((Qt::ItemFlags)63);}
           else{ui->tableWidget_couple->item(row, column)->setFlags((Qt::ItemFlags)0);}
        }
    }
    //初始化轴共有参数
    initTableWidget(ui->tableWidget_withAxis, c_scaraTableWithAxisHorizon, c_scaraAxisNames);
    //初始化comboboxAxis

#define CREATE_UI_COMBO(OBJ_NAME) uiMap_[STRING_LATER(CAT_2_TOKEN(combobox_,OBJ_NAME))] = new QComboBox(this);
#define CREATE_LABEL_WITH_COMBO(OBJ_NAME, ROW)\
    CREATE_UI_COMBO(OBJ_NAME)\
    CREATE_UI_LABEL(OBJ_NAME, #OBJ_NAME)\
    ui->gridLayout_axis->addWidget(uiMap_[STRING_LATER(CAT_2_TOKEN(label_,OBJ_NAME))], ROW, 0);\
((QComboBox*)uiMap_[STRING_LATER(CAT_2_TOKEN(combobox_,OBJ_NAME))])->addItems(c_scaraComboboxList.split(c_split_token));\
    ui->gridLayout_axis->addWidget(uiMap_[STRING_LATER(CAT_2_TOKEN(combobox_,OBJ_NAME))], ROW, 1);\
    connect(((QComboBox*)uiMap_[STRING_LATER(CAT_2_TOKEN(combobox_,OBJ_NAME))]),QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index)\
    {\
       if(ui->checkBox_notFollowAxis){return;}\
       ui->tableWidget_withAxis->item(ROW, 0)->setText(AxisVec_.at(index).VelLim);\
       ui->tableWidget_withAxis->item(ROW, 1)->setText(AxisVec_.at(index).AccLim);\
       ui->tableWidget_withAxis->item(ROW, 2)->setText(AxisVec_.at(index).PosPLim);\
       ui->tableWidget_withAxis->item(ROW, 3)->setText(AxisVec_.at(index).PosNLim);\
    });

   DO_FOREACH_IN_AXIS_INDEX_LATER(AXIS_MAX_SUM, CREATE_LABEL_WITH_COMBO)
   connect(ui->comboBox_axisSum, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index)
   {

#define SHOW_COMB_AXIS_AND_INDEX(OBJ_NAME, INDEX)\
       ((QComboBox*)uiMap_[STRING_LATER(CAT_2_TOKEN(combobox_,OBJ_NAME))])->show();\
       ((QLabel*)uiMap_[STRING_LATER(CAT_2_TOKEN(label_,OBJ_NAME))])->show();\
       ui->tableWidget_common->showRow(INDEX);\
       ui->tableWidget_couple->showRow(INDEX);\
       ui->tableWidget_couple->showColumn(INDEX);\
       ui->tableWidget_withAxis->showRow(INDEX);
#define HIDE_COMB_AXIS_AND_INDEX(OBJ_NAME, INDEX)\
       ((QComboBox*)uiMap_[STRING_LATER(CAT_2_TOKEN(combobox_,OBJ_NAME))])->hide();\
       ((QLabel*)uiMap_[STRING_LATER(CAT_2_TOKEN(label_,OBJ_NAME))])->hide();\
       ui->tableWidget_common->hideRow(INDEX);\
       ui->tableWidget_couple->hideRow(INDEX);\
       ui->tableWidget_couple->hideColumn(INDEX);\
       ui->tableWidget_withAxis->hideRow(INDEX);

       DO_FOREACH_IN_AXIS_INDEX_LATER(AXIS_MAX_SUM, HIDE_COMB_AXIS_AND_INDEX);
#define CHECK_SHOW_COMB_AXIS(NUM)\
       if(index == NUM){DO_FOREACH_IN_AXIS_INDEX_LATER(NUM, SHOW_COMB_AXIS_AND_INDEX)};
        DO_FOREACH_AXIS_SUM_LATER(AXIS_MAX_SUM, CHECK_SHOW_COMB_AXIS);
    //最小高度
        ui->tableWidget_common->setMinimumHeight(TABLE_MINI_HEIGHT*(index+1));
        ui->tableWidget_couple->setMinimumHeight(TABLE_MINI_HEIGHT*(index+1));
        ui->tableWidget_withAxis->setMinimumHeight(TABLE_MINI_HEIGHT*(index+1));
   });
   connect(ui->pushButton_apply, &QPushButton::clicked, this, [this]()
   {
#define SAVE_UI_LINEEDIT(OBJ_NAME)\
    if(qobject_cast<QLineEdit*>(uiMap_[STRING_LATER(lineedit_##OBJ_NAME)]))\
       {\
            QLineEdit* edit = (QLineEdit*)uiMap_[STRING_LATER(lineedit_##OBJ_NAME)];\
            scara_.OBJ_NAME = edit->text();\
        }
    DO_FOREACH_ELEMENT(7, SAVE_UI_LINEEDIT, RobotType, Car_MaxGAcc, \
                       Car_MaxGVel, Car_MaxPAcc, Car_MaxPVel, Car_StopGDec, Car_StopPDec);
    scara_.AXISNum = ui->comboBox_axisSum->currentText();
    scara_.RobotID = ui->comboBox_intanceId->currentText();
#define SAVE_UI_COMMON_TABLE(OBJ_NAME, TABLE_INDEX)\
    scara_.OBJ_NAME.clear();\
    for(int i = 0; i < ui->tableWidget_common->rowCount(); i++)\
    {\
        scara_.OBJ_NAME.append(ui->tableWidget_common->item(i, TABLE_INDEX)->text());\
    }
    SAVE_UI_COMMON_TABLE(Joint_StopDec,0);
    SAVE_UI_COMMON_TABLE(Robot_Link,1);
    //couple
    QStringList coupleGroupList;
    for(int i = 0; i < ui->tableWidget_couple->rowCount(); i++)
    {
        for(int j = 0; j < ui->tableWidget_couple->columnCount(); j++)
        {
            if(!ui->tableWidget_couple->item(i, j)->text().isEmpty())
            {
                coupleGroupList.append(tr("%1,%2,%3").arg(j).arg(i).arg(ui->tableWidget_couple->item(i, j)->text()));
            }
        }
    }
    scara_.CoupleRatio = coupleGroupList;
    //axisID
    QStringList axisID;
        #define READ_AXIS_ID_COMBO(OBJ_NAME, ROW)\
    axisID.append(QString::number(((QComboBox*)uiMap_[STRING_LATER(CAT_2_TOKEN(combobox_, OBJ_NAME))])->currentIndex()));
    DO_FOREACH_IN_AXIS_INDEX_LATER(AXIS_MAX_SUM, READ_AXIS_ID_COMBO);
    scara_.AxisID = axisID;
    saveDataToFile();
    //back save axis param
#define READ_UI_AXIS_DATA(OBJ_NAME, ROW)\
    if(scara_.AxisID.size() > ROW)\
    {\
       QComboBox* tempcomb = (QComboBox*)uiMap_[STRING_LATER(CAT_2_TOKEN(combobox_,OBJ_NAME))];\
        int index = tempcomb->currentIndex();\
        AxisVec_[index].VelLim = ui->tableWidget_withAxis->item(ROW, 0)->text();\
        AxisVec_[index].AccLim = ui->tableWidget_withAxis->item(ROW, 1)->text();\
        AxisVec_[index].PosPLim = ui->tableWidget_withAxis->item(ROW, 2)->text();\
        AxisVec_[index].PosNLim = ui->tableWidget_withAxis->item(ROW, 3)->text();\
    }

    DO_FOREACH_IN_AXIS_INDEX_LATER(AXIS_MAX_SUM, READ_UI_AXIS_DATA);
    ProgramsCtrl::getInstance()->getProgram(pgName_)->set_AxisVec_(AxisVec_);
   });

   ui->tableWidget_common->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
   ui->tableWidget_couple->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
   ui->tableWidget_withAxis->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
   ui->tableWidget_common->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
   ui->tableWidget_couple->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
   ui->tableWidget_withAxis->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void ScaraConfigForm::updateUi()
{
#ifdef DEBUG
    QMap<QString, QWidget*>::iterator it;
    for(QMap<QString, QWidget*>::iterator it = uiMap_.begin(); it != uiMap_.end(); it++)
    {
       qDebug()<<it.key()<<" "<<it.value();
    }
#endif
#define UPDATE_UI_LINEEDIT(OBJ_NAME)\
    if(qobject_cast<QLineEdit*>(uiMap_[STRING_LATER(lineedit_##OBJ_NAME)]))\
    {\
        QLineEdit* edit = (QLineEdit*)uiMap_[STRING_LATER(lineedit_##OBJ_NAME)];\
        edit->setText(scara_.OBJ_NAME);\
    }
    DO_FOREACH_ELEMENT(7, UPDATE_UI_LINEEDIT, RobotType, Car_MaxGAcc, \
                       Car_MaxGVel, Car_MaxPAcc, Car_MaxPVel, Car_StopGDec, Car_StopPDec);
    ui->comboBox_axisSum->setCurrentText(QString::number(scara_.AXISNum.toUInt()));
    ui->comboBox_intanceId->setCurrentIndex(scara_.RobotID.toUInt());
    //update table
    for(int i = 0; i < scara_.Joint_StopDec.size() && i < ui->tableWidget_common->rowCount(); i++)
    {
        ui->tableWidget_common->item(i, 0)->setText(scara_.Joint_StopDec.at(i));
    }
    for(int i = 0; i < scara_.Robot_Link.size() && i < ui->tableWidget_common->rowCount(); i++)
    {
        ui->tableWidget_common->item(i, 1)->setText(scara_.Robot_Link.at(i));
    }

    //update couple
    for(int i = 0; i < scara_.CoupleRatio.size(); i++)
    {
        QStringList coupleGroup = scara_.CoupleRatio.at(i).split(",");
        if(coupleGroup.size() != 3)
        {
            continue;
        }
        ui->tableWidget_couple->item(coupleGroup.at(1).toInt(), coupleGroup.at(0).toInt())->setText(coupleGroup.at(2));
    }
    //update with axis
#define UPDATE_UI_AXIS_COMBO(OBJ_NAME, ROW)\
    if(scara_.AxisID.size() > ROW)\
    {\
        ((QComboBox*)uiMap_[STRING_LATER(CAT_2_TOKEN(combobox_,OBJ_NAME))])->setCurrentIndex(1);\
        ((QComboBox*)uiMap_[STRING_LATER(CAT_2_TOKEN(combobox_,OBJ_NAME))])->setCurrentIndex(scara_.AxisID[ROW].toInt());\
    }
    DO_FOREACH_IN_AXIS_INDEX_LATER(AXIS_MAX_SUM, UPDATE_UI_AXIS_COMBO);
}

void ScaraConfigForm::readAndParserFile()
{
    Json::Value root = GlobalFunction::ReadJsonFromJsonfile(filePath_);
    if(QString::fromStdString(root["type"].asString()) != c_type)
    {
        return;
    }
    #define PARSER_JSON(SCARA, data) SCARA.data = QString::fromStdString(root[C_TYPE][#data].asString());
    #define PARSER_JSON_ARRAY(SCARA, data)\
    SCARA.data.clear();\
    for(int i = 0; i < (int)root[C_TYPE][#data].size(); i++)\
    {\
        SCARA.data.append(QString::fromStdString(root[C_TYPE][#data][i].asString()));    \
    }
    DO_INSTANCE_STRUCT(PARSER_JSON, PARSER_JSON_ARRAY, scara_);
}

void ScaraConfigForm::saveDataToFile()
{

    Json::Value root;
    root["type"] = C_TYPE;
    #define SAVE_TO_JSON(SCARA, data) root[C_TYPE][#data] = SCARA.data.toStdString();
    #define SAVE_TO_JSON_ARRAY(SCARA, data)\
    for(int i = 0; i < (int)SCARA.data.size(); i++)\
    {\
         root [C_TYPE][#data][i] = SCARA.data.at(i).toStdString();\
    }
    DO_INSTANCE_STRUCT(SAVE_TO_JSON, SAVE_TO_JSON_ARRAY, scara_);
    GlobalFunction::WriteJsonToJsonfile(root, filePath_);
}

QByteArray ScaraConfigForm::calAllUiData()
{
    QByteArray array;
    QDataStream out(&array, QIODevice::WriteOnly);
    out << ui->comboBox_axisSum->currentIndex() << ui->comboBox_intanceId->currentIndex();
    for(QMap<QString, QWidget*>::iterator it = uiMap_.begin(); it != uiMap_.end(); it++)
    {
        if(qobject_cast<QLineEdit*>(it.value()))
        {
            out << qobject_cast<QLineEdit*>(it.value())->text();
        }
        else if(qobject_cast<QComboBox*>(it.value()))
        {
            out << qobject_cast<QComboBox*>(it.value())->currentText();
        }
    }
    auto pushAllTableData = [&out](QTableWidget* table)
    {
        for(int row = 0; row < table->rowCount(); row++)
        {
            for(int column = 0; column < table->columnCount(); column++)
            {
                out << table->item(row, column)->text();
            }
        }
    };
    pushAllTableData(ui->tableWidget_common);
    pushAllTableData(ui->tableWidget_couple);
    pushAllTableData(ui->tableWidget_withAxis);
    return QCryptographicHash::hash(array, QCryptographicHash::Md5);
}

void ScaraConfigForm::getDefaultDocData(QByteArray& docData)
{
    Scara scara;
    Json::Value root;
    DO_INSTANCE_STRUCT(SAVE_TO_JSON, SAVE_TO_JSON_ARRAY, scara)
    GlobalFunction::WriteJsonToQByteArray(root, docData);
}

