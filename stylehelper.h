#ifndef STYLEHELPER_H
#define STYLEHELPER_H

#include <QFile>
#include <QApplication>
#include <QDebug>

class StyleHelper
{
public:
    static void setStyle(const QString &style)
    {
        QFile qss(style);
        if(qss.open(QFile::ReadOnly))
        {
            QString style;
            QTextStream in(&qss);
            QString line = in.readLine();
            while (!line.isNull())
            {
                style += line;
                line = in.readLine();
            }
            qApp->setStyleSheet(style);
            qss.close();
        }
    }
};

#endif // STYLEHELPER_H
