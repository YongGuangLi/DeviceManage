#include "mainwidget.h"
#include "ui_mainwidget.h"

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);

    this->showMaximized();
    this->setWindowTitle(QString::fromLocal8Bit("硬件配置管理"));

    //连接mysql
    if(!SingletonDBHelper->open(SingletonConfig->getIpMySql(),SingletonConfig->getPortMySql(),SingletonConfig->getDbName(), SingletonConfig->getUserMysql(),SingletonConfig->getPasswdMysql()))
    {
        qWarning()<<SingletonDBHelper->getError();
    }

    //连接redis
    SingletonRedis->open(SingletonConfig->getIpRedis(), SingletonConfig->getPortRedis());

    ui->label_ProgressManage->installEventFilter(this);
    ui->label_ServiceManage->installEventFilter(this);

    serviceManage_ = new ServiceManage(ui->stackedWidget->widget(0));
    ui->gridLayout_3->addWidget(serviceManage_);
    serviceManage_->show();

    //进程信息显示表
    ui->tableWidget->verticalHeader()->setHidden(true);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);    //设置内容不可更改
    ui->tableWidget->setColumnWidth(1,200);
    ui->tableWidget->setColumnWidth(3,200);
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);           //允许右击菜单
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);   //单击选择一行
    connect(ui->tableWidget,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(showCtrlMenu(QPoint)));  //右击菜单

    connect(SingletonRedis, SIGNAL(sendProcessInfo(ProcessInfoMsg)), this, SLOT(receiveProcessInfo(ProcessInfoMsg)));
    connect(SingletonRedis, SIGNAL(sendDevicesStatus(DevicesStatusMsg)), this, SLOT(receiveDevicesStatus(DevicesStatusMsg)));
    connect(SingletonRedis, SIGNAL(sendProcessAlarm(ProcessAlarmMsg)), this, SLOT(receiveProcessAlarm(ProcessAlarmMsg)));

    mapServiceStatus_[TYPE_MANUALSTOP] = "手动停止";
    mapServiceStatus_[TYPE_ERRORSTOP] = "异常停止";
    mapServiceStatus_[TYPE_NORMALRUN] = "正在运行";
    mapServiceStatus_[TYPE_UNKNOWN] = "未知";

    mapDeviceStatus_[TYPE_ONLINE] = "在线";
    mapDeviceStatus_[TYPE_OFFLINE] = "离线";
    mapDeviceStatus_[TYPE_OPEN] = "open";
    mapDeviceStatus_[TYPE_OFF] = "close";
}

MainWidget::~MainWidget()
{
    serviceManage_->deleteLater();
    delete ui;
}



bool MainWidget::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == ui->label_ServiceManage && event->type() == QEvent::MouseButtonPress)
    {
        ui->stackedWidget->setCurrentIndex(0);
        return true;
    }
    else if(obj == ui->label_ProgressManage && event->type() == QEvent::MouseButtonPress)
    {
        ui->stackedWidget->setCurrentIndex(1);
        return true;
    }
    return QObject::eventFilter(obj, event);
}

