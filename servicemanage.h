#ifndef SERVICEMANAGE_H
#define SERVICEMANAGE_H

#include <QWidget>
#include <QIcon>
#include <QMap>
#include <QStandardItemModel>
#include <QMenu>
#include <QAction>
#include <QLabel>
#include <QStringList>
#include <QDebug>
#include <QPushButton>
#include <QUuid>
#include <QTreeWidgetItem>
#include <QGroupBox>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QMapIterator>
#include <QTextEdit>
#include <QTableWidget>
#include <QButtonGroup>
#include <QVBoxLayout>
#include "datatype.h"
#include "databasehelper.h"
#include "flowlayout.h"
#include "devicedatadisp.h"
#include "SafeManageMsg.pb.h"
#include "redisthread.h"
#include "configini.h"



namespace Ui {
class ServiceManage;
}

class ServiceManage : public QWidget
{
    Q_OBJECT
    
public:
    explicit ServiceManage(QWidget *parent = 0);
    ~ServiceManage();
    void initGroupBox();

    void initServiceData();

    void initAreaData();

    void recursionAreaData(int);  //递归显示区域数据

    void modifyDeviceData();

    void clearGroupBoxButton();

    void addDeviceItem(QString serviceId ,QObjectList listObject);   //服务节点添加设备节点

    QStandardItem* findDeviceItemByID(QString);        //通过设备ID,找到树节点

    stDeviceData *findDeviceDataByID(QString);         //通过设备ID,找到设备详细信息

    void dispDeviceData(QString);                      //展示设备详细信息

    void sendDeviceConfigMsg(QStandardItem *);         //服务配置完毕，发送设备信息

    void modifyServiceStatus(QString,int);             //修改服务状态

    QString getAreaNameById(QString);                  //通过区域ID找到区域名字
protected:
    bool eventFilter(QObject *, QEvent *);             //双击设备QGroup,选择服务

private slots:
    void slotCustomContextMenu(const QPoint &);        //右击菜单
    void serviceItemDoubleClicked(QModelIndex index);  //双击服务树下的设备节点,显示设备信息
    void areaItemDoubleClicked(QModelIndex);           //单击区域按键

    void createService();                       //新增服务节点
    void modifyServiceName();                   //修改服务节点名字
    void serviceConfigFinish();                 //配置完成，发送设备信息
    void deleteServiceItem();                   //删除树中的服务节点
    void deviceCheckChange(QStandardItem*);     //修改设备节点的复选框
    void selectServiceID(QTableWidgetItem *);   //为设备选择服务ID
    void deviceButtonClicked();                 //点击设备按键

    void receiveDeviceInitRequest(DeviceInitRequestMsg);
private:
    Ui::ServiceManage *ui;
    QStandardItemModel* serviceModel;

    QStandardItemModel* areaModel;


    QMap<DeviceType,QIcon> mapServiceIcon_;       //服务类型、服务类型图标
    QMap<DeviceType,QString> mapServiceTypeName_; //服务类型、服务类型名字
    QMap<QString,DeviceType> mapServiceType_;     //服务类型名字、服务类型
    QMap<DeviceType,QString> mapServicePrefix_;   //服务类型、新建服务名字前缀

    QMap<QString,stAreaData> mapAreaData;        //区域ID、区域信息

    QMap<DeviceType,QStringList> mapServiceID_;    //不同服务类型下所有服务ID
    QMap<QString, int> mapServiceStatus_;          //所有服务状态

    QMap<QString,QStandardItem*> mapServiceItem_;   //保存所有服务ID、树节点
    QMap<QString,QStandardItem*> mapAreaItem_;      //保存所有区域ID、树节点

    QList<stDeviceData*> listDeviceData_;          //所有设备信息,用指针便于修改值

    QList<QGroupBox*> listGroupBox_;               //保存GroupBox指针
    QMap<QString,DeviceType> mapGroupBoxTitle_;     //保存GroupBox名字对应设备类型

    QMap<QString, QPushButton*> mapDeviceButton;  //保存当前所选区域的设备按键，切换区域，清空

    QGroupBox *currentSelectDeviceGroup;          //当前选中设备组,用于选择服务ID之后获取设备信息

    QMap<QStandardItem*, stDeviceData*> mapDeviceItem_;   //保存所有设备树节点对应设备信息

};

#endif // SERVICEMANAGE_H
