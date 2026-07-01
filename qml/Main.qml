// zkw2024051604060
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import MusicPlayer

ApplicationWindow {
    id: window
    width: 780
    height: 560
    minimumWidth: 520
    minimumHeight: 400
    title: player.title !== "-" ? player.title + " - 音乐播放器" : "音乐播放器"
    visible: true
    color: "#0f0f1a"

    PlayerController { id: player }
    PlaylistModel { id: playlistModel }
    LyricsModel { id: lyricsModel }

    readonly property int modeLoop: 0
    readonly property int modeSingle: 1
    readonly property int modeShuffle: 2
    property int playMode: 0

    function formatTime(ms) {
        var sec = Math.floor(ms / 1000)
        var min = Math.floor(sec / 60)
        sec = sec % 60
        return min + ":" + (sec < 10 ? "0" : "") + sec
    }

    function hasMusic() {
        return player.title !== "-" || player.artist !== "-"
    }

    // ——— Connections ———

    Connections {
        target: playlistModel
        function onCurrentIndexChanged() {
            if (playlistModel.currentIndex < 0) {
                player.pause()
                return
            }
            player.source = playlistModel.currentFileUrl
            player.play()
        }
    }

    Connections {
        target: player
        function onSourceChanged() {
            lyricsModel.loadLyrics(player.source)
        }
        function onEnded() {
            if (playMode === modeSingle) {
                player.seek(0)
                player.play()
                return
            }
            if (playlistModel.count <= 1) return
            if (playMode === modeShuffle) {
                var ri = Math.floor(Math.random() * playlistModel.count)
                playlistModel.currentIndex = ri
            } else {
                var next = playlistModel.currentIndex + 1
                if (next >= playlistModel.count) next = 0
                playlistModel.currentIndex = next
            }
        }
    }

    Connections {
        target: player
        function onPositionChanged() {
            lyricsModel.updatePosition(player.position)
        }
    }

    Connections {
        target: lyricsModel
        function onCurrentIndexChanged() {
            if (lyricsModel.currentIndex >= 0)
                lyricsView.positionViewAtIndex(lyricsModel.currentIndex, ListView.Center)
        }
    }

    // ——— Drop Area ———

    DropArea {
        id: dropArea
        anchors.fill: parent
        onDropped: {
            var urls = []
            for (var i = 0; i < drop.urls.length; i++)
                urls.push(drop.urls[i])
            playlistModel.addFiles(urls)
        }

        // ——— Main Layout ———

        Item {
            id: mainContent
            anchors.fill: parent
            anchors.leftMargin: 24
            anchors.rightMargin: 24
            anchors.topMargin: 20
            anchors.bottomMargin: 16

            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                // ==== 顶部：标题/艺术家 ====
                RowLayout {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 52
                    spacing: 0

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 2

                        Label {
                            Layout.fillWidth: true
                            text: hasMusic() ? player.title : "音乐播放器"
                            font.pixelSize: 20
                            font.bold: true
                            color: "#f0f0f0"
                            elide: Text.ElideRight
                        }

                        Label {
                            Layout.fillWidth: true
                            text: hasMusic() ? player.artist : "拖入音频文件或点击按钮添加音乐"
                            font.pixelSize: 13
                            color: "#7f8c8d"
                            elide: Text.ElideRight
                        }
                    }

                    Button {
                        text: "☰"
                        flat: true
                        font.pixelSize: 18
                        onClicked: playlistPanel.visible = true
                        contentItem: Text {
                            text: parent.text
                            color: parent.hovered ? "#e94560" : "#aaa"
                            font.pixelSize: 18
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        background: Rectangle {
                            color: parent.hovered ? "#ffffff10" : "transparent"
                            radius: 8
                        }
                    }

                    Button {
                        text: "＋"
                        flat: true
                        font.pixelSize: 16
                        onClicked: fileDialog.open()
                        ToolTip.text: "添加音乐"
                        ToolTip.visible: hovered
                        contentItem: Text {
                            text: parent.text
                            color: parent.hovered ? "#e94560" : "#888"
                            font.pixelSize: 16
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        background: Rectangle {
                            color: parent.hovered ? "#ffffff10" : "transparent"
                            radius: 8
                        }
                    }
                }

                Item { Layout.preferredHeight: 4 }

                // ==== 中间：占位/拖放提示 ====
                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    visible: !hasMusic() || lyricsModel.rowCount() === 0

                    ColumnLayout {
                        anchors.centerIn: parent
                        spacing: 12

                        Rectangle {
                            Layout.alignment: Qt.AlignHCenter
                            width: 80
                            height: 80
                            radius: 40
                            color: "#ffffff0a"
                            border.color: "#ffffff15"
                            border.width: 1

                            Text {
                                anchors.centerIn: parent
                                text: "♬"
                                font.pixelSize: 32
                                color: hasMusic() ? "#555" : "#888"
                            }
                        }

                        Label {
                            Layout.alignment: Qt.AlignHCenter
                            text: hasMusic() ? "暂无歌词" : "拖入音乐文件 或 点击下方按钮添加"
                            font.pixelSize: 14
                            color: "#555"
                        }

                        Button {
                            visible: !hasMusic()
                            Layout.alignment: Qt.AlignHCenter
                            text: "打开文件"
                            onClicked: fileDialog.open()
                            contentItem: Text {
                                text: parent.text
                                color: "#fff"
                                font.pixelSize: 14
                                font.bold: true
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            background: Rectangle {
                                implicitWidth: 120
                                implicitHeight: 36
                                color: parent.hovered ? "#ff6b81" : "#e94560"
                                radius: 8
                            }
                        }
                    }
                }

                // ==== 中间：歌词区 ====
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    visible: hasMusic() && lyricsModel.rowCount() > 0
                    color: "#ffffff04"
                    radius: 14
                    border.color: "#ffffff08"
                    border.width: 1

                    ListView {
                        id: lyricsView
                        anchors.fill: parent
                        anchors.leftMargin: 4
                        anchors.rightMargin: 4
                        anchors.topMargin: 10
                        anchors.bottomMargin: 10
                        model: lyricsModel
                        clip: true
                        spacing: 6
                        flickDeceleration: 3000
                        maximumFlickVelocity: 1200

                        delegate: Item {
                            width: ListView.view.width
                            height: lyricText.implicitHeight + 8

                            Rectangle {
                                visible: model.isCurrent
                                anchors.fill: parent
                                anchors.leftMargin: 12
                                anchors.rightMargin: 12
                                color: "#e9456015"
                                radius: 8
                                border.color: "#e9456030"
                                border.width: 1
                            }

                            Text {
                                id: lyricText
                                anchors.centerIn: parent
                                width: parent.width - 40
                                text: model.lyricText
                                color: model.isCurrent ? "#ffffff" : "#4a4a5a"
                                font.pixelSize: model.isCurrent ? 22 : 15
                                font.bold: model.isCurrent
                                horizontalAlignment: Text.AlignHCenter
                                wrapMode: Text.Wrap
                                maximumLineCount: 3
                                elide: Text.ElideRight

                                Behavior on color {
                                    ColorAnimation { duration: 350; easing.type: Easing.OutCubic }
                                }
                                Behavior on font.pixelSize {
                                    NumberAnimation { duration: 350; easing.type: Easing.OutCubic }
                                }
                            }
                        }
                    }
                }

                Item { Layout.preferredHeight: 8 }

                // ==== 底部：进度条 ====
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 10

                    Label {
                        text: formatTime(player.position)
                        font.pixelSize: 11
                        color: "#666"
                        font.family: "monospace"
                    }

                    Slider {
                        id: progressSlider
                        Layout.fillWidth: true
                        from: 0
                        to: Math.max(player.duration, 1)
                        value: player.position
                        onMoved: player.seek(value)

                        background: Rectangle {
                            x: parent.leftPadding
                            y: parent.topPadding + parent.availableHeight / 2 - height / 2
                            implicitWidth: 200
                            implicitHeight: 4
                            width: parent.availableWidth
                            height: implicitHeight
                            radius: 2
                            color: "#ffffff15"

                            Rectangle {
                                width: parent.visualPosition * parent.width
                                height: parent.height
                                radius: 2
                                color: "#e94560"
                            }
                        }

                        handle: Rectangle {
                            x: parent.leftPadding + parent.visualPosition * (parent.availableWidth - width)
                            y: parent.topPadding + parent.availableHeight / 2 - height / 2
                            implicitWidth: 12
                            implicitHeight: 12
                            radius: 6
                            color: parent.pressed ? "#ff6b81" : "#e94560"
                            visible: parent.hovered || parent.pressed
                        }
                    }

                    Label {
                        text: formatTime(player.duration)
                        font.pixelSize: 11
                        color: "#666"
                        font.family: "monospace"
                    }
                }

                Item { Layout.preferredHeight: 12 }

                // ==== 底部：控制栏 ====
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 6

                    Button {
                        text: "⏮"
                        flat: true
                        font.pixelSize: 16
                        onClicked: playlistModel.currentIndex = Math.max(0, playlistModel.currentIndex - 1)
                    }

                    Button {
                        text: player.playing ? "⏸" : "▶"
                        flat: true
                        font.pixelSize: 24
                        onClicked: {
                            if (playlistModel.currentIndex < 0 && playlistModel.count > 0)
                                playlistModel.currentIndex = 0
                            else
                                player.toggle()
                        }
                    }

                    Button {
                        text: "⏭"
                        flat: true
                        font.pixelSize: 16
                        onClicked: playlistModel.currentIndex = Math.min(playlistModel.count - 1, playlistModel.currentIndex + 1)
                    }

                    Item { Layout.fillWidth: true }

                    Button {
                        text: "🔊"
                        flat: true
                        font.pixelSize: 14
                        onClicked: volumeSlider.value = volumeSlider.value === 0 ? 0.7 : 0
                    }

                    Slider {
                        id: volumeSlider
                        Layout.preferredWidth: 80
                        from: 0
                        to: 1
                        value: player.volume
                        onMoved: player.volume = value

                        background: Rectangle {
                            x: parent.leftPadding
                            y: parent.topPadding + parent.availableHeight / 2 - height / 2
                            implicitWidth: 200
                            implicitHeight: 3
                            width: parent.availableWidth
                            height: implicitHeight
                            radius: 2
                            color: "#ffffff10"

                            Rectangle {
                                width: parent.visualPosition * parent.width
                                height: parent.height
                                radius: 2
                                color: "#aaa"
                            }
                        }

                        handle: Rectangle {
                            x: parent.leftPadding + parent.visualPosition * (parent.availableWidth - width)
                            y: parent.topPadding + parent.availableHeight / 2 - height / 2
                            implicitWidth: 10
                            implicitHeight: 10
                            radius: 5
                            color: parent.pressed ? "#ddd" : "#ccc"
                            visible: parent.hovered || parent.pressed
                        }
                    }

                    Button {
                        text: "↻"
                        flat: true
                        font.pixelSize: 14
                        highlighted: playMode === modeLoop || playMode === modeSingle
                        onClicked: playMode = playMode === modeLoop ? modeSingle : modeLoop
                        contentItem: Text {
                            text: parent.text
                            color: playMode === modeSingle ? "#e94560" : (playMode === modeLoop ? "#ccc" : "#555")
                            font.pixelSize: 14
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }

                    Button {
                        text: "🔀"
                        flat: true
                        font.pixelSize: 14
                        onClicked: playMode = playMode === modeShuffle ? modeLoop : modeShuffle
                        contentItem: Text {
                            text: parent.text
                            color: playMode === modeShuffle ? "#e94560" : "#555"
                            font.pixelSize: 14
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                }

                // ==== 底部：专辑信息 ====
                RowLayout {
                    Layout.fillWidth: true
                    visible: player.album !== "-"

                    Label {
                        text: "专辑: " + player.album
                        font.pixelSize: 11
                        color: "#444"
                    }
                }
            }
        }

        // ==== 播放列表遮罩 ====
        Rectangle {
            id: playlistOverlay
            visible: playlistPanel.visible
            anchors.fill: parent
            color: "#00000060"
            z: 1

            Behavior on opacity {
                NumberAnimation { duration: 200; easing.type: Easing.OutCubic }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: playlistPanel.visible = false
            }
        }

        // ==== 播放列表面板 (右侧滑出) ====
        Rectangle {
            id: playlistPanel
            visible: false
            width: 280
            height: parent.height
            anchors.right: parent.right
            color: "#12121f"
            border.color: "#ffffff08"
            border.width: 1
            z: 2

            x: visible ? parent.width - width : parent.width
            Behavior on x {
                NumberAnimation { duration: 250; easing.type: Easing.OutCubic }
            }

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 12

                RowLayout {
                    Layout.fillWidth: true
                    Label {
                        text: "播放列表"
                        font.pixelSize: 16
                        font.bold: true
                        color: "#e0e0e0"
                    }
                    Label {
                        text: playlistModel.count + " 首"
                        font.pixelSize: 12
                        color: "#555"
                    }
                    Item { Layout.fillWidth: true }
                    Button {
                        text: "✕"
                        flat: true
                        font.pixelSize: 14
                        onClicked: playlistPanel.visible = false
                        contentItem: Text {
                            text: parent.text
                            color: parent.hovered ? "#fff" : "#888"
                            font.pixelSize: 14
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                }

                ListView {
                    id: playlistView
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    model: playlistModel
                    clip: true
                    spacing: 2

                    delegate: ItemDelegate {
                        width: ListView.view.width
                        highlighted: index === playlistModel.currentIndex
                        onClicked: {
                            playlistModel.currentIndex = index
                            playlistPanel.visible = false
                        }

                        contentItem: RowLayout {
                            spacing: 8
                            Rectangle {
                                Layout.preferredWidth: 4
                                Layout.preferredHeight: 28
                                radius: 2
                                color: index === playlistModel.currentIndex ? "#e94560" : "transparent"
                            }
                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 1
                                Label {
                                    Layout.fillWidth: true
                                    text: model.fileName
                                    font.pixelSize: 13
                                    color: index === playlistModel.currentIndex ? "#f0f0f0" : "#999"
                                    elide: Text.ElideRight
                                }
                            }
                        }

                        background: Rectangle {
                            color: {
                                if (index === playlistModel.currentIndex)
                                    return "#e9456012"
                                if (parent.hovered)
                                    return "#ffffff06"
                                return "transparent"
                            }
                            radius: 6
                        }
                    }
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
