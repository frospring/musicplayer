// zkw2024051604060
#pragma once

#include <QObject>
#include <QUrl>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QtQml/qqmlregistration.h>

class PlayerController : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged FINAL)
    Q_PROPERTY(bool playing READ isPlaying NOTIFY playingChanged FINAL)
    Q_PROPERTY(qint64 position READ position NOTIFY positionChanged FINAL)
    Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged FINAL)
    Q_PROPERTY(QString title READ title NOTIFY metaDataChanged FINAL)
    Q_PROPERTY(QString artist READ artist NOTIFY metaDataChanged FINAL)
    Q_PROPERTY(QString album READ album NOTIFY metaDataChanged FINAL)

public:
    explicit PlayerController(QObject *parent = nullptr);

    QUrl source() const;
    void setSource(const QUrl &url);

    bool isPlaying() const;
    qint64 position() const;
    qint64 duration() const;
    QString title() const;
    QString artist() const;
    QString album() const;

public slots:
    void play();
    void pause();
    void toggle();
    void seek(qint64 pos);

signals:
    void sourceChanged();
    void playingChanged();
    void positionChanged();
    void durationChanged();
    void metaDataChanged();
    void ended();

private:
    QMediaPlayer *m_player;
    QAudioOutput *m_audioOutput;
    QUrl m_source;
    QString m_title;
    QString m_artist;
    QString m_album;
};
