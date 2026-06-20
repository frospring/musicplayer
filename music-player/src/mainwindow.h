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

    QLabel *m_titleLabel;
    QLabel *m_artistLabel;
    QLabel *m_lyricsLabel;
    QPushButton *m_playBtn;
    QPushButton *m_openBtn;
    QSlider *m_progressSlider;
    QListWidget *m_playlist;
    QMediaPlayer *m_player;
    QAudioOutput *m_audioOutput;
    bool m_isPlaying;

    QVector<qint64> m_lyricTimes;
    QStringList m_lyricTexts;
    int m_currentLyricIndex;
};

#endif
