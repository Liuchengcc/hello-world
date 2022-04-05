#include "userswidget.h"
#include "ui_userswidget.h"

const QString c_dockHeadName("用户坐标");
#define HORIZON_HEADERS "名称 注释 X Y Z U V W"

UsersWidget::UsersWidget(QWidget *parent) :
    BaseWidget(parent),
    ui(new Ui::UsersWidget)
{
    ui->setupUi(this);
    initDock();
}

UsersWidget::UsersWidget(QString filePath, QWidget *parent):
    BaseWidget(parent),
    filePath_(filePath),
    ui(new Ui::UsersWidget)
{
    ui->setupUi(this);
    ui->tableWidget_user->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    initDock();
    updateUi();
}

UsersWidget::~UsersWidget()
{
    delete ui;
}

void UsersWidget::initDock()
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

void UsersWidget::updateUi()
{
    ui->tableWidget_user->setColumnCount(tr(HORIZON_HEADERS).split(" ").size());
    ui->tableWidget_user->setHorizontalHeaderLabels(tr(HORIZON_HEADERS).split(" "));
    QStringList pathList = filePath_.split("/");
    pathList.pop_back();
    QString programName = pathList.last();
    coordValueList_ = ProgramsCtrl::getInstance()->getProgram(programName)->get_userCoordsList_();
    ui->tableWidget_user->setRowCount(coordValueList_.size());
    for(int i = 0; i < coordValueList_.size(); i++)
    {
        ui->tableWidget_user->setItem(i, 0 ,new QTableWidgetItem(coordValueList_.at(i).name));
        ui->tableWidget_user->setItem(i, 1 ,new QTableWidgetItem(coordValueList_.at(i).Des));
        ui->tableWidget_user->setItem(i, 2 ,new QTableWidgetItem(coordValueList_.at(i).X));
        ui->tableWidget_user->setItem(i, 3 ,new QTableWidgetItem(coordValueList_.at(i).Y));
        ui->tableWidget_user->setItem(i, 4 ,new QTableWidgetItem(coordValueList_.at(i).Z));
        ui->tableWidget_user->setItem(i, 5 ,new QTableWidgetItem(coordValueList_.at(i).U));
        ui->tableWidget_user->setItem(i, 6 ,new QTableWidgetItem(coordValueList_.at(i).V));
        ui->tableWidget_user->setItem(i, 7 ,new QTableWidgetItem(coordValueList_.at(i).W));
    }

}
