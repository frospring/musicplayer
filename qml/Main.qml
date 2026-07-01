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
    color: "#f0f2f5"

    PlayerController { id: player }
    PlaylistModel { id: playlistModel }
    LyricsModel { id: lyricsModel }

    readonly property int modeLoop: 0
    readonly property int modeSingle: 1
    readonly property int modeShuffle: 2
    property int playMode: 0

    property string statusText: {
        if (playlistModel.count === 0)
            return "拖入音乐文件或点击下方按钮添加"
        if (player.title === "-" && player.artist === "-")
            return "加载中..."
        return player.artist !== "-" ? player.artist : "未知艺术家"
    }

    function formatTime(ms) {
        var sec = Math.floor(ms / 1000)
        var min = Math.floor(sec / 60)
        sec = sec % 60
        return min + ":" + (sec < 10 ? "0" : "") + sec
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
            anchors.leftMargin: 32
            anchors.rightMargin: 32
            anchors.topMargin: 24
            anchors.bottomMargin: 18

            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                // ==== 顶部 ====
                RowLayout {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 56
                    spacing: 0

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 12

                        Rectangle {
                            Layout.preferredWidth: 40
                            Layout.preferredHeight: 40
                            radius: 10
                            color: "#14b8a6"

                            Text {
                                anchors.centerIn: parent
                                text: "♬"
                                font.pixelSize: 18
                                color: "#fff"
                            }
                        }

                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 1

                            Label {
                                Layout.fillWidth: true
                                text: playlistModel.count > 0 && player.title !== "-" ? player.title : "音乐播放器"
                                font.pixelSize: 20
                                font.bold: true
                                color: "#1a1a2e"
                                elide: Text.ElideRight
                            }

                            Label {
                                Layout.fillWidth: true
                                text: statusText
                                font.pixelSize: 13
                                color: "#8899aa"
                                elide: Text.ElideRight
                            }
                        }
                    }

                    Button {
                        text: "＋"
                        flat: true
                        font.pixelSize: 18
                        onClicked: fileDialog.open()
                        contentItem: Text {
                            text: parent.text
                            color: parent.hovered ? "#0d9488" : "#14b8a6"
                            font.pixelSize: 18
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        background: Rectangle {
                            implicitWidth: 36
                            implicitHeight: 36
                            color: parent.hovered ? "#14b8a610" : "transparent"
                            radius: 8
                        }
                    }

                    Button {
                        text: "☰"
                        flat: true
                        font.pixelSize: 18
                        onClicked: playlistPanel.visible = true
                        contentItem: Text {
                            text: parent.text
                            color: parent.hovered ? "#0d9488" : "#666"
                            font.pixelSize: 18
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        background: Rectangle {
                            implicitWidth: 36
                            implicitHeight: 36
                            color: parent.hovered ? "#00000008" : "transparent"
                            radius: 8
                        }
                    }
                }

                Item { Layout.preferredHeight: 16 }

                // ==== 空状态 ====
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    visible: lyricsModel.rowCount() === 0
                    color: "#ffffff"
                    radius: 14
                    border.color: "#e8e8ec"
                    border.width: 1

                    ColumnLayout {
                        anchors.centerIn: parent
                        spacing: 16

                        Rectangle {
                            Layout.alignment: Qt.AlignHCenter
                            width: 72
                            height: 72
                            radius: 16
                            color: "#14b8a612"

                            Text {
                                anchors.centerIn: parent
                                text: "♬"
                                font.pixelSize: 28
                                color: "#14b8a6"
                            }
                        }

                        Label {
                            Layout.alignment: Qt.AlignHCenter
                            text: playlistModel.count === 0 ? "还没有添加音乐哦" : "暂无歌词"
                            font.pixelSize: 16
                            color: "#555"
                        }

                        Label {
                            Layout.alignment: Qt.AlignHCenter
                            visible: playlistModel.count === 0
                            text: "拖入文件 或 点击按钮开始"
                            font.pixelSize: 13
                            color: "#aaa"
                        }

                        Button {
                            visible: playlistModel.count === 0
                            Layout.alignment: Qt.AlignHCenter
                            text: "打开音乐文件"
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
                                implicitWidth: 140
                                implicitHeight: 40
                                color: parent.hovered ? "#0d9488" : "#14b8a6"
                                radius: 10
                            }
                        }
                    }
                }

                // ==== 歌词区 ====
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    visible: lyricsModel.rowCount() > 0
                    color: "#ffffff"
                    radius: 14
                    border.color: "#e8e8ec"
                    border.width: 1

                    ListView {
                        id: lyricsView
                        anchors.fill: parent
                        anchors.leftMargin: 4
                        anchors.rightMargin: 4
                        anchors.topMargin: 12
                        anchors.bottomMargin: 12
                        model: lyricsModel
                        clip: true
                        spacing: 8
                        flickDeceleration: 3000
                        maximumFlickVelocity: 1200

                        delegate: Item {
                            width: ListView.view.width
                            height: lyricText.implicitHeight + 10

                            Rectangle {
                                visible: model.isCurrent
                                anchors.fill: parent
                                anchors.leftMargin: 20
                                anchors.rightMargin: 20
                                color: "#14b8a60c"
                                radius: 10
                            }

                            Text {
                                id: lyricText
                                anchors.centerIn: parent
                                width: parent.width - 60
                                text: model.lyricText
                                color: model.isCurrent ? "#14b8a6" : "#bbb"
                                font.pixelSize: model.isCurrent ? 24 : 16
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

                Item { Layout.preferredHeight: 12 }

                // ==== 进度条 ====
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 10

                    Label {
                        text: formatTime(player.position)
                        font.pixelSize: 11
                        color: "#999"
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
                            color: "#e8e8ec"

                            Rectangle {
                                width: parent.visualPosition * parent.width
                                height: parent.height
                                radius: 2
                                color: "#14b8a6"
                            }
                        }

                        handle: Rectangle {
                            x: parent.leftPadding + parent.visualPosition * (parent.availableWidth - width)
                            y: parent.topPadding + parent.availableHeight / 2 - height / 2
                            implicitWidth: 14
                            implicitHeight: 14
                            radius: 7
                            color: parent.pressed ? "#0d9488" : "#14b8a6"
                            visible: parent.hovered || parent.pressed
                        }
                    }

                    Label {
                        text: formatTime(player.duration)
                        font.pixelSize: 11
                        color: "#999"
                        font.family: "monospace"
                    }
                }

                Item { Layout.preferredHeight: 14 }

                // ==== 控制栏 ====
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 8

                    Button {
                        text: "⏮"
                        flat: true
                        font.pixelSize: 16
                        onClicked: playlistModel.currentIndex = Math.max(0, playlistModel.currentIndex - 1)
                        contentItem: Text {
                            text: parent.text
                            color: parent.hovered ? "#14b8a6" : "#666"
                            font.pixelSize: 16
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        background: Rectangle {
                            implicitWidth: 36
                            implicitHeight: 36
                            color: parent.hovered ? "#00000006" : "transparent"
                            radius: 18
                        }
                    }

                    Button {
                        text: player.playing ? "⏸" : "▶"
                        flat: true
                        font.pixelSize: 26
                        onClicked: {
                            if (playlistModel.currentIndex < 0 && playlistModel.count > 0)
                                playlistModel.currentIndex = 0
                            else
                                player.toggle()
                        }
                        contentItem: Text {
                            text: parent.text
                            color: "#14b8a6"
                            font.pixelSize: 26
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        background: Rectangle {
                            implicitWidth: 44
                            implicitHeight: 44
                            color: parent.hovered ? "#14b8a612" : "#14b8a60c"
                            radius: 22
                        }
                    }

                    Button {
                        text: "⏭"
                        flat: true
                        font.pixelSize: 16
                        onClicked: playlistModel.currentIndex = Math.min(playlistModel.count - 1, playlistModel.currentIndex + 1)
                        contentItem: Text {
                            text: parent.text
                            color: parent.hovered ? "#14b8a6" : "#666"
                            font.pixelSize: 16
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        background: Rectangle {
                            implicitWidth: 36
                            implicitHeight: 36
                            color: parent.hovered ? "#00000006" : "transparent"
                            radius: 18
                        }
                    }

                    Item { Layout.fillWidth: true }

                    Button {
                        text: "🔊"
                        flat: true
                        font.pixelSize: 14
                        onClicked: volumeSlider.value = volumeSlider.value === 0 ? 0.7 : 0
                        contentItem: Text {
                            text: parent.text
                            color: parent.hovered ? "#14b8a6" : "#888"
                            font.pixelSize: 14
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        background: Rectangle {
                            implicitWidth: 32
                            implicitHeight: 32
                            color: parent.hovered ? "#00000006" : "transparent"
                            radius: 16
                        }
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
                            color: "#e8e8ec"

                            Rectangle {
                                width: parent.visualPosition * parent.width
                                height: parent.height
                                radius: 2
                                color: "#14b8a6"
                            }
                        }

                        handle: Rectangle {
                            x: parent.leftPadding + parent.visualPosition * (parent.availableWidth - width)
                            y: parent.topPadding + parent.availableHeight / 2 - height / 2
                            implicitWidth: 10
                            implicitHeight: 10
                            radius: 5
                            color: parent.pressed ? "#0d9488" : "#14b8a6"
                            visible: parent.hovered || parent.pressed
                        }
                    }

                    Button {
                        text: "↻"
                        flat: true
                        font.pixelSize: 14
                        onClicked: playMode = playMode === modeLoop ? modeSingle : modeLoop
                        contentItem: Text {
                            text: parent.text
                            color: {
                                if (playMode === modeSingle) return "#14b8a6"
                                if (playMode === modeLoop) return "#555"
                                return "#ccc"
                            }
                            font.pixelSize: 14
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        background: Rectangle {
                            implicitWidth: 32
                            implicitHeight: 32
                            color: parent.hovered ? "#00000006" : "transparent"
                            radius: 16
                        }
                    }

                    Button {
                        text: "🔀"
                        flat: true
                        font.pixelSize: 14
                        onClicked: playMode = playMode === modeShuffle ? modeLoop : modeShuffle
                        contentItem: Text {
                            text: parent.text
                            color: playMode === modeShuffle ? "#14b8a6" : "#ccc"
                            font.pixelSize: 14
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        background: Rectangle {
                            implicitWidth: 32
                            implicitHeight: 32
                            color: parent.hovered ? "#00000006" : "transparent"
                            radius: 16
                        }
                    }
                }
            }
        }

        // ==== 播放列表遮罩 ====
        Rectangle {
            id: playlistOverlay
            visible: playlistPanel.visible
            anchors.fill: parent
            color: "#00000040"
            z: 1

            Behavior on opacity {
                NumberAnimation { duration: 200 }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: playlistPanel.visible = false
            }
        }

        // ==== 播放列表面板 ====
        Rectangle {
            id: playlistPanel
            visible: false
            width: 280
            height: parent.height
            anchors.right: parent.right
            color: "#ffffff"
            border.color: "#e8e8ec"
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
                        color: "#1a1a2e"
                    }
                    Label {
                        text: playlistModel.count + " 首"
                        font.pixelSize: 12
                        color: "#aaa"
                    }
                    Item { Layout.fillWidth: true }
                    Button {
                        text: "✕"
                        flat: true
                        font.pixelSize: 14
                        onClicked: playlistPanel.visible = false
                        contentItem: Text {
                            text: parent.text
                            color: parent.hovered ? "#333" : "#aaa"
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
                        onClicked: {
                            playlistModel.currentIndex = index
                            playlistPanel.visible = false
                        }

                        contentItem: RowLayout {
                            spacing: 8
                            Rectangle {
                                Layout.preferredWidth: 3
                                Layout.preferredHeight: 24
                                radius: 2
                                color: index === playlistModel.currentIndex ? "#14b8a6" : "transparent"
                            }
                            Label {
                                Layout.fillWidth: true
                                text: model.fileName
                                font.pixelSize: 13
                                color: index === playlistModel.currentIndex ? "#14b8a6" : "#555"
                                elide: Text.ElideRight
                            }
                        }

                        background: Rectangle {
                            color: {
                                if (index === playlistModel.currentIndex)
                                    return "#14b8a610"
                                if (parent.hovered)
                                    return "#00000004"
                                return "transparent"
                            }
                            radius: 8
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
