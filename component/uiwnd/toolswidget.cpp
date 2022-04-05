#include "toolswidget.h"
#include "ui_toolswidget.h"

const QString c_dockHeadName("工具坐标");
#define HORIZON_HEADERS "名称 注释 X Y Z U V W"

ToolsWidget::ToolsWidget(QWidget *parent) :
    BaseWidget(parent),
    ui(new Ui::ToolsWidget)
{

    ui->setupUi(this);
    initDock();
}

ToolsWidget::ToolsWidget(QString filePath, QWidget *parent):
    BaseWidget(parent),
    filePath_(filePath),
    ui(new Ui::ToolsWidget)
{
    ui->setupUi(this);
    ui->tableWidget_tool->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    initDock();
    updateUi();

}

ToolsWidget::~ToolsWidget()
{
    delete ui;
}

void ToolsWidget::initDock()
{
    area_ = ads::CenterDockWidgetArea;
    dock_ = new ads::CDockWidget(c_dockHeadName);
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

void ToolsWidget::updateUi()
{
    ui->tableWidget_tool->setColumnCount(tr(HORIZON_HEADERS).split(" ").size());
    ui->tableWidget_tool->setHorizontalHeaderLabels(tr(HORIZON_HEADERS).split(" "));
    QStringList pathList = filePath_.split("/");
    pathList.pop_back();
    QString programName = pathList.last();
    coordValueList_ = ProgramsCtrl::getInstance()->getProgram(programName)->get_toolCoordsList_();
    ui->tableWidget_tool->setRowCount(coordValueList_.size());
    for(int i = 0; i < coordValueList_.size(); i++)
    {
        ui->tableWidget_tool->setItem(i, 0 ,new QTableWidgetItem(coordValueList_.at(i).name));
        ui->tableWidget_tool->setItem(i, 1 ,new QTableWidgetItem(coordValueList_.at(i).Des));
        ui->tableWidget_tool->setItem(i, 2 ,new QTableWidgetItem(coordValueList_.at(i).X));
        ui->tableWidget_tool->setItem(i, 3 ,new QTableWidgetItem(coordValueList_.at(i).Y));
        ui->tableWidget_tool->setItem(i, 4 ,new QTableWidgetItem(coordValueList_.at(i).Z));
        ui->tableWidget_tool->setItem(i, 5 ,new QTableWidgetItem(coordValueList_.at(i).U));
        ui->tableWidget_tool->setItem(i, 6 ,new QTableWidgetItem(coordValueList_.at(i).V));
        ui->tableWidget_tool->setItem(i, 7 ,new QTableWidgetItem(coordValueList_.at(i).W));
    }
}
