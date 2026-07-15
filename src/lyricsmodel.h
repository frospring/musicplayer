// zkw2024051604060
#pragma once

#include <QAbstractListModel>
#include <QUrl>
#include <QVector>
#include <QtQml/qqmlregistration.h>
#include "lyricsparser.h"

class LyricsModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(int currentIndex READ currentIndex NOTIFY currentIndexChanged FINAL)
    Q_PROPERTY(int count READ count NOTIFY countChanged FINAL)

public:
    enum Roles {
        LyricTextRole = Qt::UserRole + 1,
        IsCurrentRole,
        TimestampRole
    };

    explicit LyricsModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    int currentIndex() const;
    void setCurrentIndex(int idx);
    int count() const;

    Q_INVOKABLE void loadLyrics(const QUrl &audioUrl);
    Q_INVOKABLE void updatePosition(qint64 positionMs);

signals:
    void currentIndexChanged();
    void countChanged();

private:
    QVector<LyricEntry> m_entries;
    int m_currentIndex;
};
