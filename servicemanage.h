#ifndef SERVICEMANAGE_H
#define SERVICEMANAGE_H

#include <QWidget>
#include <QIcon>
#include <QMap>
#include <QStandardItemModel>
#include <QMenu>
#include <QAction>
#include <QStringList>
#include <QDebug>
#include <QUuid>
#include <QTreeWidgetItem>
#include <QMessageBox>


enum serviceNo
{
    Door,
    Camera,
    Location,
    Warning,
    Infrared,
    Speaker
};

namespace Ui {
class ServiceManage;
}

class ServiceManage : public QWidget
{
    Q_OBJECT
    
public:
    explicit ServiceManage(QWidget *parent = 0);
    ~ServiceManage();
    void modifyDeviceData();
private slots:
    void slotCustomContextMenu(const QPoint &);
    void treeViewClicked(QModelIndex index);
    void createServiceGroup();

    void modifyServiceGroupName();
    void serviceConfigFinish();
    void deleteServiceGroup();
private:
    Ui::ServiceManage *ui;
    QStandardItemModel* model;
    QStandardItem* itemProject;
    QMap<int,QIcon> publicIconMap_;
    QMap<serviceNo,QString> mapServiceName_;
    QMap<QString,QString> mapServiceID;
    QMap<QString,QString> mapServriceGroupID;
};

#endif // SERVICEMANAGE_H
