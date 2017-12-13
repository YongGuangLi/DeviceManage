#include "databasehelper.h"


DataBaseHelper * DataBaseHelper::dbHelp_ = NULL;

DataBaseHelper *DataBaseHelper::GetInstance()
{
    if(dbHelp_ == NULL)
    {
      dbHelp_ = new DataBaseHelper();
    }
    return dbHelp_;
}


DataBaseHelper::DataBaseHelper(QObject *parent) :
    QObject(parent)
{
    sqlDatabase = QSqlDatabase::addDatabase("QMYSQL");
}

bool DataBaseHelper::open(QString ip, int port, QString dbName, QString user, QString passwd)
{
    sqlDatabase.setHostName(ip);
    sqlDatabase.setPort(port);
    sqlDatabase.setDatabaseName(dbName);
    sqlDatabase.setUserName(user);
    sqlDatabase.setPassword(passwd);
    ip_ = ip;
    port_ = port;
    dbName_ = dbName;
    user_ = user;
    passwd_ = passwd;
    bool isopen = sqlDatabase.open();
    if(isopen)
    {
        qDebug()<<"Mysql Connect Success:"<<ip;
    }
    else
    {
         qDebug()<<"Mysql Connect Fail:"<<ip;
    }
    return isopen;
}

QString DataBaseHelper::getError()
{
    return sqlDatabase.lastError().text();
}


