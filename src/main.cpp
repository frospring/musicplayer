// zkw2024051604060
#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.loadFromModule("MusicPlayer", "Main");

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
