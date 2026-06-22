#include "filemanager.h"
#include <QFileInfo>
#include <QDir>

FileManager::FileManager(QObject *parent)
    : QObject(parent)
{
}

QString FileManager::findLyricFile(const QString &audioFilePath)
{
    QFileInfo info(audioFilePath);
    QString base = info.completeBaseName();
    QString dir  = info.absolutePath();

    // 1. 优先查找同名 .lrc
    QString lrcPath = dir + "/" + base + ".lrc";
    if (QFileInfo::exists(lrcPath))
        return lrcPath;

    // 2. 回退查找 .vtt
    QDir qdir(dir);
    QStringList vtts = qdir.entryList({base + "*.vtt"}, QDir::Files);
    if (!vtts.isEmpty())
        return dir + "/" + vtts.first();

    return {};
}
