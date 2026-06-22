#ifndef APPCONTEXT_H
#define APPCONTEXT_H

#include <QObject>

class QQmlApplicationEngine;
class AudioEngine;
class AudioController;
class PlaylistModel;
class LyricModel;
class FileManager;

// 应用上下文 - 组装所有模块并初始化 QML 引擎
// 职责: 依赖注入、对象生命周期管理、QML 上下文暴露
class AppContext : public QObject
{
    Q_OBJECT

public:
    explicit AppContext(QObject *parent = nullptr);
    ~AppContext() override = default;

    // 初始化 QML 引擎, 将各模块暴露给 QML 上下文
    void initialize(QQmlApplicationEngine &engine);

private:
    // 模块实例 (按依赖顺序声明)
    AudioEngine    *m_audioEngine;
    FileManager    *m_fileManager;
    LyricModel     *m_lyricModel;
    AudioController *m_audioController;
    PlaylistModel  *m_playlistModel;
};

#endif
