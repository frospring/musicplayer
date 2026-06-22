#include "appcontext.h"
#include "audioengine.h"
#include "audiocontroller.h"
#include "playlistmodel.h"
#include "lyricmodel.h"
#include "filemanager.h"

#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDebug>

AppContext::AppContext(QObject *parent)
    : QObject(parent)
{
    // 按依赖顺序创建模块
    // 1. 底层引擎 (无依赖)
    m_audioEngine = new AudioEngine(this);

    // 2. 文件管理 (无依赖)
    m_fileManager = new FileManager(this);

    // 3. 歌词模型 (无依赖)
    m_lyricModel = new LyricModel(this);

    // 4. 播放控制器 (依赖: audioEngine, lyricModel)
    m_audioController = new AudioController(m_audioEngine, m_lyricModel, this);

    // 5. 播放列表模型 (无依赖)
    m_playlistModel = new PlaylistModel(this);
}

void AppContext::initialize(QQmlApplicationEngine &engine)
{
    // 将 C++ 对象暴露给 QML 上下文
    // QML 中通过 audioController / playlistModel 访问
    engine.rootContext()->setContextProperty("audioController", m_audioController);
    engine.rootContext()->setContextProperty("playlistModel", m_playlistModel);

    // 从 Qt 资源系统加载 QML 界面
    const QUrl qmlUrl(QStringLiteral("qrc:/qml/MainWindow.qml"));

    // 连接错误信号以便诊断加载失败原因
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &engine, [](const QUrl &url) {
                         qWarning() << "QML 对象创建失败:" << url.toString();
                     }, Qt::QueuedConnection);

    engine.load(qmlUrl);
    if (engine.rootObjects().isEmpty()) {
        qCritical("无法加载 QML 界面: %s", qPrintable(qmlUrl.toString()));
    }
}
