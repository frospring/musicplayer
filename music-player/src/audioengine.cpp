#include "audioengine.h"

AudioEngine::AudioEngine(QObject *parent)
    : QObject(parent)
{
    m_player = new QMediaPlayer(this);
    m_output = new QAudioOutput(this);
    m_player->setAudioOutput(m_output);
    m_output->setVolume(1.0);

    // 信号直连转发
    connect(m_player, &QMediaPlayer::positionChanged,
            this, &AudioEngine::positionChanged);
    connect(m_player, &QMediaPlayer::durationChanged,
            this, &AudioEngine::durationChanged);
    connect(m_player, &QMediaPlayer::playbackStateChanged, this, [this]() {
        emit stateChanged();
    });
    connect(m_player, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::LoadedMedia || status == QMediaPlayer::BufferedMedia)
            emit loadedChanged();
    });
    connect(m_player, &QMediaPlayer::metaDataChanged,
            this, &AudioEngine::metaDataChanged);
    connect(m_player, &QMediaPlayer::errorOccurred, this, [this](QMediaPlayer::Error err, const QString &str) {
        m_error = str;
        emit errorOccurred(str);
    });
}

QUrl AudioEngine::source() const           { return m_player->source(); }
qint64 AudioEngine::position() const       { return m_player->position(); }
qint64 AudioEngine::duration() const       { return m_player->duration(); }
int AudioEngine::state() const             { return static_cast<int>(m_player->playbackState()); }
bool AudioEngine::isLoaded() const         { return m_player->mediaStatus() == QMediaPlayer::LoadedMedia
                                                || m_player->mediaStatus() == QMediaPlayer::BufferedMedia; }
QString AudioEngine::error() const         { return m_error; }
QMediaMetaData AudioEngine::metaData() const { return m_player->metaData(); }

void AudioEngine::setSource(const QUrl &url)
{
    m_error.clear();
    m_player->setSource(url);
    emit sourceChanged();
}

void AudioEngine::play()    { m_player->play(); }
void AudioEngine::pause()   { m_player->pause(); }
void AudioEngine::stop()    { m_player->stop(); }
void AudioEngine::seek(qint64 position) { m_player->setPosition(position); }
