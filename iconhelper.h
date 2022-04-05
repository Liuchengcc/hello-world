#ifndef ICONHELPER_H
#define ICONHELPER_H

#include <QObject>
#include <QMutex>
#include <qfont.h>

class QLabel;
class QAbstractButton;
class QAction;
class QWidgetAction;
class QPushButton;

class IconHelper : public QObject
{
    Q_OBJECT

public:
    explicit IconHelper(QObject *parent = 0);
    static IconHelper *Instance()
    {
        static QMutex mutex;

        if (!self) {
            QMutexLocker locker(&mutex);

            if (!self) {
                self = new IconHelper;
            }
        }

        return self;
    }
    void setIcon(QLabel *lab, QChar c, quint32 size = 9);
    void setIcon(QAbstractButton *btn, QChar c, quint32 size = 9);
    void setIcon(QAction* action, QChar c, quint32 size = 9);
    void setIcon(QWidgetAction* action, QChar c, quint32 size = 9);
    void setIcon(QPushButton* btn, QChar c, quint32 size = 9);
    QPixmap getPixmap(const QString &color, QChar c, quint32 size = 9,
                      quint32 pixWidth = 10, quint32 pixHeight = 10);

private:
    static IconHelper *self;    //对象自身
    QFont iconFont;             //图形字体
};



#endif // ICONHELPER_H
