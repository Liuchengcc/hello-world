#ifndef TASKMONITOR_H
#define TASKMONITOR_H

#include <QWidget>
#include <basewidget.h>

namespace Ui {
class TaskMonitor;
}

class TaskMonitor : public BaseWidget
{
    Q_OBJECT

public:
    static TaskMonitor* getInstance();
    explicit TaskMonitor(QWidget *parent = nullptr);
    ~TaskMonitor();
    void initDock() override;
    void initUi();

private:
    static TaskMonitor* instance_;

private:
    Ui::TaskMonitor *ui;
};

#endif // TASKMONITOR_H
