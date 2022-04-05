#ifndef LOGOUTFORM_H
#define LOGOUTFORM_H


#include <QWidget>
#include <basewidget.h>

namespace Ui {
class LogoutForm;
}

class LogoutForm : public BaseWidget
{
    Q_OBJECT

public:
    explicit LogoutForm(QWidget *parent = nullptr);
    static LogoutForm* getInstance();
    ~LogoutForm();
    void initDock() override;
    void showLogMsg(QString);

private:
    static LogoutForm* instance_;
    Ui::LogoutForm *ui;
};

#endif // LOGOUTFORM_H
