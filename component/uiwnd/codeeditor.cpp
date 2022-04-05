#include "ui_codeeditor.h"
#include "codeeditor.h"
#include "DockManager.h"
#include "globalfunction.h"
#include "qnamespace.h"
#include "qsciscintilla.h"
#include "qscilexerlua.h"
#include "programsctrl.h"
#include "qmessagebox.h"
#include "qsciapis.h"
#include "savefiledialog.h"
#include "qaction.h"

#define LH_API_PATH ":/res/lhapis.api"

CodeEditor::CodeEditor(QString filePath, QWidget *parent) :
    BaseWidget(parent),
    filePath_(filePath),
    ui(new Ui::CodeEditor)
{
    ui->setupUi(this);
    editor_ = new QsciScintilla;
    fileName_ = filePath.split("/").last();
    initDock();
    initEditor();
    ui->gridLayout->addWidget(editor_);
}

CodeEditor::~CodeEditor()
{
    if(editor_)
    {
        delete editor_;
        editor_ = 0;
    }
    delete ui;
}

void CodeEditor::initEditor()
{

    if(filePath_.isEmpty())
    {
        return;
    }

    //自动补全
    QsciLexerLualh*lexer = new QsciLexerLualh;
    lexer->setColor(QColor(Qt::magenta), QsciLexerLua::Keyword);
    lexer->setColor(QColor(Qt::red), QsciLexerLua::BasicFunctions);
    lexer->setColor(QColor(Qt::darkBlue), QsciLexerLua::KeywordSet5);
    editor_->setLexer(lexer);

    QsciAPIs *apis = new QsciAPIs(lexer);
    apis->load(LH_API_PATH);
    apis->prepare();
    editor_->setAutoCompletionSource(QsciScintilla::AcsAll);
    editor_->setAutoCompletionCaseSensitivity(true);
    editor_->setAutoCompletionReplaceWord(true);
    editor_->setAutoCompletionThreshold(1);
    //tab 设定
    editor_->setIndentationsUseTabs(false); //空格代替/t
    editor_->setTabWidth(4);
    editor_->setIndentationGuides(true);
    editor_->setIndentationWidth(0);
    editor_->setAutoIndent(true);
    editor_->setTabIndents(true);//空格补齐tab
    editor_->setBackspaceUnindents(true);
    //行号显示
    editor_->setMarginType(1, QsciScintilla::NumberMargin);
    editor_->setMarginLineNumbers(1, true);
    editor_->setMarginWidth(1, 30);
    //断点
    editor_->setMarginType(0, QsciScintilla::SymbolMargin);
    editor_->setMarginLineNumbers(0, false);
    editor_->setMarginWidth(0, 15);
    editor_->setMarginSensitivity(0, true);
    editor_->setMarginMarkerMask(0, 0x02);
    editor_->setMarginsBackgroundColor(QColor("#bbfaae"));
     //自动缩进
    editor_->setAutoIndent(true);
    //括号匹配
    editor_->setBraceMatching(QsciScintilla::SloppyBraceMatch);
    //当前行高亮
    editor_->setCaretLineVisible(true);
    editor_->setCaretLineBackgroundColor(QColor(255, 250, 155));
    //函数提示
    editor_->setCallTipsVisible(true);
    editor_->setCallTipsStyle(QsciScintilla::CallTipsContext);
    editor_->setCallTipsPosition(QsciScintilla::CallTipsBelowText);
    //加载
    QByteArray docData;
    GlobalFunction::ReadDocFileToByteArray(filePath_, docData);
    set_fileData_(docData);
    editor_->setText(docData);
    //设置快捷键 ctrl+s
    QAction *saveAction = new QAction(this);
    saveAction->setShortcut(tr("ctrl+s"));
    this->addAction(saveAction);
    connect(saveAction, &QAction::triggered, [this]{saveFile();});
    connect(editor_, &QsciScintilla::modificationChanged, [this](bool status)
    {
        QString header;
        if(status){header = tr("*") + fileName_;}
        else{header = fileName_;}
        if(header != dock_->windowTitle()){dock_->setWindowTitle(header);}
    });
    connect(editor_, &QsciScintilla::linesChanged, []{qDebug()<< "line change !";});
    connect(editor_, &QsciScintilla::marginClicked, [this](int margin, int line, Qt::KeyboardModifiers state)
    {
        Q_UNUSED(margin);
        Q_UNUSED(state);
        if (editor_->markersAtLine(line) != 0)
        {
            editor_->markerDelete(line, 0);
        }
        else
        {
            editor_->markerAdd(line, 0);
        }
    });
    editor_->markerDefine(QsciScintilla::Circle, 0);
    editor_->setMarkerBackgroundColor(QColor("#ee1111"), 0);

    //代码折叠
    editor_->setMarginType(2, QsciScintilla::SymbolMargin);
    editor_->setMarginLineNumbers(2, false);
    editor_->setMarginWidth(2, 15);
    editor_->setMarginSensitivity(2, true);
    editor_->setMarginsBackgroundColor(QColor("#bbfaae"));
    editor_->setFolding(QsciScintilla::CircledTreeFoldStyle, 2);
    editor_->setFoldMarginColors(Qt::gray, Qt::lightGray);

    editor_->setModified(false);
}

