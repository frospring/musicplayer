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
            spacing: 6

            Button {
                onClicked: fileDialog.open()

                contentItem: Column {
                    spacing: 2
                    anchors.centerIn: parent
                    Label {
                        text: "\u2795"
                        font.pixelSize: 16
                        color: "#ffffff"
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                    Label {
                        text: "Add"
                        font.pixelSize: 10
                        color: "#cccccc"
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
            }

            Button {
                enabled: playlistModel.count > 0
                onClicked: {
                    if (playlistModel.currentIndex >= 0) {
                        playlistModel.removeItem(playlistModel.currentIndex)
                    }
                }

                contentItem: Column {
                    spacing: 2
                    anchors.centerIn: parent
                    Label {
                        text: "\u2715"
                        font.pixelSize: 16
                        color: parent.parent.enabled ? "#ffffff" : "#666666"
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                    Label {
                        text: "Del"
                        font.pixelSize: 10
                        color: parent.parent.enabled ? "#cccccc" : "#666666"
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
            }

            Item { Layout.fillWidth: true }

            Button {
                enabled: (audioController.hasMedia && !audioController.isPlaying) || (!audioController.hasMedia && playlistModel.count > 0)
                onClicked: {
                    if (!audioController.hasMedia && playlistModel.count > 0) {
                        playlistModel.currentIndex = 0
                        audioController.playFile(playlistModel.fileUrlAt(0))
                    } else {
                        audioController.play()
                    }
                }

                contentItem: Column {
                    spacing: 2
                    anchors.centerIn: parent
                    Label {
                        text: "\u25B6"
                        font.pixelSize: 18
                        color: parent.parent.enabled ? "#ffffff" : "#666666"
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                    Label {
                        text: "Play"
                        font.pixelSize: 11
                        color: parent.parent.enabled ? "#cccccc" : "#666666"
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
            }

            Button {
                enabled: audioController.isPlaying
                onClicked: audioController.pause()

                contentItem: Column {
                    spacing: 2
                    anchors.centerIn: parent
                    Label {
                        text: "\u23F8"
                        font.pixelSize: 18
                        color: parent.parent.enabled ? "#ffffff" : "#666666"
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                    Label {
                        text: "Pause"
                        font.pixelSize: 11
                        color: parent.parent.enabled ? "#cccccc" : "#666666"
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
            }

            Button {
                enabled: audioController.hasMedia
                onClicked: audioController.stop()

                contentItem: Column {
                    spacing: 2
                    anchors.centerIn: parent
                    Label {
                        text: "\u23F9"
                        font.pixelSize: 18
                        color: parent.parent.enabled ? "#ffffff" : "#666666"
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                    Label {
                        text: "Stop"
                        font.pixelSize: 11
                        color: parent.parent.enabled ? "#cccccc" : "#666666"
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
            }

            Button {
                onClicked: Qt.quit()

                contentItem: Column {
                    spacing: 2
                    anchors.centerIn: parent
                    Label {
                        text: "\u23FB"
                        font.pixelSize: 18
                        color: "#ffffff"
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                    Label {
                        text: "Quit"
                        font.pixelSize: 11
                        color: "#cccccc"
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
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
