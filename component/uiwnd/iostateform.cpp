#include "iostateform.h"
#include "ui_iostateform.h"
#include "marcroly.h"
#include "DockManager.h"
#include "controller.h"

const QString c_dockHeadName("IO状态");

INSTANCE_MODE(IOStateForm, instance_);
#define IO_ON_STYLE "QPushButton{background-color:mediumseagreen;}QPushButton:hover{background-color:seagreen;}"
#define IO_OFF_STYLE "QPushButton{background-color:orange;}QPushButton:hover{background-color:tomato;}"
#define IO_UNKNOWN_STYLE "QPushButton{background-color:grey;}QPushButton:hover{background-color:dimgray;}"
#define IO_SUM 256
#define IO_ROW_SUM 16
#define IO_COLUMN_SUM  (IO_SUM/IO_ROW_SUM)
#define TIMEOUT_UPDATEDATA 300
#define UPDATE_INPUT_STATUS(INDEX) inputBtnList_.at(INDEX)->setStyleSheet(inputStateList_.at(INDEX)?IO_ON_STYLE:IO_OFF_STYLE);
#define UPDATE_OUTPUT_STATUS(INDEX) outputBtnList_.at(INDEX)->setStyleSheet(outputStateList_.at(INDEX)?IO_ON_STYLE:IO_OFF_STYLE);

IOStateForm::IOStateForm(QWidget *parent) :
    BaseWidget(parent),
    ui(new Ui::IOStateForm)
{
    ui->setupUi(this);
    initDock();
    initUi();
    startTimer(TIMEOUT_UPDATEDATA);
}

IOStateForm::~IOStateForm()
{
    delete ui;
}

void IOStateForm::initDock()
{
    area_ = ads::CenterDockWidgetArea;
    dock_ = new ads::CDockWidget(c_dockHeadName);
    dock_->setWidget(this);
    dock_->setFeatures(ads::CDockWidget::AllDockWidgetFeatures);
    connect(dock_, &ads::CDockWidget::closeRequested, [this]()
    {
        qDebug()<<"dock requested" ;
        dock_->dockManager()->removeDockWidget(dock_);
    });
    connect(dock_, &ads::CDockWidget::closed, [](){qDebug()<<"dock is closed";});
    connect(dock_, &ads::CDockWidget::visibilityChanged, [](bool status){qDebug()<<"dock is visibility" << status;});
}

void IOStateForm::initUi()
{
    realSum_ = IO_SUM;
    for(int i = 0; i < IO_SUM; i++)
    {
         QPushButton* inputBtn = new QPushButton(QString::number(i), this);
         inputBtn->setStyleSheet(IO_UNKNOWN_STYLE);
         inputBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
         inputBtnList_.append(inputBtn);
         inputStateList_.append(false);
         QPushButton* outputBtn = new QPushButton(QString::number(i), this);
         outputBtn->setStyleSheet(IO_UNKNOWN_STYLE);
         outputBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
         outputBtnList_.append(outputBtn);
         outputStateList_.append(false);
         connect(outputBtn, &QPushButton::clicked, [this, i]
         {
             Controller::getInstance()->buildPacketSend(FUNC_TEACH, TEACH_SET_OUT, tr("%1;%2;,").arg(i).arg((int)(!outputStateList_[i])));
         });
    }
    int index = 0;
    for(int i = 0; i < IO_COLUMN_SUM; i++)
    {
        for(int j = 0; j < IO_ROW_SUM; j++)
        {
            ui->gridLayout_input->addWidget(inputBtnList_.at(index), j, i);
            ui->gridLayout_output->addWidget(outputBtnList_.at(index), j, i);
            index++;
        }
    }
    connect(Controller::getInstance(), &Controller::change_networkState_, [this](Controller::NetWorkState state)
    {
        if(state != Controller::net_connected)
        {
            for(int i = 0; i < IO_SUM; i++)
            {
                inputBtnList_.at(i)->setStyleSheet(IO_UNKNOWN_STYLE);
                outputBtnList_.at(i)->setStyleSheet(IO_UNKNOWN_STYLE);
            }
        }
        else
        {
            for(int i = 0; i < IO_SUM; i++)
            {

                UPDATE_INPUT_STATUS(i);
                UPDATE_OUTPUT_STATUS(i);
            }
        }
    });
    connect(Controller::getInstance(), &Controller::recvIOState, [this](QList<bool> &inputList, QList<bool>& outputList)
    {
        if(inputList.size() > IO_SUM){ERROR_OCCUR}
        if(realSum_ != inputList.size())
        {
            realSum_ = inputList.size();
            for(int i = 0; i < inputBtnList_.size(); i++)
            {
                inputBtnList_.at(i)->hide();
                outputBtnList_.at(i)->hide();
            }
            for(int i = 0; i < inputList.size(); i++)
            {
                inputBtnList_.at(i)->show();
                outputBtnList_.at(i)->show();
            }
        }
        int size = inputList.size();
        for(int i = 0; i < size; i++)
        {
            if(inputList.at(i) != inputStateList_.at(i))
            {
                inputStateList_[i] = inputList.at(i);
                UPDATE_INPUT_STATUS(i);
            }
            if(outputList.at(i) != outputStateList_.at(i))
            {
                outputStateList_[i] = outputList.at(i);
                UPDATE_OUTPUT_STATUS(i);
            }
        }
    });
}

void IOStateForm::timerEvent(QTimerEvent *event)
{
 Q_UNUSED(event)
    if(!isVisible())return;
    if(Controller::getInstance()->get_networkState_() == Controller::net_connected)
    {
       Controller::getInstance()->buildPacketSend(FUNC_SYS, SYS_GET_IO_STATE, tr(""));
    }
}

