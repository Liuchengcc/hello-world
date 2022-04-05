#ifndef IOSTATEFORM_H
#define IOSTATEFORM_H

#include <QWidget>
#include <basewidget.h>
#include <QPushButton>

namespace Ui {
class IOStateForm;
}

class IOStateForm : public BaseWidget
{
    Q_OBJECT

public:
    static IOStateForm* getInstance();
    explicit IOStateForm(QWidget *parent = nullptr);
    ~IOStateForm();
    void initDock() override;
    void initUi();

protected:
    void timerEvent(QTimerEvent *event) override;
private:
    static IOStateForm* instance_;
    Ui::IOStateForm *ui;
    QList<QPushButton*> inputBtnList_;
    QList<bool> inputStateList_;
    QList<QPushButton*> outputBtnList_;
    QList<bool> outputStateList_;
    int realSum_;
};

#endif // IOSTATEFORM_H
