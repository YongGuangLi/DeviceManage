#include "mainwidget.h"
#include "stylehelper.h"
#include "win32fixes.h"
#include "log4qt/logmanager.h"
#include "log4qt/propertyconfigurator.h"
#include <QApplication>
#include <QTextCodec>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "hiredis.lib")

int main(int argc, char *argv[])
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,1) , &wsaData);

    QApplication a(argc, argv);

    StyleHelper::setStyle(":/qssStyle/whiteStyle.qss");

    QTextCodec *codec=QTextCodec::codecForName("utf-8");
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);

    Log4Qt::PropertyConfigurator::configure(a.applicationDirPath()+ "/log4qt.properties");
    Log4Qt::LogManager::setHandleQtMessages(true);

    MainWidget w;
    w.show();

    return a.exec();
}
