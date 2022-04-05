#ifndef COMMONDIALOG_H
#define COMMONDIALOG_H

#include <QDialog>
#include <qlineedit.h>

namespace Ui {
class CommonDialog;
}

class CommonDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CommonDialog(const QString& header,const QStringList& keyList, QWidget *parent = nullptr);
    ~CommonDialog();
    QStringList getDialogData();

private slots:

    void on_pushButton_ok_clicked();

    void on_pushButton_cancel_clicked();

private:
    Ui::CommonDialog *ui;
    QList<QLineEdit*> lineditList_;
};

#endif // COMMONDIALOG_H