//显示进程信息
void MainWidget::receiveProcessInfo(ProcessInfoMsg processInfoMsg)
{
    QString serviceID = processInfoMsg.serviceid().c_str();
    QList<QTableWidgetItem*> listItem = ui->tableWidget->findItems(serviceID,Qt::MatchExactly);
    //查看该进程是否存在
    if(listItem.size() == 0)
    {
        int rowCount = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(rowCount);

        QTableWidgetItem *item0 = new QTableWidgetItem(serviceID);
        item0->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

        QTableWidgetItem *item1 = new QTableWidgetItem(processInfoMsg.processname().c_str());
        item1->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

        QTableWidgetItem *item2 = new QTableWidgetItem(mapServiceStatus_[processInfoMsg.status()]);
        item2->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

        QTableWidgetItem *item3 = new QTableWidgetItem(QDateTime::fromString(processInfoMsg.starttime().c_str(),"yyyyMMddhhmmsszzz").toString("yyyy-MM-dd hh:mm:ss"));
        item3->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

        QTableWidgetItem *item4 = new QTableWidgetItem(QString::number(processInfoMsg.restartnum()));
        item4->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

        QTableWidgetItem *item5 = new QTableWidgetItem(QString::number(processInfoMsg.abnormalnum()));
        item5->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

        QTableWidgetItem *item6 = new QTableWidgetItem(QDateTime::fromString(processInfoMsg.hearttime().c_str(),"yyyyMMddhhmmsszzz").toString("yyyy-MM-dd hh:mm:ss"));
        item6->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);


        ui->tableWidget->setItem(rowCount,0,item0);
        ui->tableWidget->setItem(rowCount,1,item1);
        ui->tableWidget->setItem(rowCount,2,item2);
        ui->tableWidget->setItem(rowCount,3,item3);
        ui->tableWidget->setItem(rowCount,4,item4);
        ui->tableWidget->setItem(rowCount,5,item5);
        ui->tableWidget->setItem(rowCount,6,item6);
    }
    else
    {
        int rowCount = listItem.at(0)->row();
        QTableWidgetItem *item1 = ui->tableWidget->item(rowCount,1);
        item1->setText(processInfoMsg.processname().c_str());

        QTableWidgetItem *item2 = ui->tableWidget->item(rowCount,2);
        item2->setText(mapServiceStatus_[processInfoMsg.status()]);

        QTableWidgetItem *item3 = ui->tableWidget->item(rowCount,3);
        item3->setText(QDateTime::fromString(processInfoMsg.starttime().c_str(),"yyyyMMddhhmmsszzz").toString("yyyy-MM-dd hh:mm:ss"));

        QTableWidgetItem *item4 = ui->tableWidget->item(rowCount,4);
        item4->setText(QString::number(processInfoMsg.restartnum()));

        QTableWidgetItem *item5 = ui->tableWidget->item(rowCount,5);
        item5->setText(QString::number(processInfoMsg.abnormalnum()));

        QTableWidgetItem *item6 = ui->tableWidget->item(rowCount,6);
        item6->setText(QDateTime::fromString(processInfoMsg.hearttime().c_str(),"yyyyMMddhhmmsszzz").toString("yyyy-MM-dd hh:mm:ss"));
    }
}

//显示设备状态
void MainWidget::receiveDevicesStatus(DevicesStatusMsg devicesStatusMsg)
{
   QString serviceID = devicesStatusMsg.serviceid().c_str();
   int iSize = devicesStatusMsg.devicestatus_size();
   for(int i = 0; i < iSize; ++i)
   {
       DeviceStatusMsg deviceStatusMsg = devicesStatusMsg.devicestatus(i);
       QString areaID = deviceStatusMsg.areaid().c_str();
       QString deviceID = deviceStatusMsg.deviceid().c_str();
       DeviceStatusType status = deviceStatusMsg.status();
       QString logInfo = QString("服务ID:%1 区域ID:%2 设备ID:%3 设备状态：%4").
                                      arg(serviceID).arg(areaID).arg(deviceID).arg(mapDeviceStatus_[status]);
       if(TYPE_OFFLINE == status)
       {
           ui->textBrowser->append(QString("<span style=\" color:#%1;\">%2</span>").arg("FF0000").arg(logInfo));
       }
       else
       {
           ui->textBrowser->append(QString("<span style=\" color:#%1;\">%2</span>").arg("000000").arg(logInfo));
       }

   }
}

//显示进程告警
void MainWidget::receiveProcessAlarm(ProcessAlarmMsg processAlarmMsg)
{
    ui->textBrowser->append(QString("时间:%1   服务ID:%2   进程名称:%3   告警内容:%4").
                            arg(processAlarmMsg.time().c_str()).arg(processAlarmMsg.serviceid().c_str()).
                            arg(processAlarmMsg.processname().c_str()).arg(processAlarmMsg.alarmdetail().c_str()));
}

