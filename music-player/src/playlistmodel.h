#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <QAbstractListModel>
#include <QUrl>
#include <QList>

// 播放列表数据模型 - 管理音频文件 URL 列表
// 角色: filePath (QUrl), fileName (QString)
class PlaylistModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum Roles { FilePathRole = Qt::UserRole + 1, FileNameRole };

    explicit PlaylistModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    int currentIndex() const;
    void setCurrentIndex(int index);
    int count() const;

    Q_INVOKABLE void addFiles(const QList<QUrl> &urls);
    Q_INVOKABLE void addFile(const QUrl &url);
    Q_INVOKABLE void removeItem(int index);
    Q_INVOKABLE void clear();
    Q_INVOKABLE QUrl fileUrlAt(int index) const;

signals:
    void currentIndexChanged();
    void countChanged();

private:
    QList<QUrl> m_files;
    int m_currentIndex;
};

#endif
