#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMutex>
#include <QLabel>
#include <qabstractbutton.h>
#include <windowsmanager.h>
#include <config.h>
#include <QWidgetAction>
#include <QPushButton>



class Controller;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    //样式枚举
    enum Style {
        Style_Silvery = 0,          //银色样式
        Style_Blue = 1,             //蓝色样式
        Style_LightBlue = 2,        //淡蓝色样式
        Style_DarkBlue = 3,         //深蓝色样式
        Style_Gray = 4,             //灰色样式
        Style_LightGray = 5,        //浅灰色样式
        Style_DarkGray = 6,         //深灰色样式
        Style_Black = 7,            //黑色样式
        Style_LightBlack = 8,       //浅黑色样式
        Style_DarkBlack = 9,        //深黑色样式
        Style_PSBlack = 10,         //PS黑色样式
        Style_FlatBlack = 11,       //黑色扁平样式
        Style_FlatWhite = 12        //白色扁平样式
    };
public:
    explicit MainWindow(QWidget *parent = 0);
    void setStyle(Style);
    void initMenu();
    void initToolBar();
    void initStatusBar();
    void initFolder();
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    //ui component
    QLabel* toolbar_netstate_;
    WindowsManager* wm_;
    Config* config_;
    Controller* controller_;


};

#endif // MAINWINDOW_H
