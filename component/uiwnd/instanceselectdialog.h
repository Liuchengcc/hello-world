#ifndef INSTANCESELECTDIALOG_H
#define INSTANCESELECTDIALOG_H

#include <QDialog>

namespace Ui {
class InstanceSelectDialog;
}

class InstanceSelectDialog : public QDialog
{
    Q_OBJECT

public:
    enum InstanceType{scara, puma, module};
    explicit InstanceSelectDialog(QWidget *parent = nullptr);
    ~InstanceSelectDialog();
    QString getFileName();
    InstanceType getInstanceType();

private slots:
    void on_pushButton_ok_clicked();

    void on_pushButton_cancel_clicked();

private:
    QString fileName_;
    InstanceType type_;

    Ui::InstanceSelectDialog *ui;
};

#endif // INSTANCESELECTDIALOG_H
