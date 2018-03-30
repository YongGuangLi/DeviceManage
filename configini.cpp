#include "configini.h"

ConfigIni* ConfigIni::configIni = NULL;

ConfigIni *ConfigIni::GetInstance()
{
    if(configIni == NULL)
    {
        configIni = new ConfigIni();
    }
    return configIni;
}

ConfigIni::ConfigIni(QObject *parent) : QObject(parent)
{
    if(QFile::exists(QApplication::applicationDirPath() + "/config.ini"))
    {
        QSettings* settings_ = new QSettings(QApplication::applicationDirPath() + "/config.ini",QSettings::IniFormat);
        settings_->setIniCodec("UTF-8");

        settings_->beginGroup("MYSQL");
        ipMySql_ =  settings_->value("ip").toString();
        portMySql_ =  settings_->value("port").toInt();
        userMysql_ =  settings_->value("user").toString();
        passwdMysql_ =  settings_->value("passwd").toString();
        dbName_ = settings_->value("dbName").toString();
        settings_->endGroup();

        settings_->beginGroup("REDIS");
        ipRedis_ = settings_->value("ip").toString();
        portRedis_ = settings_->value("port").toInt();
        settings_->endGroup();

        settings_->beginGroup("PATH");
        path_ = settings_->value("path").toString();
        settings_->endGroup();
    }
    else
    {
        qWarning()<<QCoreApplication::applicationDirPath() + "/config.ini" + " not exist";
    }
}

QString ConfigIni::getDbName() const
{
    return dbName_;
}


QString ConfigIni::getIpMySql() const
{
    return ipMySql_;
}

int ConfigIni::getPortMySql() const
{
    return portMySql_;
}

QString ConfigIni::getUserMysql() const
{
    return userMysql_;
}

QString ConfigIni::getPasswdMysql() const
{
    return passwdMysql_;
}

QString ConfigIni::getIpRedis() const
{
    return ipRedis_;
}

int ConfigIni::getPortRedis() const
{
    return portRedis_;
}

QString ConfigIni::getPath() const
{
    return path_;
}
