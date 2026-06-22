#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QString>

// 文件管理器 - 歌词文件查找等文件系统操作
class FileManager : public QObject
{
    Q_OBJECT

public:
    explicit FileManager(QObject *parent = nullptr);

    // 根据音频文件路径查找对应的歌词文件
    // 依次查找: 同名 .lrc -> 同名 .vtt -> 空字符串
    Q_INVOKABLE static QString findLyricFile(const QString &audioFilePath);
};

#endif
