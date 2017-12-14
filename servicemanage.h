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
#include <QMapIterator>
#include "datatype.h"
#include "databasehelper.h"
#include "flowlayout.h"



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
protected:
    bool eventFilter(QObject *, QEvent *);

private slots:
    void slotCustomContextMenu(const QPoint &);
    void treeViewClicked(QModelIndex index);
    void createService();

    void modifyServiceGroupName();
    void serviceConfigFinish();
    void deleteServiceGroup();

    void areaButtonClicked();
private:
    Ui::ServiceManage *ui;
    QStandardItemModel* model;
    QStandardItem* itemProject;

    QMap<ServiceNo,QIcon> mapServiceIcon_;       //服务类型、服务图标
    QMap<ServiceNo,QString> mapServiceName_;     //服务类型、服务名字

    QMap<QString,QString> mapAreaData;           //区域id、区域描述

    QMap<ServiceNo,QStringList> mapServiceID_;   //一种服务类型下所有服务ID

    QList<stDeviceData> listDeviceData;

    QMap<ServiceNo,QList<stDeviceData> > listDeviceData_;

    QList<QGroupBox*> listQGroupBox;
};

#endif // SERVICEMANAGE_H
