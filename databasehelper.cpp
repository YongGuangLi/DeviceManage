#include "databasehelper.h"


#define SQL_AREADATA "select area_id,area_name,level,parent_id from ws_area"
#define SQL_DEVICEDATA "select device_id,device_name,area_id,device_type,ip,port,device_username, \
password,service_id,checkable from ws_device"

#define SQL_MODIFYSERVICEID "UPDATE ws_device set SERVICE_ID = '%1', CHECKABLE = %2 WHERE DEVICE_ID = '%3'"

#define SQL_ADDSERVICE "insert into ws_service(SERVICE_ID,SERVICE_TYPE) value ('%1',%2)"

#define SQL_SERVICRDATA "select SERVICE_ID,SERVICE_TYPE,SERVICE_STATUS from ws_service"

#define SQL_DEVICECHECKABLE "update ws_device set CHECKABLE = %1 where DEVICE_ID = '%2'"

#define SQL_DELETESERVICE "delete from ws_service where SERVICE_ID = '%1'"

#define SQL_MODIFYSERVICE "update ws_service set service_status = %1 where service_id = '%2'"

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
    //把平台添加的设备类型和本程序的设备类型相对应
    mapDeviceType["DOOR"] = TYPE_ACCESSCTRL;
    mapDeviceType["INFRARED"] = TYPE_INFRARED;
    mapDeviceType["VIDEO"] = TYPE_CAMERA;
    mapDeviceType["ALARM"] = TYPE_ALARMLAMP;
    mapDeviceType["IPSOUND"] = TYPE_IPSOUND;
    mapDeviceType["LOCAL"] = TYPE_LOCATION;
    mapDeviceType["COUNT_CAMERA"] = TYPE_CAMERA_COUNT;

    QTimer *timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(checkConnect()));
    timer->start(10 * 1000);
}

bool DataBaseHelper::open(QString ip, int port, QString dbName, QString user, QString passwd)
{
    bool isopen = false;
    sqlDatabase = QSqlDatabase::addDatabase("QMYSQL");
    if(sqlDatabase.isValid())
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
        isopen = sqlDatabase.open();
        if(isopen)
        {
            qDebug()<<"Mysql Connect Success:"<<ip;
        }
    }
    return isopen;
}

void DataBaseHelper::readDeviceDataFromDB(QList<stDeviceData*>& listDeviceData)
{
    QSqlQuery query(QString(SQL_DEVICEDATA));
    if(query.lastError().isValid())
    {
        qWarning()<<query.lastError().text();
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
        int checkable =  query.value(9).toInt();

        stDeviceData *deviceData = new stDeviceData();
        deviceData->DeviceID_ = device_id;
        deviceData->DeviceName_ = device_name;
        deviceData->AreaID_ = area_id;
        deviceData->DeviceIp_ = ip;
        deviceData->DevicePort_ = port;
        deviceData->DeviceUser_ = device_username;
        deviceData->DevicePasswd = password;
        deviceData->ServiceID_ = service_id;
        deviceData->Checkable_ = checkable;
        deviceData->deviceType_ = mapDeviceType.value(device_type);

        listDeviceData.push_back(deviceData);
        //qDebug()<<device_id<<device_name<<area_id<<device_type<<ip<<port<<device_username<<password<<service_id<<checkable;
    }
}

//读取区域信息
void DataBaseHelper::readAreaDataFromDB(QMap<QString,stAreaData> &mapAreaData)
{
    QSqlQuery query(QString(SQL_AREADATA));
    if(query.lastError().isValid())
    {
        qWarning()<<query.lastError().text();
        return;
    }
    while(query.next())
    {
        stAreaData areaData;
        areaData.AreaID_ =  query.value(0).toString();
        areaData.AreadName_ =  query.value(1).toString();
        areaData.Level_ =  query.value(2).toInt();
        areaData.ParentAreaID_ = query.value(3).toString();

        mapAreaData[areaData.AreaID_] = areaData;
    }
}

bool DataBaseHelper::modifyDeviceServiceID(QString deviceID,int checkable, QString serviceID)
{
    QSqlQuery query(QString(SQL_MODIFYSERVICEID).arg(serviceID).arg(checkable).arg(deviceID));

    if(query.lastError().isValid())
    {
        qWarning()<<query.lastError().text();
        return false;
    }
    else
    {
        return true;
    }
}

bool DataBaseHelper::writeServiceDataToDB(QString serviceID, int serviceType)
{
    QSqlQuery query(QString(SQL_ADDSERVICE).arg(serviceID).arg(serviceType));

    if(query.lastError().isValid())
    {
        qWarning()<<query.lastError().text();
        return false;
    }
    else
    {
        return true;
    }
}

void DataBaseHelper::readServiceDataFromDB(QMap<DeviceType, QStringList> &mapServiceID, QMap<QString, int> &mapServiceStatus)
{
    QSqlQuery query(QString(SQL_SERVICRDATA));
    if(query.lastError().isValid())
    {
        qWarning()<<query.lastError().text();
        return;
    }
    while(query.next())
    {
        QString serviceID = query.value(0).toString();
        int serviceType = query.value(1).toInt();
        int serviceStatus = query.value(2).toInt();

        mapServiceID[DeviceType(serviceType)].push_back(serviceID);
        mapServiceStatus[serviceID] = serviceStatus;
    }
}

bool DataBaseHelper::modifyDeviceCheck(QString deviceID, int checkable)
{
    QSqlQuery query(QString(SQL_DEVICECHECKABLE).arg(checkable).arg(deviceID));
    if(query.lastError().isValid())
    {
        qWarning()<<query.lastError().text();
        return false;
    }
    else
    {
        return true;
    }
}

bool DataBaseHelper::deleteService(QString serviceID)
{
    bool status = false;
    QSqlQuery query(QString(SQL_DELETESERVICE).arg(serviceID));

    if(query.lastError().isValid())
    {
        qWarning()<<query.lastError().text();
    }
    else
    {
        status = true;
    }

    return status;
}

bool DataBaseHelper::modifyService(QString serviceID, int serviceStatus)
{
    bool status = false;
    QSqlQuery query(QString(SQL_MODIFYSERVICE).arg(serviceStatus).arg(serviceID));

    if(query.lastError().isValid())
    {
        qWarning()<<query.lastError().text();
    }
    else
    {
        status = true;
    }

    return status;
}

QString DataBaseHelper::getError()
{
    return sqlDatabase.lastError().text();
}

void DataBaseHelper::checkConnect()
{
    if(sqlDatabase.isOpen())
        return;

    if(sqlDatabase.open())
    {
        qDebug()<<"Mysql Reconnect Success:"<<ip_;
    }
    else
    {
        qDebug()<<"Mysql Reconnect Failure:"<<ip_;
        qDebug()<<getError();
    }
}


