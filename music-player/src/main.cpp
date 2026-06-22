#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "appcontext.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // 创建应用上下文 (组装所有 C++ 模块)
    AppContext ctx;

    // 初始化 QML 引擎并加载界面
    QQmlApplicationEngine engine;
    ctx.initialize(engine);

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
