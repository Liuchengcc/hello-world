#ifndef CREATEPRJDIALOG_H
#define CREATEPRJDIALOG_H

#include <QDialog>
#include <marcroly.h>

#define OP_CREATE_PRJ_ALL_OBJ(OP_MARCRO)\
    OP_MARCRO(QString, path_);\
    OP_MARCRO(QString, name_);

namespace Ui {
class CreatePrjDialog;
}

class CreatePrjDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreatePrjDialog(QWidget *parent = nullptr);
    ~CreatePrjDialog();
signals:
OP_CREATE_PRJ_ALL_OBJ(DECL_SIGNAL_METADATA);
OP_CREATE_PRJ_ALL_OBJ(REGISTER_METADATA);
private:
    Ui::CreatePrjDialog *ui;
};

#endif // CREATEPRJDIALOG_H
