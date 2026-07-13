#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H

#include <QObject>
#include <QUrl>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QMediaMetaData>

// 音频引擎 - 底层封装 QMediaPlayer + QAudioOutput
// 职责: 原始音频播放控制、状态信号转发、元数据提取
class AudioEngine : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source NOTIFY sourceChanged)
    Q_PROPERTY(qint64 position READ position NOTIFY positionChanged)
    Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(int state READ state NOTIFY stateChanged)
    Q_PROPERTY(bool loaded READ isLoaded NOTIFY loadedChanged)
    Q_PROPERTY(QString error READ error NOTIFY errorOccurred)
    Q_PROPERTY(qreal volume READ volume WRITE setVolume NOTIFY volumeChanged)

public:
    // 播放状态枚举
    enum State { Stopped = 0, Playing = 1, Paused = 2 };
    Q_ENUM(State)

    explicit AudioEngine(QObject *parent = nullptr);

    QUrl source() const;
    qint64 position() const;
    qint64 duration() const;
    int state() const;
    bool isLoaded() const;
    QString error() const;
    qreal volume() const;

    // 获取原始元数据 (供上层解析)
    QMediaMetaData metaData() const;

public slots:
    void setSource(const QUrl &url);
    void play();
    void pause();
    void stop();
    void seek(qint64 position);
    void setVolume(qreal vol);

signals:
    void sourceChanged();
    void positionChanged();
    void durationChanged();
    void stateChanged();
    void loadedChanged();
    void errorOccurred(const QString &error);
    void metaDataChanged();
    void volumeChanged();

private:
    QMediaPlayer *m_player;
    QAudioOutput *m_output;
    QString m_error;
};

#endif
