#ifndef LYRICPARSER_H
#define LYRICPARSER_H

#include <QString>
#include <QVector>

// 歌词行数据结构
struct LyricLine {
    qint64 timeMs;   // 时间戳 (毫秒)
    QString text;    // 歌词文本
};

// 歌词解析器 - 静态工具类，无状态、无 UI 依赖
// 支持 LRC 和 WebVTT 两种格式
class LyricParser
{
public:
    // 解析 LRC 格式: [mm:ss.xx]歌词文本
    static QVector<LyricLine> parseLrc(const QString &filePath);

    // 解析 WebVTT 格式: [HH:]MM:SS.mmm --> [HH:]MM:SS.mmm
    static QVector<LyricLine> parseVtt(const QString &filePath);
};

#endif
