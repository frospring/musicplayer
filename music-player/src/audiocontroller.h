#ifndef AUDIOCONTROLLER_H
#define AUDIOCONTROLLER_H

#include <QObject>
#include <QUrl>
#include "audioengine.h"
#include "lyricmodel.h"

// 音频控制器 - 协调 AudioEngine + LyricModel + FileManager
// 职责: 高级播放控制、元数据提取、歌词同步
class AudioController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QString artist READ artist NOTIFY artistChanged)
    Q_PROPERTY(QString album READ album NOTIFY albumChanged)
    Q_PROPERTY(bool isPlaying READ isPlaying NOTIFY isPlayingChanged)
    Q_PROPERTY(bool hasMedia READ hasMedia NOTIFY hasMediaChanged)
    Q_PROPERTY(int currentLyricIndex READ currentLyricIndex NOTIFY currentLyricIndexChanged)
    Q_PROPERTY(LyricModel* lyricModel READ lyricModel CONSTANT)
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorOccurred)
    Q_PROPERTY(qint64 position READ position NOTIFY positionChanged)
    Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged)

public:
    explicit AudioController(AudioEngine *engine, LyricModel *lyricModel,
                             QObject *parent = nullptr);

    QString title() const;
    QString artist() const;
    QString album() const;
    bool isPlaying() const;
    bool hasMedia() const;
    int currentLyricIndex() const;
    LyricModel* lyricModel() const;
    QString errorString() const;
    qint64 position() const;
    qint64 duration() const;

    // 播放指定文件 (加载歌词 + 异步播放)
    Q_INVOKABLE void playFile(const QUrl &url);
    Q_INVOKABLE void togglePlayPause();
    Q_INVOKABLE void seek(qint64 pos);

signals:
    void titleChanged();
    void artistChanged();
    void albumChanged();
    void isPlayingChanged();
    void hasMediaChanged();
    void currentLyricIndexChanged();
    void errorOccurred();
    void positionChanged();
    void durationChanged();

private slots:
    void onEngineStateChanged();
    void onEnginePositionChanged();
    void onEngineLoaded();
    void onEngineMetaDataChanged();
    void onEngineError(const QString &error);

private:
    void updateLyricIndex();
    void loadLyrics(const QUrl &audioUrl);

    AudioEngine *m_engine;
    LyricModel *m_lyricModel;
    QString m_title;
    QString m_artist;
    QString m_album;
    QString m_error;
    int m_currentLyricIndex;
    bool m_playPending;  // 标记: 媒体加载后自动播放
};

#endif
