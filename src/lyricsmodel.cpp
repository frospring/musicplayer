// zkw2024051604060
#include "lyricsmodel.h"

#include <QFile>
#include <QFileInfo>
#include <algorithm>

LyricsModel::LyricsModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_currentIndex(-1)
{
}

int LyricsModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_entries.size();
}

QVariant LyricsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_entries.size())
        return {};

    switch (role) {
    case LyricTextRole:
        return m_entries[index.row()].text;
    case IsCurrentRole:
        return index.row() == m_currentIndex;
    case TimestampRole:
        return m_entries[index.row()].timestampMs;
    }
    return {};
}

QHash<int, QByteArray> LyricsModel::roleNames() const
{
    return {
        {LyricTextRole, "lyricText"},
        {IsCurrentRole, "isCurrent"},
        {TimestampRole, "timestampMs"},
    };
}

int LyricsModel::currentIndex() const { return m_currentIndex; }

int LyricsModel::count() const { return m_entries.size(); }

void LyricsModel::setCurrentIndex(int idx)
{
    if (idx == m_currentIndex) return;
    int oldIndex = m_currentIndex;
    m_currentIndex = idx;
    emit currentIndexChanged();

    if (idx >= 0 && idx < m_entries.size())
        emit dataChanged(index(idx), index(idx), QList<int>{IsCurrentRole});
    if (oldIndex >= 0 && oldIndex < m_entries.size())
        emit dataChanged(index(oldIndex), index(oldIndex), QList<int>{IsCurrentRole});
}

void LyricsModel::loadLyrics(const QUrl &audioUrl)
{
    beginResetModel();
    m_entries.clear();
    m_currentIndex = -1;

    QString path = audioUrl.toLocalFile();
    QFileInfo fi(path);
    QString base = fi.absolutePath() + "/" + fi.completeBaseName();

    QStringList candidates = {
        base + ".lrc",
        base + ".vtt",
        base + ".zh.vtt",
        base + ".zho.vtt",
    };

    for (const auto &candidate : candidates) {
        if (QFile::exists(candidate)) {
            if (candidate.endsWith(".lrc", Qt::CaseInsensitive))
                m_entries = LyricsParser::parseLrcFile(candidate);
            else
                m_entries = LyricsParser::parseVttFile(candidate);
            break;
        }
    }

    if (m_entries.isEmpty())
        m_entries = {LyricEntry{-1, "暂无歌词"}};

    endResetModel();
    emit countChanged();
}

void LyricsModel::updatePosition(qint64 positionMs)
{
    if (m_entries.isEmpty()) return;

    // 内嵌歌词无时间戳, 不定位
    if (m_entries[0].timestampMs < 0) return;

    // 二分查找: 找最后一个timestampMs <= positionMs的条目
    auto it = std::upper_bound(
        m_entries.begin(), m_entries.end(), positionMs,
        [](qint64 pos, const LyricEntry &entry) {
            return pos < entry.timestampMs;
        });

    int idx = static_cast<int>(std::distance(m_entries.begin(), it)) - 1;
    if (idx != m_currentIndex)
        setCurrentIndex(idx);
}
