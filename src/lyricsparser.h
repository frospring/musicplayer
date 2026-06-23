// zkw2024051604060
#pragma once

#include <QString>
#include <QVector>

struct LyricEntry {
    qint64 timestampMs; // -1 表示不同步歌词(内嵌文本)
    QString text;
};

class LyricsParser
{
public:
    static QVector<LyricEntry> parseLrcFile(const QString &path);
    static QVector<LyricEntry> parseVttFile(const QString &path);
    static QVector<LyricEntry> parseText(const QString &text);
};
