#include "basewidget.h"
#include "DockManager.h"
ads::CDockWidget *BaseWidget::getDock()
{
    return dock_;
}

ads::DockWidgetArea BaseWidget::getArea()
{
    return area_;
}

BaseWidget::BaseWidget(QWidget *parent):QWidget(parent)
{
    area_ = ads::DockWidgetArea::CenterDockWidgetArea;
    dock_ = new ads::CDockWidget("default name");
}

void BaseWidget::initDock()
{
}

BaseWidget::~BaseWidget()
{

}
