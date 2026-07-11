#include "lyricparser.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>

QVector<LyricLine> LyricParser::parseLrc(const QString &filePath)
{
    QVector<LyricLine> lines;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return lines;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        QRegularExpression re(R"(\[(\d+):(\d+(?:\.\d+)?)\](.*))");
        auto match = re.match(line);
        if (match.hasMatch()) {
            int minutes = match.captured(1).toInt();
            double seconds = match.captured(2).toDouble();
            QString text = match.captured(3).trimmed();
            qint64 timeMs = static_cast<qint64>((minutes * 60.0 + seconds) * 1000.0);
            lines.append({timeMs, text.isEmpty() ? QString("...") : text});
        }
    }
    return lines;
}

QVector<LyricLine> LyricParser::parseVtt(const QString &filePath)
{
    QVector<LyricLine> lines;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return lines;

    QTextStream in(&file);
    QStringList rawLines = in.readAll().split('\n');

    for (int i = 0; i < rawLines.size(); ++i) {
        QString line = rawLines[i].trimmed();
        if (line == "WEBVTT" || line.isEmpty() || line.startsWith("NOTE"))
            continue;

        QRegularExpression re(
            R"((?:(\d+):)?(\d+):(\d+)\.(\d+)\s*-->\s*(?:(\d+):)?(\d+):(\d+)\.(\d+))");
        auto match = re.match(line);
        if (match.hasMatch()) {
            int h = match.captured(1).isEmpty() ? 0 : match.captured(1).toInt();
            int m = match.captured(2).toInt();
            int s = match.captured(3).toInt();
            int ms = match.captured(4).toInt();

            int msWidth = match.captured(4).length();
            for (int k = msWidth; k < 3; ++k)
                ms *= 10;

            qint64 timeMs = static_cast<qint64>(h * 3600000LL + m * 60000 + s * 1000 + ms);

            int j = i + 1;
            while (j < rawLines.size() && rawLines[j].trimmed().isEmpty())
                j++;
            QString text = (j < rawLines.size()) ? rawLines[j].trimmed() : QString();
            lines.append({timeMs, text.isEmpty() ? QString("...") : text});
            i = j;
        }
    }
    return lines;
}
