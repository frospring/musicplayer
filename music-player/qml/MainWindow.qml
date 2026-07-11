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

    FileDialog {
        id: fileDialog
        title: "添加音频文件"
        nameFilters: ["音频文件 (*.mp3 *.wav *.flac *.ogg *.m4a)", "所有文件 (*)"]
        fileMode: FileDialog.OpenFiles
        onAccepted: playlistModel.addFiles(selectedFiles)
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 10

        Label {
            text: audioController.title || "未在播放"
            font.pixelSize: 22
            font.bold: true
            color: "#ffffff"
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            elide: Text.ElideRight
        }

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

        RowLayout {
            Layout.fillWidth: true
            spacing: 4

            Button {
                text: "\u2795 Add"
                font.pixelSize: 13
                onClicked: fileDialog.open()
            }

            Button {
                text: "\u2715 Del"
                font.pixelSize: 13
                enabled: playlistModel.count > 0
                onClicked: {
                    var idx = playlistModel.currentIndex
                    if (idx >= 0) {
                        var removedUrl = playlistModel.fileUrlAt(idx)
                        playlistModel.removeItem(idx)
                        if (audioController.source.toString() === removedUrl.toString())
                            audioController.stop()
                    }
                }
            }

            Button {
                text: "\u23FB Quit"
                font.pixelSize: 13
                onClicked: Qt.quit()
            }

            Item { Layout.fillWidth: true }

            Button {
                text: "\u25B6 Play"
                font.pixelSize: 13
                enabled: (audioController.hasMedia && !audioController.isPlaying) || (!audioController.hasMedia && playlistModel.count > 0)
                onClicked: {
                    if (!audioController.hasMedia && playlistModel.count > 0) {
                        playlistModel.currentIndex = 0
                        audioController.playFile(playlistModel.fileUrlAt(0))
                    } else {
                        audioController.play()
                    }
                }
            }

            Button {
                text: "\u23F8 Pause"
                font.pixelSize: 13
                enabled: audioController.isPlaying
                onClicked: audioController.pause()
            }

            Button {
                text: "\u23F9 Stop"
                font.pixelSize: 13
                enabled: audioController.hasMedia
                onClicked: audioController.stop()
            }
        }

        Slider {
            id: progressSlider
            Layout.fillWidth: true
            from: 0
            to: Math.max(audioController.duration, 1)
            value: progressSlider.pressed ? progressSlider.value : audioController.position
            onMoved: audioController.seek(value)
        }

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

        Label {
            visible: audioController.errorString !== ""
            text: "错误: " + audioController.errorString
            color: "#ff4444"
            font.pixelSize: 12
            Layout.fillWidth: true
            wrapMode: Text.Wrap
        }

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

    function formatTime(ms) {
        var totalSeconds = Math.floor(ms / 1000)
        var minutes = Math.floor(totalSeconds / 60)
        var seconds = totalSeconds % 60
        return minutes + ":" + (seconds < 10 ? "0" : "") + seconds
    }
}