void CodeEditor::saveFile()
{
    if(editor_->isModified())
    {
        SaveFileDialog dia(tr("提示"), fileName_, this);
        dia.setDiffData(fileData_, editor_->text().toUtf8());
        dia.setModal(false);
        dia.show();
        if(QDialog::Accepted == dia.exec())
        {
            QByteArray fileData = editor_->text().toUtf8();
            editor_->setText(fileData);
            GlobalFunction::WriteByteArrayToDocFile(get_filePath_(), fileData);
            GlobalFunction::showLogMsg(tr("文件 %1 已保存！").arg(fileName_));
            editor_->setModified(false);
        }
        else
        {
            return;
        }
    }
}

void CodeEditor::copy()
{
    editor_->copy();
}

void CodeEditor::paste()
{
    editor_->paste();
}

void CodeEditor::cut()
{
    editor_->cut();
}

void CodeEditor::undo()
{
    editor_->undo();
}

void CodeEditor::redo()
{
   editor_->redo();
}

void CodeEditor::marginIsClicked(int margin, int line, Qt::KeyboardModifiers state)
{
    Q_UNUSED(margin)
    Q_UNUSED(line)
    Q_UNUSED(state);
}

void CodeEditor::initDock()
{
    area_ = ads::CenterDockWidgetArea;
    dock_ = new ads::CDockWidget(fileName_);
    dock_->setWidget(this);
    dock_->setFeatures(ads::CDockWidget::AllDockWidgetFeatures);
    dock_->setFeature(ads::CDockWidget::DockWidgetDeleteOnClose,true);
    dock_->setFeature(ads::CDockWidget::DockWidgetForceCloseWithArea,true);
    connect(dock_, &ads::CDockWidget::closeRequested, [this]()
    {
        if(editor_->isModified())
        {
          if(QMessageBox::Cancel == QMessageBox::information(this, tr("提示"), tr("文件 %1 尚未保存，是否强制退出？").arg(fileName_), QMessageBox::Ok|QMessageBox::Cancel))
          {
              return;
          }
        }
        qDebug()<<"dock requested" ;
        dock_->closeDockWidget();
    });
    connect(dock_, &ads::CDockWidget::closed, []()
    {
        qDebug()<<"dock is closed";
    });
    connect(dock_, &ads::CDockWidget::visibilityChanged, [](bool status)
    {
        qDebug()<<"dock is visibility" << status;
    });
}

const char *QsciLexerLualh::keywords(int set) const
{
    if(set == 5)
    {
        static QString apistr;
        if(apistr.isEmpty())
        {
            QByteArray apiData;
            GlobalFunction::ReadDocFileToByteArray(LH_API_PATH, apiData);
            QStringList apiList = QString().append(apiData).split("\n");
            foreach(QString api, apiList)
            {
                apistr += api.split("(").at(0);
                apistr += " ";
            }
        }
        return apistr.toStdString().c_str();
    }
    return QsciLexerLua::keywords(set);
}
