import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

ApplicationWindow {
    id: root
    width: 500
    height: 700
    title: "Music Player"
    visible: true
    color: "#1e1e1e"

    // 文件选择对话框
    FileDialog {
        id: fileDialog
        title: "选择音频文件"
        nameFilters: ["音频文件 (*.mp3 *.wav *.flac *.ogg *.m4a)", "所有文件 (*)"]
        fileMode: FileDialog.OpenFiles
        onAccepted: playlistModel.addFiles(selectedFiles)
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 10

        // 歌曲标题
        Label {
            text: audioController.title || "未在播放"
            font.pixelSize: 22
            font.bold: true
            color: "#ffffff"
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            elide: Text.ElideRight
        }

        // 歌手 - 专辑
        Label {
            text: {
                var parts = []
                if (audioController.artist) parts.push(audioController.artist)
                if (audioController.album) parts.push(audioController.album)
                return parts.join(" - ") || ""
            }
            font.pixelSize: 14
            color: "#aaaaaa"
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            elide: Text.ElideRight
        }

        // 控制按钮
        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Button {
                text: "打开文件"
                onClicked: fileDialog.open()
            }

            Item { Layout.fillWidth: true }

            Button {
                text: audioController.isPlaying ? "暂停" : "播放"
                onClicked: {
                    if (!audioController.hasMedia && playlistModel.count > 0) {
                        playlistModel.currentIndex = 0
                        audioController.playFile(playlistModel.fileUrlAt(0))
                    } else {
                        audioController.togglePlayPause()
                    }
                }
            }
        }

        // 进度条 (拖动时不绑定避免循环)
        Slider {
            id: progressSlider
            Layout.fillWidth: true
            from: 0
            to: Math.max(audioController.duration, 1)
            value: progressSlider.pressed ? progressSlider.value : audioController.position
            onMoved: audioController.seek(value)
        }

        // 时间标签
        RowLayout {
            Layout.fillWidth: true
            Label {
                text: formatTime(audioController.position)
                color: "#888888"
                font.pixelSize: 11
            }
            Item { Layout.fillWidth: true }
            Label {
                text: formatTime(audioController.duration)
                color: "#888888"
                font.pixelSize: 11
            }
        }

        // 错误提示
        Label {
            visible: audioController.errorString !== ""
            text: "错误: " + audioController.errorString
            color: "#ff4444"
            font.pixelSize: 12
            Layout.fillWidth: true
            wrapMode: Text.Wrap
        }

        // 播放列表
        Label {
            text: "播放列表"
            font.pixelSize: 14
            font.bold: true
            color: "#cccccc"
            Layout.topMargin: 4
        }

        ListView {
            id: playlistView
            Layout.fillWidth: true
            Layout.preferredHeight: 160
            model: playlistModel
            clip: true
            spacing: 2

            delegate: ItemDelegate {
                width: playlistView.width
                highlighted: index === playlistModel.currentIndex

                contentItem: Text {
                    text: fileName
                    color: index === playlistModel.currentIndex ? "#1db954" : "#dddddd"
                    font.bold: index === playlistModel.currentIndex
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }

                background: Rectangle {
                    color: index === playlistModel.currentIndex ? "#2a2a2a" : "transparent"
                    radius: 4
                }

                onClicked: {
                    playlistModel.currentIndex = index
                    audioController.playFile(filePath)
                }
            }
        }

        // 歌词
        Label {
            text: "歌词"
            font.pixelSize: 14
            font.bold: true
            color: "#cccccc"
            Layout.topMargin: 4
        }

        ListView {
            id: lyricsView
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: audioController.lyricModel
            clip: true
            spacing: 6

            delegate: Label {
                width: lyricsView.width
                text: lyricText
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: 15
                font.bold: index === audioController.currentLyricIndex
                color: index === audioController.currentLyricIndex ? "#1db954" : "#777777"
                padding: 4
                wrapMode: Text.Wrap
            }

            // 歌词自动滚动到当前行
            Connections {
                target: audioController
                function onCurrentLyricIndexChanged() {
                    if (audioController.currentLyricIndex >= 0) {
                        lyricsView.positionViewAtIndex(
                            audioController.currentLyricIndex, ListView.Center)
                    }
                }
            }
        }
    }

    // 毫秒 → mm:ss 格式化
    function formatTime(ms) {
        var totalSeconds = Math.floor(ms / 1000)
        var minutes = Math.floor(totalSeconds / 60)
        var seconds = totalSeconds % 60
        return minutes + ":" + (seconds < 10 ? "0" : "") + seconds
    }
}
