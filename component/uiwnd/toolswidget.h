#ifndef TOOLSWIDGET_H
#define TOOLSWIDGET_H

#include <QWidget>
#include <basewidget.h>
#include <controller.h>

#define OP_TOOLS_ALL_OBJ(OP_MARCRO)\
    OP_MARCRO(QString, filePath_)\
    OP_MARCRO(QList<CoordValue>, coordValueList_)

namespace Ui {
class ToolsWidget;
}

class ToolsWidget : public BaseWidget
{
    Q_OBJECT

public:
    explicit ToolsWidget(QWidget *parent = nullptr);
    ToolsWidget(QString filePath, QWidget* parent = nullptr);
    ~ToolsWidget();
    void initDock() override;
    void updateUi();

signals:
    OP_TOOLS_ALL_OBJ(DECL_SIGNAL_METADATA);
    OP_TOOLS_ALL_OBJ(REGISTER_METADATA);
private:
    Ui::ToolsWidget *ui;

};

#endif // TOOLSWIDGET_H
