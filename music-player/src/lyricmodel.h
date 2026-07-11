#ifndef LYRICMODEL_H
#define LYRICMODEL_H

#include <QAbstractListModel>
#include <QVector>
#include "lyricparser.h"

class LyricModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum Roles { TimeRole = Qt::UserRole + 1, TextRole };

    explicit LyricModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    int count() const;

    int indexAtTime(qint64 position) const;

    Q_INVOKABLE void loadForAudio(const QString &audioFilePath);
    Q_INVOKABLE void clear();

signals:
    void countChanged();

private:
    QVector<LyricLine> m_lines;
};

#endif
