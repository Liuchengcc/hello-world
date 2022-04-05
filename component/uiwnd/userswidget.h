#ifndef UsersWidget_H
#define UsersWidget_H

#include <QWidget>
#include <basewidget.h>
#include <controller.h>

#define OP_USERS_ALL_OBJ(OP_MARCRO)\
    OP_MARCRO(QString, filePath_)\
    OP_MARCRO(QList<CoordValue>, coordValueList_)

namespace Ui {
class UsersWidget;
}

class UsersWidget : public BaseWidget
{
    Q_OBJECT

public:
    explicit UsersWidget(QWidget *parent = nullptr);
    UsersWidget(QString filePath, QWidget* parent = nullptr);
    ~UsersWidget();
    void initDock() override;
    void updateUi();

signals:
    OP_USERS_ALL_OBJ(DECL_SIGNAL_METADATA);
    OP_USERS_ALL_OBJ(REGISTER_METADATA);

private:
    Ui::UsersWidget *ui;
};

#endif // UsersWidget_H
