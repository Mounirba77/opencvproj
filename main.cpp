#include <QGuiApplication>
#include <QQmlApplicationEngine>
//videocapture class inclusion
//main.cpp has the role of connecting qml front to c++ back

#include "videocapture.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);
    //linking videocapture class to qml as a customcomponent
    qmlRegisterType<VideoCapture>("CustomComponents", 1, 0, "VideoCapture");
    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
