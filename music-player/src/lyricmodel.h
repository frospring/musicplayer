#ifndef LYRICMODEL_H
#define LYRICMODEL_H

#include <QAbstractListModel>
#include <QVector>
#include "lyricparser.h"

// 歌词数据模型 - 将 LyricParser 解析结果包装为 QAbstractListModel
// 供 QML ListView 直接绑定使用
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

    // 根据播放位置 (毫秒) 查找当前歌词行索引
    int indexAtTime(qint64 position) const;

    // 根据音频文件路径加载歌词 (自动查找 .lrc / .vtt)
    Q_INVOKABLE void loadForAudio(const QString &audioFilePath);
    Q_INVOKABLE void clear();

signals:
    void countChanged();

private:
    QVector<LyricLine> m_lines;
};

#endif
