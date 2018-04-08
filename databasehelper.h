#ifndef DATABASEHELPER_H
#define DATABASEHELPER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlDriver>
#include <QDebug>
#include <QMap>
#include <QList>
#include <QSqlQuery>
#include "datatype.h"
#include "SafeManageMsg.pb.h"

#define SingletonDBHelper DataBaseHelper::GetInstance()

class DataBaseHelper : public QObject
{
    Q_OBJECT
public:
    static DataBaseHelper *GetInstance();
    bool open(QString ip,int port, QString dbName, QString user, QString passwd);

    void readDeviceDataFromDB(QList<stDeviceData *> &mapDeviceData);
    void readAreaDataFromDB(QMap<QString, stAreaData> &mapAreaData);
    bool modifyDeviceServiceID(QString deviceID, int checkable, QString serviceID);

    bool modifyDeviceCheck(QString deviceID, int checkable);

    bool writeServiceDataToDB(QString serviceID, int serviceType);
    void readServiceDataFromDB(QMap<DeviceType,QStringList> &mapServiceID,QMap<QString, int>&);
    bool deleteService(QString serviceID);

    bool modifyService(QString serviceID, int serviceStatus);

    QString getError();
private:
    explicit DataBaseHelper(QObject *parent = 0);
    static DataBaseHelper * dbHelp_;

    QSqlDatabase sqlDatabase;
    QString ip_;
    int port_;
    QString dbName_;
    QString user_;
    QString passwd_;
    QMap<QString,DeviceType> mapDeviceType;   //设备类型对应的服务类型
signals:
    
public slots:
    
};

#endif // DATABASEHELPER_H
