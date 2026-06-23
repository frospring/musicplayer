// zkw2024051604060
#pragma once

#include <QAbstractListModel>
#include <QUrl>
#include <QStringList>
#include <QtQml/qqmlregistration.h>

class PlaylistModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged FINAL)
    Q_PROPERTY(int count READ count NOTIFY countChanged FINAL)

public:
    enum Roles {
        FilePathRole = Qt::UserRole + 1,
        FileNameRole
    };

    explicit PlaylistModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    int currentIndex() const;
    void setCurrentIndex(int idx);
    int count() const;

    Q_INVOKABLE void addFiles(const QList<QUrl> &urls);
    Q_INVOKABLE void remove(int index);
    Q_INVOKABLE void clear();

signals:
    void currentIndexChanged();
    void countChanged();

private:
    QStringList m_files;
    int m_currentIndex;
};
