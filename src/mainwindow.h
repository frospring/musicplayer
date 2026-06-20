#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QLabel;
class QPushButton;
class QSlider;
class QListWidget;
class QMediaPlayer;
class QAudioOutput;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;

private:
    QLabel *m_titleLabel;
    QLabel *m_artistLabel;
    QLabel *m_lyricsLabel;
    QPushButton *m_playBtn;
    QSlider *m_progressSlider;
    QListWidget *m_playlist;
    QMediaPlayer *m_player;
    QAudioOutput *m_audioOutput;
};

#endif
