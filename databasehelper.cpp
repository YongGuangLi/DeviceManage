#include "databasehelper.h"


#define SQL_AREADATA "select area_id,area_name from ws_area"
#define SQL_DEVICEDATA "select eqp_id,eqp_name,area_id,service_id,check_box from ws_eqp"

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

void DataBaseHelper::readDeviceDataFromDB()
{
    // if(isOpen())
    {
        QSqlQuery query(QString(SQL_DEVICEDATA));
        if(query.lastError().isValid())
        {
            qDebug()<<query.lastError().text();
            return;
        }
        while(query.next())
        {
            QString eqp_id =  query.value(0).toString();
            QString eqp_name =  query.value(1).toString();
            QString area_id = query.value(2).toString();
            QString service_id =  query.value(3).toString();
            QString check_box =  query.value(4).toString();
            //qDebug()<<eqp_id<<eqp_name<<area_id<<service_id<<check_box;
        }
    }
}

void DataBaseHelper::readAreaDataFromDB()
{
    {
        QSqlQuery query(QString(SQL_AREADATA));
        if(query.lastError().isValid())
        {
            qDebug()<<query.lastError().text();
            return;
        }
        while(query.next())
        {
            QString area_id =  query.value(0).toString();
            QString area_name =  query.value(1).toString();
            //qDebug()<<area_id<<area_name;
        }
    }
}

QString DataBaseHelper::getError()
{
    return sqlDatabase.lastError().text();
}


