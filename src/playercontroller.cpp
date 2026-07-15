// zkw2024051604060
#include "playercontroller.h"
#include <QMediaMetaData>

PlayerController::PlayerController(QObject *parent)
    : QObject(parent)
    , m_player(new QMediaPlayer(this))
    , m_audioOutput(new QAudioOutput(this))
    , m_title("-")
    , m_artist("-")
    , m_album("-")
{
    m_player->setAudioOutput(m_audioOutput);

    connect(m_player, &QMediaPlayer::playbackStateChanged, this, [this](QMediaPlayer::PlaybackState state) {
        Q_UNUSED(state);
        emit playingChanged();
    });

    connect(m_player, &QMediaPlayer::positionChanged, this, &PlayerController::positionChanged);
    connect(m_player, &QMediaPlayer::durationChanged, this, &PlayerController::durationChanged);

    connect(m_player, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia)
            emit ended();
    });

    connect(m_player, &QMediaPlayer::metaDataChanged, this, [this]() {
        auto meta = m_player->metaData();

        QString t = meta.value(QMediaMetaData::Title).toString();
        m_title = t.isEmpty() ? "-" : t;

        // 艺术家优先读取 AlbumArtist, 回退 ContributingArtist/LeadPerformer
        QString a = meta.value(QMediaMetaData::AlbumArtist).toString();
        if (a.isEmpty()) a = meta.value(QMediaMetaData::ContributingArtist).toString();
        if (a.isEmpty()) a = meta.value(QMediaMetaData::LeadPerformer).toString();
        m_artist = a.isEmpty() ? "-" : a;

        QString al = meta.value(QMediaMetaData::AlbumTitle).toString();
        m_album = al.isEmpty() ? "-" : al;

        emit metaDataChanged();
    });

    connect(m_player, &QMediaPlayer::errorOccurred, this, [this](QMediaPlayer::Error /*err*/, const QString &errStr) {
        m_error = errStr;
        emit errorOccurred();
    });
}

QUrl PlayerController::source() const { return m_source; }

void PlayerController::setSource(const QUrl &url)
{
    if (m_source == url) return;
    m_source = url;
    m_title = "-";
    m_artist = "-";
    m_album = "-";
    m_error.clear();
    m_player->setSource(url);
    emit sourceChanged();
    emit metaDataChanged();
}

bool PlayerController::isPlaying() const
{
    return m_player->playbackState() == QMediaPlayer::PlayingState;
}

qint64 PlayerController::position() const { return m_player->position(); }
qint64 PlayerController::duration() const { return m_player->duration(); }
QString PlayerController::title() const { return m_title; }
QString PlayerController::artist() const { return m_artist; }
QString PlayerController::album() const { return m_album; }

float PlayerController::volume() const { return m_audioOutput->volume(); }

void PlayerController::setVolume(float vol)
{
    vol = qBound(0.0f, vol, 1.0f);
    if (qAbs(vol - m_audioOutput->volume()) < 0.001f) return;
    m_audioOutput->setVolume(vol);
    emit volumeChanged();
}

void PlayerController::play() { m_player->play(); }
void PlayerController::pause() { m_player->pause(); }
void PlayerController::toggle()
{
    if (isPlaying()) m_player->pause();
    else m_player->play();
}

void PlayerController::stop()
{
    m_player->stop();
    m_source.clear();
    m_title = "-";
    m_artist = "-";
    m_album = "-";
    m_error.clear();
    m_player->setSource(QUrl());
    emit sourceChanged();
    emit metaDataChanged();
    emit playingChanged();
    emit positionChanged();
    emit durationChanged();
}

void PlayerController::seek(qint64 pos) { m_player->setPosition(pos); }

QString PlayerController::errorString() const { return m_error; }
