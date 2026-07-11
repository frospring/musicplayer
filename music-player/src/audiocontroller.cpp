#include "audiocontroller.h"
#include <QFileInfo>
#include <QMediaMetaData>

AudioController::AudioController(AudioEngine *engine, LyricModel *lyricModel,
                                 QObject *parent)
    : QObject(parent)
    , m_engine(engine)
    , m_lyricModel(lyricModel)
    , m_currentLyricIndex(-1)
    , m_playPending(false)
{
    connect(m_engine, &AudioEngine::stateChanged,
            this, &AudioController::onEngineStateChanged);
    connect(m_engine, &AudioEngine::positionChanged,
            this, &AudioController::onEnginePositionChanged);
    connect(m_engine, &AudioEngine::loadedChanged,
            this, &AudioController::onEngineLoaded);
    connect(m_engine, &AudioEngine::metaDataChanged,
            this, &AudioController::onEngineMetaDataChanged);
    connect(m_engine, &AudioEngine::errorOccurred,
            this, &AudioController::onEngineError);
    connect(m_engine, &AudioEngine::sourceChanged,
            this, &AudioController::sourceChanged);
}

QString AudioController::title() const       { return m_title; }
QString AudioController::artist() const      { return m_artist; }
QString AudioController::album() const       { return m_album; }
bool AudioController::isPlaying() const      { return m_engine->state() == AudioEngine::Playing; }
bool AudioController::hasMedia() const       { return !m_engine->source().isEmpty(); }
int AudioController::currentLyricIndex() const { return m_currentLyricIndex; }
LyricModel* AudioController::lyricModel() const { return m_lyricModel; }
QString AudioController::errorString() const { return m_error; }
qint64 AudioController::position() const     { return m_engine->position(); }
qint64 AudioController::duration() const     { return m_engine->duration(); }
QUrl AudioController::source() const          { return m_engine->source(); }

void AudioController::playFile(const QUrl &url)
{
    m_playPending = true;
    m_engine->setSource(url);

    m_title = QFileInfo(url.toLocalFile()).fileName();
    m_artist.clear();
    m_album.clear();
    m_error.clear();
    emit titleChanged();
    emit artistChanged();
    emit albumChanged();
    emit hasMediaChanged();

    m_currentLyricIndex = -1;
    emit currentLyricIndexChanged();
    loadLyrics(url);
}

void AudioController::togglePlayPause()
{
    if (isPlaying())
        m_engine->pause();
    else
        m_engine->play();
}

void AudioController::play()
{
    m_engine->play();
}

void AudioController::pause()
{
    m_engine->pause();
}

void AudioController::stop()
{
    m_engine->stop();
    m_engine->setSource(QUrl());
    m_title.clear();
    m_artist.clear();
    m_album.clear();
    m_error.clear();
    m_currentLyricIndex = -1;
    m_lyricModel->clear();
    m_playPending = false;
    emit titleChanged();
    emit artistChanged();
    emit albumChanged();
    emit hasMediaChanged();
    emit currentLyricIndexChanged();
    emit positionChanged();
    emit durationChanged();
    emit sourceChanged();
}

void AudioController::seek(qint64 pos)
{
    m_engine->seek(pos);
}

void AudioController::onEngineStateChanged()
{
    emit isPlayingChanged();
}

void AudioController::onEnginePositionChanged()
{
    emit positionChanged();
    updateLyricIndex();
}

void AudioController::onEngineLoaded()
{
    emit durationChanged();
    if (m_playPending) {
        m_playPending = false;
        m_engine->play();
    }
}

void AudioController::onEngineMetaDataChanged()
{
    QMediaMetaData meta = m_engine->metaData();

    QString title = meta.stringValue(QMediaMetaData::Title);
    QString artist = meta.stringValue(QMediaMetaData::AlbumArtist);
    if (artist.isEmpty())
        artist = meta.stringValue(QMediaMetaData::ContributingArtist);
    if (artist.isEmpty())
        artist = meta.stringValue(QMediaMetaData::LeadPerformer);
    QString album = meta.stringValue(QMediaMetaData::AlbumTitle);

    if (!title.isEmpty() && title != m_title) {
        m_title = title;
        emit titleChanged();
    }
    if (!artist.isEmpty() && artist != m_artist) {
        m_artist = artist;
        emit artistChanged();
    }
    if (!album.isEmpty() && album != m_album) {
        m_album = album;
        emit albumChanged();
    }
}

void AudioController::onEngineError(const QString &error)
{
    m_error = error;
    m_playPending = false;
    emit errorOccurred();
}

void AudioController::updateLyricIndex()
{
    int idx = m_lyricModel->indexAtTime(m_engine->position());
    if (idx != m_currentLyricIndex) {
        m_currentLyricIndex = idx;
        emit currentLyricIndexChanged();
    }
}

void AudioController::loadLyrics(const QUrl &audioUrl)
{
    m_lyricModel->loadForAudio(audioUrl.toLocalFile());
}
