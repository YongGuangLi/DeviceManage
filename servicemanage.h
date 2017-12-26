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

    void modifyDeviceData();

    void clearGroupBoxButton();

    void addDeviceItem(QString serviceId ,QObjectList listObject);   //服务节点添加设备节点

    QStandardItem* findDeviceItemByID(QString);        //通过设备ID,找到树节点

    stDeviceData *findDeviceDataByID(QString);         //通过设备ID,找到设备详细信息

    void dispDeviceData(QString);                      //展示设备详细信息
protected:
    bool eventFilter(QObject *, QEvent *);             //双击设备QGroup,选择服务

private slots:
    void slotCustomContextMenu(const QPoint &);     //右击菜单
    void treeViewDoubleClicked(QModelIndex index);  //双击树节点

    void createService();                       //新增服务节点
    void modifyServiceName();                   //修改服务节点名字
    void serviceConfigFinish();                 //配置完成，发送设备信息
    void deleteServiceItem();                   //删除树中的服务节点
    void deviceCheckChange(QStandardItem*);     //修改设备节点的复选框
    void selectServiceID(QTableWidgetItem *);   //为设备选择服务ID
    void areaButtonClicked();                   //单击区域按键
    void deviceButtonClicked();                 //点击设备按键


    void on_pushButton_clicked();

private:
    Ui::ServiceManage *ui;
    QStandardItemModel* model;
    QStandardItem* itemProject;

    QMap<ServiceNo,QIcon> mapServiceIcon_;       //服务类型、服务图标
    QMap<ServiceNo,QString> mapServiceName_;     //服务类型、服务名字
    QMap<ServiceNo,QString> mapServicePrefix_;   //服务类型、新建服务名字前缀
    QMap<QString,QString> mapAreaData;             //区域id、区域描述

    QMap<ServiceNo,QStringList> mapServiceID_;     //所有服务ID
    QMap<QString,QStandardItem*> mapServiceItem_;  //保存所有服务的树节点

    QList<stDeviceData*> listDeviceData_;          //所有设备

    QList<QGroupBox*> listQGroupBox_;             //保存GroupBox指针，index对应设备类型枚举
    QMap<QString,ServiceNo> mapGroupBoxName_;     //保存GroupBox名字对应设备类型

    QMap<QString,bool> mapDeviceStatus;           //设备是否选择服务
    QMap<QString, QPushButton*> mapDeviceButton;  //保存当前所选区域的设备

    QGroupBox *currentSelectDeviceGroup;          //当前选中设备组,用于选择服务ID之后获取设备信息

    QMap<QStandardItem*, stDeviceData*> mapDeviceItem_;   //保存所有设备树节点信息
};

#endif // SERVICEMANAGE_H
