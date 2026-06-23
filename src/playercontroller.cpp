// zkw2024051604060
#include "playercontroller.h"

PlayerController::PlayerController(QObject *parent)
    : QObject(parent)
    , m_player(new QMediaPlayer(this))
    , m_audioOutput(new QAudioOutput(this))
    , m_title("-")
    , m_artist("-")
    , m_album("-")
    , m_hasEmbeddedLyrics(false)
{
    m_player->setAudioOutput(m_audioOutput);

    connect(m_player, &QMediaPlayer::playbackStateChanged, this, [this](QMediaPlayer::PlaybackState state) {
        Q_UNUSED(state);
        emit playingChanged();
    });

    connect(m_player, &QMediaPlayer::positionChanged, this, &PlayerController::positionChanged);
    connect(m_player, &QMediaPlayer::durationChanged, this, &PlayerController::durationChanged);

    // 元数据: 读取标题/艺术家/专辑/内嵌歌词
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

        // ID3 USLT 帧 (非同步歌词)
        m_embeddedLyrics = meta.value(QMediaMetaData::Lyrics).toString();
        m_hasEmbeddedLyrics = !m_embeddedLyrics.isEmpty();

        emit metaDataChanged();
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
    m_embeddedLyrics = "";
    m_hasEmbeddedLyrics = false;
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
bool PlayerController::hasEmbeddedLyrics() const { return m_hasEmbeddedLyrics; }
QString PlayerController::embeddedLyrics() const { return m_embeddedLyrics; }

void PlayerController::play() { m_player->play(); }
void PlayerController::pause() { m_player->pause(); }
void PlayerController::toggle()
{
    if (isPlaying()) m_player->pause();
    else m_player->play();
}

void PlayerController::seek(qint64 pos) { m_player->setPosition(pos); }
