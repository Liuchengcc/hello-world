#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QWidget>
#include <basewidget.h>
#include <qsciscintilla.h>
#include <marcroly.h>
#include <qscilexerlua.h>

#define OP_CODEEDITOR_ALL_OBJ(OP_MARCRO)\
    OP_MARCRO(QString, fileName_)\
    OP_MARCRO(QString, filePath_)\
    OP_MARCRO(QByteArray, fileData_)

namespace Ui {
class CodeEditor;
}

class CodeEditor : public BaseWidget
{
    Q_OBJECT

public:
    CodeEditor(QString filePath, QWidget *parent = nullptr);
    ~CodeEditor();
    void initEditor();
    void saveFile();
    void copy();
    void paste();
    void cut();
    void undo();
    void redo();

protected:
    void initDock() override;
public slots:
    void marginIsClicked(int, int, Qt::KeyboardModifiers);

signals:
    OP_CODEEDITOR_ALL_OBJ(DECL_SIGNAL_METADATA);
    OP_CODEEDITOR_ALL_OBJ(REGISTER_METADATA);
private:
    Ui::CodeEditor *ui;
    QsciScintilla* editor_;
};

class QsciLexerLualh: public QsciLexerLua
{
public:
    const char * keywords(int set) const override;
};


#endif // CODEEDITOR_H
