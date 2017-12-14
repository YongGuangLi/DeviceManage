#include "databasehelper.h"


#define SQL_AREADATA "select area_id,area_name from ws_area"
#define SQL_DEVICEDATA "select device_id,device_name,area_id,device_type,ip,port,device_username, \
                        password,service_id,checkable from ws_device"

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

    mapDeviceType["DOOR"] = TYPE_ACCESSCTRL;
    mapDeviceType["INFRARED"] = TYPE_INFRARED;
    mapDeviceType["VIDEO"] = TYPE_CAMERA;
    mapDeviceType["ALARM"] = TYPE_ALARMLAMP;
    mapDeviceType["IPSOUND"] = TYPE_IPSOUND;
    mapDeviceType["LOCAL"] = TYPE_LOCATION;
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

void DataBaseHelper::readDeviceDataFromDB(QList<stDeviceData>& listDeviceData)
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
            QString device_id =  query.value(0).toString();
            QString device_name =  query.value(1).toString();
            QString area_id = query.value(2).toString();
            QString device_type =  query.value(3).toString();
            QString ip =  query.value(4).toString();
            QString port =  query.value(5).toString();
            QString device_username =  query.value(6).toString();
            QString password =  query.value(7).toString();
            QString service_id =  query.value(8).toString();
            QString checkable =  query.value(8).toString();

            stDeviceData deviceData;
            deviceData.DeviceID_ = device_id;
            deviceData.DeviceName_ = device_name;
            deviceData.AreaID_ = area_id;
            deviceData.DeviceIp_ = ip;
            deviceData.DevicePort_ = port;
            deviceData.DeviceUser_ = device_username;
            deviceData.DevicePasswd = password;
            deviceData.ServiceID_ = service_id;
            deviceData.Checkable_ = checkable;
            deviceData.serviceNo_ = mapDeviceType.value(device_type);

            listDeviceData.push_back(deviceData);
            //qDebug()<<device_id<<device_name<<area_id<<device_type<<ip<<port<<device_username<<password<<service_id<<checkable;
        }
    }
}

void DataBaseHelper::readAreaDataFromDB(QMap<QString,QString> &mapAreaData)
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
            mapAreaData[area_id] = area_name;
        }
    }
}

QString DataBaseHelper::getError()
{
    return sqlDatabase.lastError().text();
}


