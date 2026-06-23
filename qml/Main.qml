// zkw2024051604060
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import MusicPlayer

ApplicationWindow {
    id: window
    width: 700
    height: 500
    title: "音乐播放器"
    visible: true

    PlayerController { id: player }
    PlaylistModel { id: playlistModel }
    LyricsModel { id: lyricsModel }

    function formatTime(ms) {
        var sec = Math.floor(ms / 1000)
        var min = Math.floor(sec / 60)
        sec = sec % 60
        return min + ":" + (sec < 10 ? "0" : "") + sec
    }

    // 当播放列表切换时，设置播放源并自动播放
    Connections {
        target: playlistModel
        function onCurrentIndexChanged() {
            if (playlistModel.currentIndex < 0) return
            var idx = playlistModel.index(playlistModel.currentIndex, 0)
            var path = playlistModel.data(idx, PlaylistModel.FilePathRole)
            player.source = "file://" + path
            player.play()
        }
    }

    // 切歌时加载歌词
    Connections {
        target: player
        function onSourceChanged() {
            lyricsModel.loadLyrics(player.source, player.embeddedLyrics)
        }
    }

    // positionChanged 触发歌词行更新
    Connections {
        target: player
        function onPositionChanged() {
            lyricsModel.updatePosition(player.position)
        }
    }

    // 当前歌词行变化时滚动到居中
    Connections {
        target: lyricsModel
        function onCurrentIndexChanged() {
            if (lyricsModel.currentIndex >= 0)
                lyricsView.positionViewAtIndex(lyricsModel.currentIndex, ListView.Center)
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 6

        // 工具栏
        RowLayout {
            Button {
                text: "打开文件"
                onClicked: fileDialog.open()
            }
            Item { Layout.fillWidth: true }
        }

        // ID3标签审查区
        GridLayout {
            columns: 4
            columnSpacing: 12
            rowSpacing: 2
            Label { text: "标题:" }
            Label { text: player.title; Layout.fillWidth: true }
            Label { text: "艺术家:" }
            Label { text: player.artist; Layout.fillWidth: true }
            Label { text: "专辑:" }
            Label { text: player.album; Layout.fillWidth: true }
            Label { text: "内嵌歌词:" }
            Label { text: player.hasEmbeddedLyrics ? "有" : "无"; Layout.fillWidth: true }
        }

        // 播放控制栏
        RowLayout {
            Button {
                text: "⏮"
                onClicked: playlistModel.currentIndex = Math.max(0, playlistModel.currentIndex - 1)
            }
            Button {
                text: player.playing ? "⏸" : "▶"
                onClicked: {
                    if (playlistModel.currentIndex < 0 && playlistModel.count > 0)
                        playlistModel.currentIndex = 0
                    else
                        player.toggle()
                }
            }
            Button {
                text: "⏭"
                onClicked: playlistModel.currentIndex = Math.min(playlistModel.count - 1, playlistModel.currentIndex + 1)
            }

            Slider {
                id: progressSlider
                Layout.fillWidth: true
                from: 0
                to: player.duration
                value: player.position
                onMoved: player.seek(value)
            }

            Label {
                text: formatTime(player.position) + " / " + formatTime(player.duration)
            }
        }

        // 歌词+播放列表 (上下分屏)
        SplitView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ListView {
                id: lyricsView
                SplitView.fillWidth: true
                SplitView.fillHeight: true
                model: lyricsModel
                clip: true
                spacing: 4

                delegate: Text {
                    width: ListView.view.width
                    text: model.lyricText
                    color: model.isCurrent ? "#fff" : "#888"
                    font.bold: model.isCurrent
                    font.pixelSize: model.isCurrent ? 18 : 14
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.Wrap
                }
            }

            ListView {
                id: playlistView
                SplitView.fillWidth: true
                SplitView.fillHeight: true
                model: playlistModel
                clip: true

                delegate: ItemDelegate {
                    width: ListView.view.width
                    text: model.fileName
                    highlighted: index === playlistModel.currentIndex
                    onClicked: playlistModel.currentIndex = index
                }
            }
        }
    }

    FileDialog {
        id: fileDialog
        title: "选择音乐文件"
        nameFilters: ["音频文件 (*.mp3 *.wav *.flac *.ogg *.m4a)"]
        fileMode: FileDialog.OpenFiles
        onAccepted: playlistModel.addFiles(selectedFiles)
    }
}
