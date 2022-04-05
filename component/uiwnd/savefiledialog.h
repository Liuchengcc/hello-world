#ifndef SAVE_FILE_DIALOG_H
#define SAVE_FILE_DIALOG_H

#include <QDialog>
#include <qlineedit.h>

namespace Ui {
class SaveFileDialog;
}

class SaveFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SaveFileDialog(const QString& header, const QString& fileName, QWidget *parent = nullptr);
    void setDiffData(QByteArray origin, QByteArray modified);
    ~SaveFileDialog();
    QStringList getDialogData();

private slots:

    void on_pushButton_ok_clicked();

    void on_pushButton_cancel_clicked();

    void on_pushButton_checkModify_clicked();

private:
    Ui::SaveFileDialog *ui;
    QList<QLineEdit*> lineditList_;
    QByteArray originData_;
    QByteArray modifiedData_;
};

#endif // SaveFileDialog_H
