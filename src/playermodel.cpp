// zkw2024051604060
#include "playermodel.h"
#include <QFileInfo>

PlaylistModel::PlaylistModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_currentIndex(-1)
{
}

int PlaylistModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_files.size();
}

QVariant PlaylistModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_files.size())
        return {};

    switch (role) {
    case FilePathRole:
        return m_files[index.row()];
    case FileNameRole:
        return QFileInfo(m_files[index.row()]).fileName();
    }
    return {};
}

QHash<int, QByteArray> PlaylistModel::roleNames() const
{
    return {
        {FilePathRole, "filePath"},
        {FileNameRole, "fileName"},
    };
}

int PlaylistModel::currentIndex() const { return m_currentIndex; }

void PlaylistModel::setCurrentIndex(int idx)
{
    if (idx == m_currentIndex || idx < 0 || idx >= m_files.size()) return;
    m_currentIndex = idx;
    emit currentIndexChanged();
}

int PlaylistModel::count() const { return m_files.size(); }

QUrl PlaylistModel::currentFileUrl() const
{
    if (m_currentIndex < 0 || m_currentIndex >= m_files.size())
        return {};
    return QUrl::fromLocalFile(m_files[m_currentIndex]);
}

void PlaylistModel::addFiles(const QList<QUrl> &urls)
{
    if (urls.isEmpty()) return;

    int first = m_files.size();
    int last = first + urls.size() - 1;
    beginInsertRows(QModelIndex(), first, last);

    for (const auto &url : urls)
        m_files.append(url.toLocalFile());

    endInsertRows();
    emit countChanged();
}

void PlaylistModel::remove(int index)
{
    if (index < 0 || index >= m_files.size()) return;

    beginRemoveRows(QModelIndex(), index, index);
    m_files.removeAt(index);
    endRemoveRows();

    int oldIndex = m_currentIndex;
    if (m_files.isEmpty()) {
        m_currentIndex = -1;
    } else if (m_currentIndex == index) {
        m_currentIndex = -1;
    } else if (m_currentIndex > index) {
        m_currentIndex--;
    }

    if (m_currentIndex != oldIndex)
        emit currentIndexChanged();

    emit countChanged();
}

void PlaylistModel::clear()
{
    beginResetModel();
    m_files.clear();
    m_currentIndex = -1;
    endResetModel();
    emit countChanged();
}
