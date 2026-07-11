#include "lyricmodel.h"
#include "filemanager.h"

LyricModel::LyricModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int LyricModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_lines.size();
}

QVariant LyricModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_lines.size())
        return {};

    const LyricLine &line = m_lines.at(index.row());
    switch (role) {
    case TimeRole: return line.timeMs;
    case TextRole: return line.text;
    }
    return {};
}

QHash<int, QByteArray> LyricModel::roleNames() const
{
    return {{TimeRole, "lyricTime"}, {TextRole, "lyricText"}};
}

int LyricModel::count() const
{
    return m_lines.size();
}

int LyricModel::indexAtTime(qint64 position) const
{
    for (int i = m_lines.size() - 1; i >= 0; --i) {
        if (position >= m_lines[i].timeMs)
            return i;
    }
    return -1;
}

void LyricModel::loadForAudio(const QString &audioFilePath)
{
    clear();

    QString lyricPath = FileManager::findLyricFile(audioFilePath);
    if (lyricPath.isEmpty())
        return;

    QVector<LyricLine> lines;
    if (lyricPath.endsWith(".lrc", Qt::CaseInsensitive))
        lines = LyricParser::parseLrc(lyricPath);
    else if (lyricPath.endsWith(".vtt", Qt::CaseInsensitive))
        lines = LyricParser::parseVtt(lyricPath);

    if (lines.isEmpty())
        return;

    beginResetModel();
    m_lines = lines;
    endResetModel();
    emit countChanged();
}

void LyricModel::clear()
{
    beginResetModel();
    m_lines.clear();
    endResetModel();
    emit countChanged();
}
