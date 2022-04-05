#ifndef BASEWIDGET_H
#define BASEWIDGET_H

#include <QWidget>
#include <DockWidget.h>

class BaseWidget: public QWidget
{
    Q_OBJECT
public:
    ads::CDockWidget *getDock();
    ads::DockWidgetArea getArea();
    ads::CDockWidget *dock_;
    ads::DockWidgetArea area_;
    explicit BaseWidget(QWidget *parent = nullptr);
    virtual void initDock();
    ~BaseWidget();
};

#endif // BASEWIDGET_H
