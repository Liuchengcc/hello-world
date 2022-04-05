#ifndef PROGRAMFILE_H
#define PROGRAMFILE_H

#include <QObject>
#include <marcroly.h>
#include "programsctrl.h"
#include <program.h>

#define OP_PROGRAM_FILE_ALL_OBJ(OP_MARCRO)\
    OP_MARCRO(QString, filePath_)

class ProgramFile : public QObject
{
    Q_OBJECT
public:
#define FILE_TYPE_NAME "文件 点位 工具 用户 日志 配置 实例 其他"
#define FILE_TYPE_NAME_SPLIT_TOKEN " "
    enum FileType
    {
        JobFile = 0,
        PointFile,
        ToolFile,
        UserFile,
        LogFile,
        ConfigFile,
        InstanceFile,
        OtherFile,
        FileTypeSum
    };
    explicit ProgramFile(QString path, QObject* parent);
    ProgramFile(QString path, FileType type, QObject* parent);
    void saveToFile();
    void openFileToContent();
    QString getFileName();
    FileType getFileType();
    QString* getFileContent();
    static QMap<FileType, QString> getTypeName();
signals:
    OP_PROGRAM_FILE_ALL_OBJ(DECL_SIGNAL_METADATA);
    OP_PROGRAM_FILE_ALL_OBJ(REGISTER_METADATA);

private:
    FileType fileType_;
    QString name_;
    QString content_;
    static QMap<FileType, QString> fileTypeNameMap_;

};

#endif // PROGRAMFILE_H
