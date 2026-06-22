#include "playlistmodel.h"
#include <QFileInfo>

PlaylistModel::PlaylistModel(QObject *parent)
    : QAbstractListModel(parent), m_currentIndex(-1)
{
}

int PlaylistModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_files.size();
}

QVariant PlaylistModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_files.size())
        return {};

    switch (role) {
    case FilePathRole: return m_files.at(index.row());
    case FileNameRole: return QFileInfo(m_files.at(index.row()).toLocalFile()).fileName();
    }
    return {};
}

QHash<int, QByteArray> PlaylistModel::roleNames() const
{
    return {{FilePathRole, "filePath"}, {FileNameRole, "fileName"}};
}

int PlaylistModel::currentIndex() const
{
    return m_currentIndex;
}

void PlaylistModel::setCurrentIndex(int index)
{
    if (index < -1 || index >= m_files.size() || index == m_currentIndex)
        return;

    int old = m_currentIndex;
    m_currentIndex = index;
    emit currentIndexChanged();

    // 通知视图刷新新旧行样式
    if (old >= 0 && old < m_files.size())
        emit dataChanged(this->index(old), this->index(old));
    if (index >= 0)
        emit dataChanged(this->index(index), this->index(index));
}

int PlaylistModel::count() const
{
    return m_files.size();
}

void PlaylistModel::addFiles(const QList<QUrl> &urls)
{
    if (urls.isEmpty()) return;

    bool wasEmpty = m_files.isEmpty();

    beginInsertRows(QModelIndex(), m_files.size(), m_files.size() + urls.size() - 1);
    m_files.append(urls);
    endInsertRows();
    emit countChanged();

    // 列表从空变非空时自动选中第一项
    if (wasEmpty) {
        m_currentIndex = 0;
        emit currentIndexChanged();
        emit dataChanged(index(0), index(0));
    }
}

void PlaylistModel::addFile(const QUrl &url)
{
    addFiles({url});
}

void PlaylistModel::removeItem(int index)
{
    if (index < 0 || index >= m_files.size()) return;

    beginRemoveRows(QModelIndex(), index, index);
    m_files.removeAt(index);
    endRemoveRows();
    emit countChanged();

    int old = m_currentIndex;
    if (m_files.isEmpty()) {
        m_currentIndex = -1;
    } else if (m_currentIndex >= m_files.size()) {
        m_currentIndex = m_files.size() - 1;
    } else if (index < m_currentIndex) {
        --m_currentIndex;
    }

    if (m_currentIndex != old) {
        emit currentIndexChanged();
        if (m_currentIndex >= 0)
            emit dataChanged(this->index(m_currentIndex), this->index(m_currentIndex));
    }
}

void PlaylistModel::clear()
{
    beginResetModel();
    m_files.clear();
    endResetModel();
    m_currentIndex = -1;
    emit currentIndexChanged();
    emit countChanged();
}

QUrl PlaylistModel::fileUrlAt(int index) const
{
    return (index >= 0 && index < m_files.size()) ? m_files.at(index) : QUrl();
}
