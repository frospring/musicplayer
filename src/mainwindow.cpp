#include "mainwindow.h"

#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMediaPlayer>
#include <QAudioOutput>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("音乐播放器");
    resize(500, 400);

    auto *central = new QWidget(this);
    setCentralWidget(central);

    auto *layout = new QVBoxLayout(central);
    layout->setSpacing(6);

    m_titleLabel = new QLabel("未选择歌曲", this);
    m_artistLabel = new QLabel("", this);
    m_lyricsLabel = new QLabel("暂无歌词", this);
    m_lyricsLabel->setWordWrap(true);
    m_lyricsLabel->setAlignment(Qt::AlignCenter);

    m_playBtn = new QPushButton("播放", this);

    m_progressSlider = new QSlider(Qt::Horizontal, this);

    m_playlist = new QListWidget(this);

    auto *infoLayout = new QVBoxLayout;
    infoLayout->addWidget(m_titleLabel);
    infoLayout->addWidget(m_artistLabel);

    auto *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(m_playBtn);

    layout->addLayout(infoLayout);
    layout->addWidget(m_lyricsLabel);
    layout->addLayout(btnLayout);
    layout->addWidget(m_progressSlider);
    layout->addWidget(m_playlist);

    m_player = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_player->setAudioOutput(m_audioOutput);
}
