#ifndef CONFIGINI_H
#define CONFIGINI_H

#include <QApplication>
#include <QObject>
#include <QFile>
#include <QDebug>
#include <QSettings>

#define SingletonConfig ConfigIni::GetInstance()

class ConfigIni : public QObject
{
    Q_OBJECT
public:
    static ConfigIni *GetInstance();

    QString getIpMySql() const;
    int getPortMySql() const;
    QString getUserMysql() const;
    QString getPasswdMysql() const;
    QString getDbName() const;

    QString getIpRedis() const;
    int getPortRedis() const;

    QString getPath() const;
private:
    explicit ConfigIni(QObject *parent = 0);
    static ConfigIni* configIni;

    QString ipMySql_;
    int portMySql_;
    QString userMysql_;
    QString passwdMysql_;
    QString dbName_;

    QString ipRedis_;
    int portRedis_;

    QString path_;

signals:
    
public slots:
    
};

#endif // CONFIGINI_H
