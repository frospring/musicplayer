// zkw2024051604060
#include "lyricsparser.h"

#include <QFile>
#include <QTextStream>
#include <QFileInfo>

QVector<LyricEntry> LyricsParser::parseLrcFile(const QString &path)
{
    QVector<LyricEntry> entries;

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return entries;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        // LRC 格式: [mm:ss.xx]lyrics text
        if (line.startsWith('[')) {
            int closeIdx = line.indexOf(']');
            if (closeIdx < 0) continue;

            QString timeStr = line.mid(1, closeIdx - 1);
            QString text = line.mid(closeIdx + 1).trimmed();

            int colon = timeStr.indexOf(':');
            if (colon > 0) {
                int minutes = timeStr.left(colon).toInt();
                double seconds = timeStr.mid(colon + 1).toDouble();
                LyricEntry entry;
                entry.timestampMs = static_cast<qint64>(minutes * 60000 + seconds * 1000);
                entry.text = text;
                entries.append(entry);
            }
        }
    }

    file.close();
    return entries;
}

QVector<LyricEntry> LyricsParser::parseVttFile(const QString &path)
{
    QVector<LyricEntry> entries;

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return entries;

    // 辅助Lambda: 解析hh:mm:ss.xxx或mm:ss.xxx → ms
    auto parseTimestamp = [](const QString &s) -> qint64 {
        QString t = s.trimmed();
        int c1 = t.indexOf(':');
        int c2 = t.lastIndexOf(':');

        int hours = 0, minutes = 0;
        double seconds = 0;
        if (c1 == c2) {
            minutes = t.left(c1).toInt();
            seconds = t.mid(c1 + 1).toDouble();
        } else {
            hours = t.left(c1).toInt();
            minutes = t.mid(c1 + 1, c2 - c1 - 1).toInt();
            seconds = t.mid(c2 + 1).toDouble();
        }
        return static_cast<qint64>(hours * 3600000 + minutes * 60000 + seconds * 1000);
    };

    QTextStream in(&file);
    bool pastHeader = false;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();

        // 跳过WEBVTT头部
        if (!pastHeader) {
            if (line.contains("-->")) pastHeader = true;
            else continue;
        }

        if (line.isEmpty()) continue;

        if (line.contains("-->")) {
            QStringList parts = line.split("-->");
            if (parts.size() < 1) continue;

            qint64 startMs = parseTimestamp(parts[0]);
            if (in.atEnd()) break;

            QString text = in.readLine().trimmed();
            LyricEntry entry;
            entry.timestampMs = startMs;
            entry.text = text;
            entries.append(entry);
        }
    }

    file.close();
    return entries;
}

QVector<LyricEntry> LyricsParser::parseText(const QString &text)
{
    QVector<LyricEntry> entries;
    QStringList lines = text.split('\n');
    for (const auto &line : lines) {
        QString trimmed = line.trimmed();
        if (trimmed.isEmpty()) continue;
        LyricEntry entry;
        entry.timestampMs = -1; // 不同步标记
        entry.text = trimmed;
        entries.append(entry);
    }
    return entries;
}
