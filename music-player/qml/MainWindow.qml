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

    background: Rectangle {
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#1a1a2e" }
            GradientStop { position: 0.5; color: "#16213e" }
            GradientStop { position: 1.0; color: "#0f3460" }
        }
    }

    property real savedVolume: 1.0

    Shortcut {
        sequence: "Space"
        onActivated: audioController.togglePlayPause()
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        onWheel: function(wheel) {
            var delta = wheel.angleDelta.y / 120
            var newVol = Math.max(0, Math.min(100, audioController.volume * 100 + delta * 5))
            audioController.setVolume(newVol / 100)
        }
    }

    function playFirst() {
        playlistModel.currentIndex = 0
        audioController.playFile(playlistModel.fileUrlAt(0))
    }

    function formatTime(ms) {
        var t = Math.floor(ms / 1000)
        var m = Math.floor(t / 60)
        var s = t % 60
        return m + ":" + (s < 10 ? "0" : "") + s
    }

    function playPrevious() {
        if (playlistModel.count === 0) return
        var idx = playlistModel.currentIndex - 1
        if (idx < 0) idx = playlistModel.count - 1
        playlistModel.currentIndex = idx
        audioController.playFile(playlistModel.fileUrlAt(idx))
    }

    function playNext() {
        if (playlistModel.count === 0) return
        var idx = playlistModel.currentIndex + 1
        if (idx >= playlistModel.count) idx = 0
        playlistModel.currentIndex = idx
        audioController.playFile(playlistModel.fileUrlAt(idx))
    }

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

        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: titleText.height
            clip: true

            Label {
                id: titleText
                text: {
                    if (audioController.isPlaying)
                        return "正在播放"
                    if (!audioController.hasMedia)
                        return "未播放"
                    return "暂停播放"
                }
                font.pixelSize: 22
                font.bold: true
                color: "#ffffff"
            }

            SequentialAnimation on x {
                id: scrollAnim
                running: audioController.isPlaying
                           && titleText.width > parent.width
                           && titleText.visible
                loops: Animation.Infinite

                PauseAnimation { duration: 1500 }

                ScriptAction { script: titleText.x = parent.width }

                NumberAnimation {
                    target: titleText
                    property: "x"
                    from: parent.width
                    to: -titleText.width
                    duration: Math.max(4000, titleText.width * 9)
                }
            }
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

            Rectangle {
                color: "#5a5a5a"
                radius: 6
                height: fileRow.implicitHeight + 8
                width: fileRow.implicitWidth + 8

                Row {
                    id: fileRow
                    anchors.centerIn: parent
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
                            var i = playlistModel.currentIndex
                            if (i < 0) return
                            var url = playlistModel.fileUrlAt(i)
                            playlistModel.removeItem(i)
                            if (audioController.source.toString() === url.toString())
                                audioController.stop()
                        }
                    }

                    Button {
                        text: "\u23FB Quit"
                        font.pixelSize: 13
                        onClicked: Qt.quit()
                        background: Rectangle {
                            color: parent.down ? "#dd7777" : "#cc6666"
                            radius: 4
                        }
                        contentItem: Text {
                            text: parent.text
                            color: "#ffffff"
                            font: parent.font
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                }
            }

            Item { Layout.fillWidth: true }

            Rectangle {
                color: "#2a4a6a"
                radius: 6
                height: playRow.implicitHeight + 8
                width: playRow.implicitWidth + 8

                Row {
                    id: playRow
                    anchors.centerIn: parent
                    spacing: 4

                    Button {
                        text: "\u25B6 Play"
                        font.pixelSize: 13
                        enabled: (!audioController.hasMedia && playlistModel.count > 0)
                               || (audioController.hasMedia && !audioController.isPlaying)
                        onClicked: audioController.hasMedia ? audioController.play() : playFirst()
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
            }
        }

        Slider {
            id: progressSlider
            Layout.fillWidth: true
            from: 0
            to: Math.max(audioController.duration, 1)
            value: pressed ? value : audioController.position
            onMoved: audioController.seek(value)
        }

        RowLayout {
            Layout.fillWidth: true
            Label { text: formatTime(audioController.position); color: "#888888"; font.pixelSize: 11 }
            Item { Layout.fillWidth: true }
            Label { text: formatTime(audioController.duration);  color: "#888888"; font.pixelSize: 11 }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 6

            Button {
                text: "\u23EE Prev"
                font.pixelSize: 13
                enabled: playlistModel.count > 0
                onClicked: playPrevious()
            }

            Button {
                text: "Next \u23ED"
                font.pixelSize: 13
                enabled: playlistModel.count > 0
                onClicked: playNext()
            }

            Item { Layout.fillWidth: true }

            Label {
                id: volumeIcon
                text: audioController.volume > 0 ? "\uD83D\uDD0A" : "\uD83D\uDD07"
                font.pixelSize: 14

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (audioController.volume > 0) {
                            savedVolume = audioController.volume
                            audioController.setVolume(0)
                        } else {
                            audioController.setVolume(savedVolume)
                        }
                    }
                }
            }

            Slider {
                id: volumeSlider
                Layout.preferredWidth: 100
                from: 0
                to: 100
                value: audioController.volume * 100
                onMoved: audioController.setVolume(value / 100)
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

            Label {
                anchors.centerIn: parent
                text: "点击 \u2795 Add 按钮添加歌曲"
                font.pixelSize: 14
                color: "#666666"
                visible: playlistModel.count === 0
            }

            delegate: ItemDelegate {
                width: playlistView.width
                property bool cur: index === playlistModel.currentIndex

                contentItem: Text {
                    text: fileName
                    color: cur ? "#1db954" : "#dddddd"
                    font.bold: cur
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }
                background: Rectangle {
                    color: cur ? "#2a2a2a" : "transparent"
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
                    if (audioController.currentLyricIndex >= 0)
                        lyricsView.positionViewAtIndex(audioController.currentLyricIndex, ListView.Center)
                }
            }
        }
    }
}
