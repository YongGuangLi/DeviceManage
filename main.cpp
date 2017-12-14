#include "mainwidget.h"
#include "stylehelper.h"
#include <QApplication>
#include <QTextCodec>
#include <QMutex>
#include <QDateTime>

QFile g_file;
void myMessageOutput(QtMsgType type,  const char *msg)
{
    Q_UNUSED(type);
    static QMutex mutex;
    mutex.lock();

    QString strDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString strMessage = QString("%1 %2").arg(strDateTime).arg(msg);

    g_file.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text);
    QTextStream stream(&g_file);
    stream << strMessage<< "\n";
    g_file.flush();
    g_file.close();
    mutex.unlock();
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    StyleHelper::setStyle(":/qssStyle/whiteStyle.qss");

    QTextCodec *codec=QTextCodec::codecForName("utf-8");
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);

    g_file.setFileName(QCoreApplication::applicationDirPath() + QString("/log%1.txt").arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss")));
    //qInstallMsgHandler(myMessageOutput);

    MainWidget w;
    w.show();

    return a.exec();
}
