#include "pointsform.h"
#include "ui_pointsform.h"
#include "globalfunction.h"
#include "DockManager.h"
#include "json/value.h"
#include "programsctrl.h"
#include "program.h"

const QString c_dockHeadName("点位信息");

#define POINTS_HORIZON_HEADERS "名称 描述 RobotID X Y Z U V W R S T"
PointsForm::PointsForm(QString filePath, QWidget *parent):
    BaseWidget(parent),
    filePath_(filePath),
    ui(new Ui::PointsForm)
{
    ui->setupUi(this);
   initDock();
    updateUi();
   // pg
    QStringList pathList = filePath_.split("/");
    pathList.pop_back();
    QString programName = pathList.last();
    Program *curPg = ProgramsCtrl::getInstance()->getProgram(programName);
    connect(curPg, &Program::change_pointsList_, [this]{updateUi();});
}

PointsForm::~PointsForm()
{
    delete ui;
}

void PointsForm::updateUi()
{
    ui->pointsTableWidget->setColumnCount(tr(POINTS_HORIZON_HEADERS).split(" ").size());
    ui->pointsTableWidget->setHorizontalHeaderLabels(tr(POINTS_HORIZON_HEADERS).split(" "));
    QStringList pathList = filePath_.split("/");
    pathList.pop_back();
    QString programName = pathList.last();
    pointsList_ = ProgramsCtrl::getInstance()->getProgram(programName)->get_pointsList_();
    ui->pointsTableWidget->setRowCount(pointsList_.size());
    for(int i = 0; i < pointsList_.size(); i++)
    {
        ui->pointsTableWidget->setItem(i,0, new QTableWidgetItem(pointsList_.at(i).name));
        ui->pointsTableWidget->setItem(i,1, new QTableWidgetItem(pointsList_.at(i).description));
        ui->pointsTableWidget->setItem(i,2, new QTableWidgetItem(pointsList_.at(i).robotID));
        ui->pointsTableWidget->setItem(i,3, new QTableWidgetItem(pointsList_.at(i).X));
        ui->pointsTableWidget->setItem(i,4, new QTableWidgetItem(pointsList_.at(i).Y));
        ui->pointsTableWidget->setItem(i,5, new QTableWidgetItem(pointsList_.at(i).Z));
        ui->pointsTableWidget->setItem(i,6, new QTableWidgetItem(pointsList_.at(i).U));
        ui->pointsTableWidget->setItem(i,7, new QTableWidgetItem(pointsList_.at(i).V));
        ui->pointsTableWidget->setItem(i,8, new QTableWidgetItem(pointsList_.at(i).W));
        ui->pointsTableWidget->setItem(i,9, new QTableWidgetItem(pointsList_.at(i).R));
        ui->pointsTableWidget->setItem(i,10, new QTableWidgetItem(pointsList_.at(i).S));
        ui->pointsTableWidget->setItem(i,11, new QTableWidgetItem(pointsList_.at(i).T));
    }
}

void PointsForm::initDock()
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
