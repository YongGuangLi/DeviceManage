#ifndef MAINWIDGET_H
#define MAINWIDGET_H


#include "databasehelper.h"
#include "configini.h"
#include "servicemanage.h"
#include "redisthread.h"
#include <QWidget>
#include <QDebug>
#include <QStandardItemModel>
#include <QGridLayout>
#include <QDateTime>
#include <QMessageBox>

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();
    void sendProcessCmd(QString, ProcessCmdType);
protected:
    bool eventFilter(QObject *, QEvent *);
private slots:
    void receiveProcessInfo(ProcessInfoMsg);
    void receiveDevicesStatus(DevicesStatusMsg);
    void receiveProcessAlarm(ProcessAlarmMsg);

    void showCtrlMenu(QPoint pos);
    void restartProcess();
    void stopProcess();
    void deleteProcess();
private:
    Ui::MainWidget *ui;
    ServiceManage *serviceManage_;
    QMap<ProcessStatusType,QString> mapServiceStatus_;
    QMap<DeviceStatusType,QString> mapDeviceStatus_;
};

#endif // MAINWIDGET_H
