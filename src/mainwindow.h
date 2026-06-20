#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QStringList>

class QLabel;
class QPushButton;
class QSlider;
class QListWidget;
class QListWidgetItem;
class QMediaPlayer;
class QAudioOutput;
class QSplitter;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;

private slots:
    void onOpenFile();
    void onPlayPause();
    void onPlayerStateChanged();
    void onPositionChanged(qint64 pos);
    void onDurationChanged(qint64 dur);
    void onSliderMoved(int val);
    void onItemDoubleClicked(QListWidgetItem *item);
    void onMetaDataChanged();

private:
    void loadLyrics(const QString &audioPath);
    void parseLrc(const QString &path);
    void parseVtt(const QString &path);

    QLabel *m_titleLabel;
    QLabel *m_artistLabel;
    QPushButton *m_playBtn;
    QPushButton *m_openBtn;
    QSlider *m_progressSlider;
    QListWidget *m_playlist;
    QListWidget *m_lyricsWidget;
    QMediaPlayer *m_player;
    QAudioOutput *m_audioOutput;
    QSplitter *m_splitter;
    bool m_isPlaying;

    QVector<qint64> m_lyricTimes;
    QStringList m_lyricTexts;
    int m_currentLyricIndex;
};

#endif
