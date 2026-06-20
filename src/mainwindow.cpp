#include "mainwindow.h"

#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QFileDialog>
#include <QUrl>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_isPlaying(false)
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
    m_openBtn = new QPushButton("打开文件", this);

    m_progressSlider = new QSlider(Qt::Horizontal, this);
    m_progressSlider->setRange(0, 0);

    m_playlist = new QListWidget(this);

    auto *infoLayout = new QVBoxLayout;
    infoLayout->addWidget(m_titleLabel);
    infoLayout->addWidget(m_artistLabel);

    auto *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(m_openBtn);
    btnLayout->addWidget(m_playBtn);

    layout->addLayout(infoLayout);
    layout->addWidget(m_lyricsLabel);
    layout->addLayout(btnLayout);
    layout->addWidget(m_progressSlider);
    layout->addWidget(m_playlist);

    m_player = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_player->setAudioOutput(m_audioOutput);

    connect(m_openBtn, &QPushButton::clicked, this, &MainWindow::onOpenFile);
    connect(m_playBtn, &QPushButton::clicked, this, &MainWindow::onPlayPause);
    connect(m_player, &QMediaPlayer::playbackStateChanged, this, &MainWindow::onPlayerStateChanged);
    connect(m_player, &QMediaPlayer::positionChanged, this, &MainWindow::onPositionChanged);
    connect(m_player, &QMediaPlayer::durationChanged, this, &MainWindow::onDurationChanged);
    connect(m_progressSlider, &QSlider::sliderMoved, this, &MainWindow::onSliderMoved);
    connect(m_playlist, &QListWidget::itemDoubleClicked, this, &MainWindow::onItemDoubleClicked);
}

void MainWindow::onOpenFile()
{
    QStringList files = QFileDialog::getOpenFileNames(
        this, "选择音乐文件", QString(),
        "音频文件 (*.mp3 *.wav *.flac *.ogg *.m4a)");

    for (const auto &path : files) {
        m_playlist->addItem(path);
    }
}

void MainWindow::onPlayPause()
{
    if (m_playlist->currentRow() < 0 && m_playlist->count() > 0) {
        m_playlist->setCurrentRow(0);
        onItemDoubleClicked(m_playlist->item(0));
        return;
    }

    if (m_isPlaying) {
        m_player->pause();
    } else {
        m_player->play();
    }
}

void MainWindow::onPlayerStateChanged()
{
    auto state = m_player->playbackState();
    if (state == QMediaPlayer::PlayingState) {
        m_isPlaying = true;
        m_playBtn->setText("暂停");
    } else {
        m_isPlaying = false;
        m_playBtn->setText("播放");
    }
}

void MainWindow::onPositionChanged(qint64 pos)
{
    if (!m_progressSlider->isSliderDown()) {
        m_progressSlider->setValue(static_cast<int>(pos));
    }
}

void MainWindow::onDurationChanged(qint64 dur)
{
    m_progressSlider->setRange(0, static_cast<int>(dur));
}

void MainWindow::onSliderMoved(int val)
{
    m_player->setPosition(val);
}

void MainWindow::onItemDoubleClicked(QListWidgetItem *item)
{
    if (!item) return;

    QString path = item->text();
    m_player->setSource(QUrl::fromLocalFile(path));

    QFileInfo fi(path);
    m_titleLabel->setText(fi.completeBaseName());

    m_player->play();
}
