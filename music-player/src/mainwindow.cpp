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
#include <QFile>
#include <QTextStream>
#include <QMediaMetaData>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_isPlaying(false), m_currentLyricIndex(-1)
{
    setWindowTitle("音乐播放器");
    resize(500, 550);

    auto *central = new QWidget(this);
    setCentralWidget(central);

    auto *layout = new QVBoxLayout(central);
    layout->setSpacing(6);

    m_titleLabel = new QLabel("未选择歌曲", this);
    m_artistLabel = new QLabel("", this);
    m_lyricsList = new QListWidget(this);
    m_lyricsList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_lyricsList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_lyricsList->setSelectionMode(QAbstractItemView::NoSelection);
    m_lyricsList->setFocusPolicy(Qt::NoFocus);
    m_lyricsList->setStyleSheet("QListWidget { border: none; background: transparent; } QListWidget::item { padding: 6px; }");
    m_lyricsList->setFixedHeight(180);
    auto *noLyricItem = new QListWidgetItem("暂无歌词");
    noLyricItem->setTextAlignment(Qt::AlignCenter);
    noLyricItem->setFlags(noLyricItem->flags() & ~Qt::ItemIsSelectable);
    m_lyricsList->addItem(noLyricItem);

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
    layout->addLayout(btnLayout);
    layout->addWidget(m_progressSlider);
    layout->addWidget(m_playlist, 1);
    layout->addWidget(m_lyricsList);

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
    connect(m_player, &QMediaPlayer::metaDataChanged, this, &MainWindow::onMetaDataChanged);
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

    if (m_lyricTimes.isEmpty()) return;

    int idx = m_lyricTimes.size() - 1;
    for (int i = 0; i < m_lyricTimes.size(); ++i) {
        if (pos < m_lyricTimes[i]) {
            idx = i - 1;
            break;
        }
    }

    if (idx != m_currentLyricIndex && idx >= 0 && idx < m_lyricsList->count()) {
        if (m_currentLyricIndex >= 0 && m_currentLyricIndex < m_lyricsList->count()) {
            auto *prevItem = m_lyricsList->item(m_currentLyricIndex);
            prevItem->setForeground(m_lyricsList->palette().color(QPalette::Text));
            QFont prevFont = prevItem->font();
            prevFont.setBold(false);
            prevItem->setFont(prevFont);
        }
        m_currentLyricIndex = idx;
        auto *curItem = m_lyricsList->item(idx);
        curItem->setForeground(QColor("#1db954"));
        QFont curFont = curItem->font();
        curFont.setBold(true);
        curItem->setFont(curFont);
        m_lyricsList->scrollToItem(curItem, QAbstractItemView::PositionAtCenter);
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

void MainWindow::onMetaDataChanged()
{
    auto meta = m_player->metaData();

    QString title = meta.value(QMediaMetaData::Title).toString();
    if (!title.isEmpty()) {
        m_titleLabel->setText(title);
    }

    QString artist = meta.value(QMediaMetaData::AlbumArtist).toString();
    if (artist.isEmpty()) {
        artist = meta.value(QMediaMetaData::ContributingArtist).toString();
    }
    if (artist.isEmpty()) {
        artist = meta.value(QMediaMetaData::LeadPerformer).toString();
    }
    QString album = meta.value(QMediaMetaData::AlbumTitle).toString();
    if (!artist.isEmpty() && !album.isEmpty()) {
        m_artistLabel->setText(artist + " — " + album);
    } else if (!artist.isEmpty()) {
        m_artistLabel->setText(artist);
    } else if (!album.isEmpty()) {
        m_artistLabel->setText(album);
    }
}

void MainWindow::loadLyrics(const QString &audioPath)
{
    m_lyricTimes.clear();
    m_lyricTexts.clear();
    m_currentLyricIndex = -1;
    m_lyricsList->clear();

    m_artistLabel->setText("");

    QFileInfo fi(audioPath);
    QString dirPath = fi.absolutePath();
    QString baseName = fi.completeBaseName();

    QFile file(dirPath + "/" + baseName + ".lrc");
    bool isVtt = false;

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QDir dir(dirPath);
        QStringList vttFiles = dir.entryList({baseName + "*.vtt"}, QDir::Files, QDir::Name);
        if (vttFiles.isEmpty()) {
            auto *plItem = new QListWidgetItem("暂无歌词");
            plItem->setTextAlignment(Qt::AlignCenter);
            plItem->setFlags(plItem->flags() & ~Qt::ItemIsSelectable);
            m_lyricsList->addItem(plItem);
            return;
        }
        file.setFileName(dirPath + "/" + vttFiles.first());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            auto *plItem2 = new QListWidgetItem("暂无歌词");
            plItem2->setTextAlignment(Qt::AlignCenter);
            plItem2->setFlags(plItem2->flags() & ~Qt::ItemIsSelectable);
            m_lyricsList->addItem(plItem2);
            return;
        }
        isVtt = true;
    }

    QTextStream in(&file);

    if (isVtt) {
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (line.isEmpty() || line == "WEBVTT") continue;
            if (!line.contains("-->") && line.contains(':')) continue;

            if (line.contains("-->")) {
                int arrowPos = line.indexOf("-->");
                QString startStr = line.left(arrowPos).trimmed();
                QStringList parts = startStr.split(':');
                if (parts.size() == 3) {
                    qint64 ms = parts[0].toInt() * 3600000
                              + parts[1].toInt() * 60000
                              + static_cast<qint64>(parts[2].toDouble() * 1000);
                    m_lyricTimes.append(ms);

                    if (!in.atEnd()) {
                        QString text = in.readLine().trimmed();
                        m_lyricTexts.append(text);
                    }
                }
            }
        }
    } else {
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (line.isEmpty()) continue;

            if (line.startsWith('[') && line.contains(']')) {
                int close = line.indexOf(']');
                QString timeStr = line.mid(1, close - 1);
                QString text = line.mid(close + 1);

                int colon = timeStr.indexOf(':');
                if (colon > 0) {
                    int min = timeStr.left(colon).toInt();
                    double sec = timeStr.mid(colon + 1).toDouble();
                    qint64 ms = static_cast<qint64>(min * 60000 + sec * 1000);

                    m_lyricTimes.append(ms);
                    m_lyricTexts.append(text);
                }
            }
        }
    }

    file.close();

    if (m_lyricTimes.isEmpty()) {
        auto *plItem = new QListWidgetItem("无歌词");
        plItem->setTextAlignment(Qt::AlignCenter);
        plItem->setFlags(plItem->flags() & ~Qt::ItemIsSelectable);
        m_lyricsList->addItem(plItem);
    } else {
        for (const auto &text : m_lyricTexts) {
            auto *lyricItem = new QListWidgetItem(text);
            lyricItem->setTextAlignment(Qt::AlignCenter);
            lyricItem->setFlags(lyricItem->flags() & ~Qt::ItemIsSelectable);
            m_lyricsList->addItem(lyricItem);
        }
        m_currentLyricIndex = 0;
        auto *firstItem = m_lyricsList->item(0);
        QFont boldFont = firstItem->font();
        boldFont.setBold(true);
        firstItem->setFont(boldFont);
        firstItem->setForeground(QColor("#1db954"));
    }
}

void MainWindow::onItemDoubleClicked(QListWidgetItem *item)
{
    if (!item) return;

    QString path = item->text();
    m_player->setSource(QUrl::fromLocalFile(path));

    QFileInfo fi(path);
    m_titleLabel->setText(fi.completeBaseName());

    loadLyrics(path);

    m_player->play();
}
