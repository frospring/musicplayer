#ifndef LYRICPARSER_H
#define LYRICPARSER_H

#include <QString>
#include <QVector>

struct LyricLine {
    qint64 timeMs;
    QString text;
};

class LyricParser
{
public:
    static QVector<LyricLine> parseLrc(const QString &filePath);
    static QVector<LyricLine> parseVtt(const QString &filePath);
};

#endif
