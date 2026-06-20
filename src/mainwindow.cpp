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
#include <QSplitter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_isPlaying(false), m_currentLyricIndex(-1)
{
    setWindowTitle("音乐播放器");
    resize(700, 500);

    auto *central = new QWidget(this);
    setCentralWidget(central);

    auto *mainLayout = new QVBoxLayout(central);
    mainLayout->setSpacing(6);
    mainLayout->setContentsMargins(8, 8, 8, 8);

    m_titleLabel = new QLabel("未选择歌曲", this);
    m_artistLabel = new QLabel("", this);

    m_playBtn = new QPushButton("播放", this);
    m_openBtn = new QPushButton("打开文件", this);

    m_progressSlider = new QSlider(Qt::Horizontal, this);
    m_progressSlider->setRange(0, 0);

    m_lyricsWidget = new QListWidget(this);
    m_lyricsWidget->setSelectionMode(QAbstractItemView::NoSelection);
    m_lyricsWidget->setFocusPolicy(Qt::NoFocus);
    m_lyricsWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_lyricsWidget->setStyleSheet(
        "QListWidget { font-size: 14px; background: transparent; border: none; }"
        "QListWidget::item { padding: 4px 8px; }"
        "QListWidget::item:!selected { color: #888; }");

    m_playlist = new QListWidget(this);
    m_playlist->setMaximumHeight(150);

    m_splitter = new QSplitter(Qt::Vertical, this);
    m_splitter->addWidget(m_lyricsWidget);
    m_splitter->addWidget(m_playlist);
    m_splitter->setStretchFactor(0, 3);
    m_splitter->setStretchFactor(1, 1);

    auto *infoLayout = new QVBoxLayout;
    infoLayout->setSpacing(2);
    infoLayout->addWidget(m_titleLabel);
    infoLayout->addWidget(m_artistLabel);

    auto *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(m_openBtn);
    btnLayout->addWidget(m_playBtn);
    btnLayout->addStretch();

    mainLayout->addLayout(infoLayout);
    mainLayout->addLayout(btnLayout);
    mainLayout->addWidget(m_progressSlider);
    mainLayout->addWidget(m_splitter, 1);

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

    if (idx != m_currentLyricIndex) {
        m_currentLyricIndex = idx;
        if (idx >= 0 && idx < m_lyricsWidget->count()) {
            m_lyricsWidget->item(idx)->setSelected(true);
            m_lyricsWidget->scrollToItem(m_lyricsWidget->item(idx), QAbstractItemView::PositionAtCenter);
        }
        for (int i = 0; i < m_lyricsWidget->count(); ++i) {
            auto *item = m_lyricsWidget->item(i);
            QFont f = item->font();
            f.setBold(i == idx);
            item->setFont(f);
        }
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

void MainWindow::parseLrc(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QTextStream in(&file);
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
    file.close();
}

void MainWindow::parseVtt(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QTextStream in(&file);
    bool inHeader = true;
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (inHeader) {
            if (line.contains("-->")) inHeader = false;
            else continue;
        }
        if (line.isEmpty()) continue;

        if (line.contains("-->")) {
            QStringList parts = line.split("-->");
            if (parts.size() != 2) continue;

            auto parseTime = [](const QString &s) -> qint64 {
                QString t = s.trimmed();
                int h = 0, m = 0;
                double sec = 0;
                int c1 = t.indexOf(':');
                int c2 = t.lastIndexOf(':');
                if (c1 == c2) {
                    m = t.left(c1).toInt();
                    sec = t.mid(c1 + 1).toDouble();
                } else {
                    h = t.left(c1).toInt();
                    m = t.mid(c1 + 1, c2 - c1 - 1).toInt();
                    sec = t.mid(c2 + 1).toDouble();
                }
                return static_cast<qint64>(h * 3600000 + m * 60000 + sec * 1000);
            };

            qint64 startMs = parseTime(parts[0]);
            if (!in.atEnd()) {
                QString text = in.readLine().trimmed();
                m_lyricTimes.append(startMs);
                m_lyricTexts.append(text);
            }
        }
    }
    file.close();
}

void MainWindow::loadLyrics(const QString &audioPath)
{
    m_lyricTimes.clear();
    m_lyricTexts.clear();
    m_currentLyricIndex = -1;

    m_lyricsWidget->clear();

    QFileInfo fi(audioPath);
    QString baseDir = fi.absolutePath();
    QString baseName = fi.completeBaseName();

    QStringList candidates = {
        baseDir + "/" + baseName + ".lrc",
        baseDir + "/" + baseName + ".vtt",
        baseDir + "/" + baseName + ".zh.vtt",
        baseDir + "/" + baseName + ".zho.vtt",
    };

    bool loaded = false;
    for (const auto &candidate : candidates) {
        if (QFile::exists(candidate)) {
            if (candidate.endsWith(".lrc", Qt::CaseInsensitive)) {
                parseLrc(candidate);
            } else {
                parseVtt(candidate);
            }
            loaded = true;
            break;
        }
    }

    if (!loaded || m_lyricTimes.isEmpty()) {
        m_lyricsWidget->addItem("暂无歌词");
        return;
    }

    for (const auto &text : m_lyricTexts) {
        auto *item = new QListWidgetItem(text, m_lyricsWidget);
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    }

    if (!m_lyricTexts.isEmpty()) {
        m_lyricsWidget->item(0)->setSelected(true);
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