//进程控制菜单
void MainWidget::showCtrlMenu(QPoint pos)
{
    QTableWidgetItem *currentItem = ui->tableWidget->itemAt(QPoint(pos.x(),pos.y()));
    if(currentItem != NULL)
    {
        QMenu *menu = new QMenu(ui->tableWidget);
        QAction *restartAction = new QAction(QString::fromLocal8Bit("重启"),ui->tableWidget);
        connect(restartAction, SIGNAL(triggered()), this, SLOT(restartProcess()));

        QAction *stopAllAction = new QAction(QString::fromLocal8Bit("停止"),ui->tableWidget);
        connect(stopAllAction, SIGNAL(triggered()), this, SLOT(stopProcess()));

        QAction *deleteAllAction = new QAction(QString::fromLocal8Bit("删除"),ui->tableWidget);
        connect(deleteAllAction, SIGNAL(triggered()), this, SLOT(deleteProcess()));

        menu->addAction(restartAction);
        menu->addAction(stopAllAction);
        menu->addAction(deleteAllAction);

        menu->move (cursor ().pos ());
        menu->show ();
    }
}

void MainWidget::restartProcess()
{
    if(QMessageBox::Ok == QMessageBox::warning(NULL, "警告","确认重启",QMessageBox::Ok | QMessageBox::No))
    {
        QList<QTableWidgetSelectionRange> selectedRanges = ui->tableWidget->selectedRanges();
        for(int i = 0; i < selectedRanges.size(); ++i)
        {
            QTableWidgetSelectionRange  selectedRange =  selectedRanges.at(i);
            int topRow = selectedRange.topRow();
            int bottomRow = selectedRange.bottomRow();

            for(int row = topRow; row <= bottomRow; ++row)
            {
                QString serviceID = ui->tableWidget->item(row, 0)->text();
                qDebug()<<"Restart Service:"<<serviceID;
                sendProcessCmd(serviceID, TYPE_RESTART);
            }
        }
    }
}

void MainWidget::stopProcess()
{
    if(QMessageBox::Ok == QMessageBox::warning(NULL, "警告","确认停止",QMessageBox::Ok | QMessageBox::No))
    {
        QList<QTableWidgetSelectionRange> selectedRanges = ui->tableWidget->selectedRanges();
        for(int i = 0; i < selectedRanges.size(); ++i)
        {
            QTableWidgetSelectionRange  selectedRange =  selectedRanges.at(i);
            int topRow = selectedRange.topRow();
            int bottomRow = selectedRange.bottomRow();

            for(int row = topRow; row <= bottomRow; ++row)
            {
                QString serviceID = ui->tableWidget->item(row, 0)->text();
                qDebug()<<"Stop Service:"<<serviceID;
                serviceManage_->modifyServiceStatus(serviceID, 0);
                sendProcessCmd(serviceID, TYPE_STOP);
            }
        }
    }
}

void MainWidget::deleteProcess()
{
    if(QMessageBox::Ok == QMessageBox::warning(NULL, "警告","确认删除",QMessageBox::Ok | QMessageBox::No))
    {
        QList<QTableWidgetSelectionRange> selectedRanges = ui->tableWidget->selectedRanges();
        for(int i = 0; i < selectedRanges.size(); ++i)
        {
            QTableWidgetSelectionRange  selectedRange =  selectedRanges.at(i);
            int topRow = selectedRange.topRow();
            int bottomRow = selectedRange.bottomRow();

            for(int row = topRow; row <= bottomRow; ++row)
            {
                QString serviceID = ui->tableWidget->item(row, 0)->text();
                qDebug()<<"Delete Service:"<<serviceID;

                if(SingletonDBHelper->modifyService(serviceID, 0))
                {
                    sendProcessCmd(serviceID, TYPE_DELETE);
                    serviceManage_->modifyServiceStatus(serviceID, 0);
                    ui->tableWidget->removeRow(row);
                }
            }
        }
    }
}


void MainWidget::sendProcessCmd(QString serviceID, ProcessCmdType cmd)
{
    SafeManageMainMsg safeManageMainMsg;
    safeManageMainMsg.set_msgtype(TYPE_PROCESSCTRLMSG);
    ProcessCtrlMsg* processCtrlMsg = safeManageMainMsg.mutable_processctrlmsg();
    processCtrlMsg->set_serviceid(serviceID.toStdString());
    processCtrlMsg->set_cmd(cmd);

    char dataBuf[1024] = {0};
    safeManageMainMsg.SerializeToArray(dataBuf, 1024);
    vector<string> channel;
    channel.push_back(PROCESSMANAGE);
    SingletonRedis->WriteValue(dataBuf, safeManageMainMsg.ByteSize(), channel, QString("8009_%1").arg(serviceID).toStdString());
}
