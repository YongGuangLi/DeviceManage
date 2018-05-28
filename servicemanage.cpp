#include "servicemanage.h"
#include "ui_servicemanage.h"


#define CkeckedStyleSheet   "background-color: rgb(136, 136, 136);"
#define UnCkeckStyleSheet   "background-color: rgb(255, 0, 255);"
#define UnSelectStyleSheet  "background-color: rgb(0, 0, 0);"

ServiceManage::ServiceManage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ServiceManage)
{
    ui->setupUi(this);
    QList<int> listInt;
    listInt.push_back(150);
    listInt.push_back(160);
    listInt.push_back(550);
    ui->splitter->setSizes(listInt);

    mapServiceIcon_[TYPE_ACCESSCTRL] =QIcon(":/images/Door.png");
    mapServiceIcon_[TYPE_INFRARED] =QIcon(":/images/Infrared.png");
    mapServiceIcon_[TYPE_CAMERA] =QIcon(":/images/Camera.png");
    mapServiceIcon_[TYPE_ALARMLAMP] =QIcon(":/images/Warning.png");
    mapServiceIcon_[TYPE_IPSOUND] =QIcon(":/images/Speaker.png");
    mapServiceIcon_[TYPE_LOCATION] =QIcon(":/images/Location.png");
    mapServiceIcon_[TYPE_CAMERA_COUNT] =QIcon(":/images/Counting.png");

    mapServiceTypeName_[TYPE_ACCESSCTRL] = "门禁服务程序";
    mapServiceTypeName_[TYPE_INFRARED] = "红外服务程序";
    mapServiceTypeName_[TYPE_CAMERA] = "视频服务程序";
    mapServiceTypeName_[TYPE_ALARMLAMP] = "声光告警服务程序";
    mapServiceTypeName_[TYPE_IPSOUND] = "语音广播服务程序";
    mapServiceTypeName_[TYPE_LOCATION] = "定位服务程序";
    mapServiceTypeName_[TYPE_CAMERA_COUNT] = "人数统计服务程序";


    mapServicePrefix_[TYPE_ACCESSCTRL] = "Door";
    mapServicePrefix_[TYPE_INFRARED] = "Infrared";
    mapServicePrefix_[TYPE_CAMERA] = "Camera";
    mapServicePrefix_[TYPE_ALARMLAMP] = "Warning";
    mapServicePrefix_[TYPE_IPSOUND] = "Speaker";
    mapServicePrefix_[TYPE_LOCATION] = "Location";
    mapServicePrefix_[TYPE_CAMERA_COUNT] = "Counting";

    mapServiceType_["门禁服务程序"] = TYPE_ACCESSCTRL;
    mapServiceType_["红外服务程序"] = TYPE_INFRARED;
    mapServiceType_["视频服务程序"] = TYPE_CAMERA;
    mapServiceType_["声光告警服务程序"] = TYPE_ALARMLAMP;
    mapServiceType_["语音广播服务程序"] = TYPE_IPSOUND;
    mapServiceType_["定位服务程序"] = TYPE_LOCATION;
    mapServiceType_["人数统计服务程序"] = TYPE_CAMERA_COUNT;

    SingletonDBHelper->readAreaDataFromDB(this->mapAreaData);
    SingletonDBHelper->readDeviceDataFromDB(this->listDeviceData_);
    SingletonDBHelper->readServiceDataFromDB(this->mapServiceID_,this->mapServiceStatus_);

    initGroupBox();

    initServiceData();

    initAreaData();

    connect(SingletonRedis, SIGNAL(sendDeviceInitRequest(DeviceInitRequestMsg)), this, SLOT(receiveDeviceInitRequest(DeviceInitRequestMsg)));
}

ServiceManage::~ServiceManage()
{
    delete ui;
}

void ServiceManage::initGroupBox()
{
    FlowLayout *flowLayout_Door = new FlowLayout(ui->groupBox_Door);
    FlowLayout *flowLayout_Infrared = new FlowLayout(ui->groupBox_Infrared);
    FlowLayout *flowLayout_Camera = new FlowLayout(ui->groupBox_Camera);
    FlowLayout *flowLayout_AlarmLamp = new FlowLayout(ui->groupBox_AlarmLamp);
    FlowLayout *flowLayout_IpSound = new FlowLayout(ui->groupBox_IpSound);
    FlowLayout *flowLayout_Location = new FlowLayout(ui->groupBox_Location);
    FlowLayout *flowLayout_Counting = new FlowLayout(ui->groupBox_Counting);

    Q_UNUSED(flowLayout_Door);
    Q_UNUSED(flowLayout_Infrared);
    Q_UNUSED(flowLayout_Camera);
    Q_UNUSED(flowLayout_AlarmLamp);
    Q_UNUSED(flowLayout_IpSound);
    Q_UNUSED(flowLayout_Location);
    Q_UNUSED(flowLayout_Counting);

    listGroupBox_.push_back(ui->groupBox_Door);
    listGroupBox_.push_back(ui->groupBox_Infrared);
    listGroupBox_.push_back(ui->groupBox_Camera);
    listGroupBox_.push_back(ui->groupBox_AlarmLamp);
    listGroupBox_.push_back(ui->groupBox_IpSound);
    listGroupBox_.push_back(ui->groupBox_Location);
    listGroupBox_.push_back(ui->groupBox_Counting);

    //绑定GroupBox对应设备类型，双击GroupBox过滤对应设备服务
    mapGroupBoxTitle_["门禁设备"] = TYPE_ACCESSCTRL;
    mapGroupBoxTitle_["红外设备"] = TYPE_INFRARED;
    mapGroupBoxTitle_["摄像设备"] = TYPE_CAMERA;
    mapGroupBoxTitle_["声光告警"] = TYPE_ALARMLAMP;
    mapGroupBoxTitle_["语音广播"] = TYPE_IPSOUND;
    mapGroupBoxTitle_["定位设备"] = TYPE_LOCATION;
    mapGroupBoxTitle_["人数统计设备"] = TYPE_CAMERA_COUNT;

    ui->groupBox_Door->installEventFilter(this);
    ui->groupBox_Infrared->installEventFilter(this);
    ui->groupBox_Camera->installEventFilter(this);
    ui->groupBox_AlarmLamp->installEventFilter(this);
    ui->groupBox_IpSound->installEventFilter(this);
    ui->groupBox_Location->installEventFilter(this);
    ui->groupBox_Counting->installEventFilter(this);
}

void ServiceManage::initServiceData()
{
    //显示服务treeView
    //ui->treeView->setEditTriggers(false);  //设置树节点不可编辑
    connect(ui->treeView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(serviceItemDoubleClicked(QModelIndex)));
    //添加右击菜单
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeView,SIGNAL(customContextMenuRequested(const QPoint &)),this, SLOT(slotCustomContextMenu(const QPoint &)));

    serviceModel = new QStandardItemModel(ui->treeView);
    connect(serviceModel, SIGNAL(itemChanged(QStandardItem*)), this,SLOT(deviceCheckChange(QStandardItem*)));

    QStandardItem* itemProject = new QStandardItem(QString("湘潭电厂硬件配置"));
    itemProject->setEditable(false);
    serviceModel->appendRow(itemProject);
    QMapIterator<DeviceType,QString> itServiceTypeName(mapServiceTypeName_);
    while(itServiceTypeName.hasNext())
    {
        itServiceTypeName.next();
        DeviceType serviceType = itServiceTypeName.key();
        QString serviceTypeName = itServiceTypeName.value();
        QStandardItem* servicesitem = new QStandardItem(serviceTypeName);
        servicesitem->setEditable(false);
        itemProject->appendRow(servicesitem);

        QStringList listServiceID = mapServiceID_[serviceType];
        for(int j = 0; j < listServiceID.size(); ++j)
        {
            QStandardItem* servicitem = new QStandardItem(mapServiceIcon_[serviceType],listServiceID.at(j));
            servicitem->setEditable(false);
            servicesitem->appendRow(servicitem);
            mapServiceItem_[listServiceID.at(j)] = servicitem;  //保存所有服务的树节点
            for(int z = 0; z < listDeviceData_.size(); z++)
            {
                stDeviceData *deviceData = listDeviceData_.at(z);
                if(deviceData->ServiceID_ == listServiceID.at(j))
                {
                    QString deviceID = deviceData->DeviceID_;
                    QString deviceName = deviceData->DeviceName_;
                    QStandardItem* deviceItem = new QStandardItem(deviceName);
                    deviceItem->setCheckable(true);
                    if(deviceData->Checkable_)
                    {
                        deviceItem->setCheckState(Qt::Checked);
                    }
                    deviceItem->setEditable(false);
                    servicitem->appendRow(deviceItem);

                    //保存所有设备树节点信息
                    stDeviceData *deviceData = findDeviceDataByID(deviceID);
                    mapDeviceItem_[deviceItem] = deviceData;
                }
            }
        }
    }
    ui->treeView->setModel(serviceModel);
}

void ServiceManage::initAreaData()
{
    //显示区域treeView
    connect(ui->treeView_area,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(areaItemDoubleClicked(QModelIndex)));
    areaModel = new QStandardItemModel(ui->treeView_area);

    QMapIterator<QString,stAreaData> itAreaData(mapAreaData);
    while(itAreaData.hasNext())
    {
        itAreaData.next();
        stAreaData areaData = itAreaData.value();
        if(areaData.Level_ == 0)
        {
            QStandardItem *areaItem = new QStandardItem(areaData.AreadName_);
            areaItem->setEditable(false);
            areaModel->appendRow(areaItem);
            mapAreaItem_[areaData.AreaID_] =  areaItem;
            break;
        }
    }

    recursionAreaData(1);

    ui->treeView_area->setModel(areaModel);
}

void ServiceManage::recursionAreaData(int level)
{
    bool flag = false;
    QMapIterator<QString,stAreaData> itAreaData(mapAreaData);
    while(itAreaData.hasNext())
    {
        itAreaData.next();
        stAreaData areaData = itAreaData.value();
        if(areaData.Level_ == level)
        {
            flag = true;
            QStandardItem *areaItem = new QStandardItem(areaData.AreadName_);
            areaItem->setEditable(false);

            QStandardItem *parentItem = mapAreaItem_.value(areaData.ParentAreaID_,NULL);
            if(parentItem == NULL)
            {
                qDebug()<<areaData.ParentAreaID_;
                continue;
            }
            mapAreaItem_[areaData.AreaID_] = areaItem;
            parentItem->appendRow(areaItem);
        }
    }
    if(flag)
    {
        recursionAreaData(++level);
    }
}

void ServiceManage::slotCustomContextMenu(const QPoint &pos)
{
    Q_UNUSED(pos);
    QModelIndex currentIndex = ui->treeView->currentIndex();
    QStandardItem* currentItem = serviceModel->itemFromIndex(currentIndex);

    if(mapServiceTypeName_.values().contains(currentItem->text()))
    {
        QMenu *menu = new QMenu(ui->treeView);
        QAction *action = new QAction("新增硬件接口程序",ui->treeView);
        connect(action,SIGNAL(triggered()),this,SLOT(createService()));
        menu->addAction(action);
        menu->move (cursor ().pos ());
        menu->show ();
    }
    else if(currentItem->parent() != NULL && mapServiceTypeName_.values().contains(currentItem->parent()->text()))
    {
        QMenu *menu = new QMenu(ui->treeView);

        QAction *actionFinish = new QAction("配置完毕",ui->treeView);
        connect(actionFinish,SIGNAL(triggered()),this,SLOT(serviceConfigFinish()));

        QAction *actionDelete = new QAction("删除",ui->treeView);
        connect(actionDelete,SIGNAL(triggered()),this,SLOT(deleteServiceItem()));

        menu->addAction(actionFinish);
        menu->addAction(actionDelete);
        menu->move(cursor().pos());
        menu->show();
    }
}

void ServiceManage::serviceItemDoubleClicked(QModelIndex index)
{
    QStandardItem *currentItem = serviceModel->itemFromIndex(index);
    if(stDeviceData* deviceData = mapDeviceItem_.value(currentItem, NULL))
    {
        dispDeviceData(deviceData->DeviceID_);
    }
}

//新建服务
void ServiceManage::createService()
{
    QModelIndex currentIndex = ui->treeView->currentIndex();
    QStandardItem *currentItem = serviceModel->itemFromIndex(currentIndex);
    DeviceType deviceType = mapServiceType_[currentItem->text()];

    QStringList listServiceID =  mapServiceID_[deviceType];

    int iCnt = 0;
    if(listServiceID.size())
    {
        QString serviceID = listServiceID.at(listServiceID.size() - 1);
        iCnt = serviceID.right(3).toInt() + 1;
    }

    QStandardItem* serviceItem = new QStandardItem(mapServiceIcon_[deviceType],
            QString("%1%2").arg(mapServicePrefix_[deviceType]).arg(iCnt,3,10,QChar('0')));

    if(SingletonDBHelper->writeServiceDataToDB(serviceItem->text(),deviceType))
    {
        serviceItem->setEditable(false);
        currentItem->appendRow(serviceItem);

        mapServiceItem_[serviceItem->text()] = serviceItem;
        mapServiceID_[deviceType].push_back(serviceItem->text());
    }
}

void ServiceManage::serviceConfigFinish()
{
    QModelIndex currentIndex = ui->treeView->currentIndex();
    QStandardItem *currentItem = serviceModel->itemFromIndex(currentIndex);
    sendDeviceConfigMsg(currentItem);
}

void ServiceManage::deleteServiceItem()               //删除树上的服务节点
{
    QModelIndex currentIndex = ui->treeView->currentIndex();
    QStandardItem *currentItem = serviceModel->itemFromIndex(currentIndex);
    QString serviceID = currentItem->text();

    int serviceStatus = mapServiceStatus_.value(serviceID, -1);
    if(serviceStatus == 1)
    {
        QMessageBox::Ok == QMessageBox::warning(NULL, "警告","服务正在运行",QMessageBox::Ok);
        return;
    }

    if(QMessageBox::Ok == QMessageBox::warning(NULL, "警告","确认删除",QMessageBox::Ok | QMessageBox::No))
    {

        if(SingletonDBHelper->deleteService(serviceID))
        {
            mapServiceItem_.remove(serviceID);                                 //删除服务节点

            DeviceType deviceType = mapServiceType_[currentItem->parent()->text()];
            mapServiceID_[deviceType].removeOne(serviceID);                     //在保存所有服务的map中删除服务

            int rowCount = currentItem->rowCount();
            for(int i = 0; i < rowCount; ++i)
            {
                QStandardItem * deviceItem = currentItem->child(i);
                if(stDeviceData* deviceData = mapDeviceItem_.value(deviceItem, NULL))
                {
                    QString deviceID = deviceData->DeviceID_;
                    if(SingletonDBHelper->modifyDeviceServiceID(deviceID, 0, ""))
                    {
                        deviceData->Checkable_ = 0;
                        deviceData->ServiceID_ = "";
                        mapDeviceItem_.remove(deviceItem);                                 //删除设备树节点
                        QPushButton *deviceButton = mapDeviceButton.value(deviceID, NULL);
                        if(deviceButton != NULL)
                        {
                            deviceButton->setStyleSheet(UnSelectStyleSheet);
                        }
                    }
                }
            }
            currentItem->parent()->removeRow(currentIndex.row());
        }
    }
}

void ServiceManage::areaItemDoubleClicked(QModelIndex index)
{
    QStandardItem *item = areaModel->itemFromIndex(index);

    QString areaID;
    QMapIterator<QString,QStandardItem*> itAreaItem(mapAreaItem_);
    while(itAreaItem.hasNext())
    {
        itAreaItem.next();
        if(itAreaItem.value() == item)
        {
            areaID = itAreaItem.key();
            break;
        }
    }
    clearGroupBoxButton();
    for(int i = 0; i < listDeviceData_.size(); ++i)
    {
        stDeviceData *deviceData = listDeviceData_.at(i);
        if(deviceData->AreaID_ == areaID)
        {
            QPushButton *deviceButton = new QPushButton(deviceData->DeviceName_);  //显示区域名称
            deviceButton->setObjectName(deviceData->DeviceID_);        //把设备ID设置成对象名字，便于单击获取设备详细信息
            deviceButton->setMinimumWidth(153);
            if(deviceData->ServiceID_.isEmpty())
            {
                deviceButton->setStyleSheet(UnSelectStyleSheet);  //设备没有选择服务
            }
            else if(deviceData->Checkable_ == 1)
            {
                deviceButton->setStyleSheet(CkeckedStyleSheet);    //设备选择服务,并且被勾选
            }
            else
            {
                deviceButton->setStyleSheet(UnCkeckStyleSheet);   //设备选择服务，但是没被勾选
            }
            connect(deviceButton,SIGNAL(clicked()),this,SLOT(deviceButtonClicked()));

            mapDeviceButton[deviceData->DeviceID_] = deviceButton;                 //保存当前显示的设备

            switch(deviceData->deviceType_)
            {
            case TYPE_ACCESSCTRL:
                ui->groupBox_Door->layout()->addWidget(deviceButton);
                break;
            case TYPE_INFRARED:
                ui->groupBox_Infrared->layout()->addWidget(deviceButton);
                break;
            case TYPE_CAMERA:
                ui->groupBox_Camera->layout()->addWidget(deviceButton);
                break;
            case TYPE_ALARMLAMP:
                ui->groupBox_AlarmLamp->layout()->addWidget(deviceButton);
                break;
            case TYPE_IPSOUND:
                ui->groupBox_IpSound->layout()->addWidget(deviceButton);
                break;
            case TYPE_LOCATION:
                ui->groupBox_Location->layout()->addWidget(deviceButton);
                break;
            case TYPE_CAMERA_COUNT:
                ui->groupBox_Counting->layout()->addWidget(deviceButton);
                break;
            default:
                break;
            }
        }
    }
}

void ServiceManage::deviceCheckChange(QStandardItem *deviceItem)
{
    if(deviceItem->checkState() == Qt::Checked)             //选中
    {
        if(stDeviceData* deviceData = mapDeviceItem_.value(deviceItem, NULL))
        {
            if(SingletonDBHelper->modifyDeviceCheck(deviceData->DeviceID_, 1))
            {
                if(mapDeviceButton.value(deviceData->DeviceID_, NULL))
                {
                    mapDeviceButton[deviceData->DeviceID_]->setStyleSheet(CkeckedStyleSheet);
                }
                findDeviceDataByID(deviceData->DeviceID_)->Checkable_ = 1;
            }
        }
    }
    else if(deviceItem->checkState() == Qt::Unchecked)      //取消选中
    {
        if(stDeviceData* deviceData = mapDeviceItem_.value(deviceItem, NULL))
        {
            if(SingletonDBHelper->modifyDeviceCheck(deviceData->DeviceID_, 0))
            {
                if(mapDeviceButton.value(deviceData->DeviceID_, NULL))
                {
                    mapDeviceButton[deviceData->DeviceID_]->setStyleSheet(UnCkeckStyleSheet);
                }
                findDeviceDataByID(deviceData->DeviceID_)->Checkable_ = 0;
            }
        }
    }
}

void ServiceManage::selectServiceID(QTableWidgetItem *item)
{
    QTableWidget *table = dynamic_cast<QTableWidget*>(sender());
    if(table != NULL)
    {
        addDeviceItem(item->text());
        table->deleteLater();
    }
}

void ServiceManage::deviceButtonClicked()
{
    QPushButton *deviceButton = dynamic_cast<QPushButton*>(sender());
    if(deviceButton != NULL)
    {
        dispDeviceData(deviceButton->objectName());
    }
}

void ServiceManage::dispDeviceData(QString deviceID)
{
    stDeviceData *deviceData = findDeviceDataByID(deviceID);
    if(deviceData != NULL)
    {
        static DeviceDataDisp *deviceDataDisp = new DeviceDataDisp();
        deviceDataDisp->setWindowIcon(mapServiceIcon_[deviceData->deviceType_]);
        deviceDataDisp->setDeviceType(deviceData->deviceType_);
        deviceDataDisp->setWindowTitle("设备详细信息");
        deviceDataDisp->setDeviceData(deviceData->DeviceID_,deviceData->DeviceName_,getAreaNameById(deviceData->AreaID_), deviceData->AreaID_,deviceData->DeviceIp_,deviceData->DevicePort_,deviceData->DeviceUser_,deviceData->DevicePasswd,deviceData->ServiceID_, deviceData->Factory_);
        deviceDataDisp->setWindowFlags(Qt::WindowStaysOnTopHint);
        deviceDataDisp->show();
    }
}

void ServiceManage::sendDeviceConfigMsg(QStandardItem *serviceItem)
{
    SafeManageMainMsg safeManageMsg;
    safeManageMsg.set_msgtype(TYPE_DEVICECONFIGMSG);

    DeviceType deviceType = mapServiceType_[serviceItem->parent()->text()];

    DeviceConfigMsg* deviceConfigMsg = safeManageMsg.mutable_deviceconfigmsg();
    deviceConfigMsg->set_type(deviceType);
    deviceConfigMsg->set_serviceid(serviceItem->text().toStdString());

    int rows = serviceItem->rowCount();
    for(int i = 0; i < rows; ++i)
    {
        QStandardItem *deviceItem = serviceItem->child(i);
        if(stDeviceData *deviceData = mapDeviceItem_.value(deviceItem, NULL))
        {
            if(deviceItem->checkState() == Qt::Checked)
            {
                qDebug()<<"Send DeviceData:"<<deviceData->DeviceID_.toStdString().c_str();
                DeviceInfoMsg* deviceInfoMsg = deviceConfigMsg->add_deviceinfo();
                deviceInfoMsg->set_areaid(deviceData->AreaID_.toStdString());
                deviceInfoMsg->set_deviceid(deviceData->DeviceID_.toStdString());
                deviceInfoMsg->set_ip(deviceData->DeviceIp_.toStdString());
                deviceInfoMsg->set_port(deviceData->DevicePort_.toInt());
                deviceInfoMsg->set_user(deviceData->DeviceUser_.toStdString());
                deviceInfoMsg->set_key(deviceData->DevicePasswd.toStdString());

                deviceConfigMsg->set_factory(deviceData->Factory_.toStdString());
            }
        }
    }


    char dataBuf[2048] = {0};
    safeManageMsg.SerializeToArray(dataBuf, 2048);
    vector<string> channel;
    channel.push_back(DATACENTER);

    if(SingletonRedis->WriteValue(dataBuf, safeManageMsg.ByteSize(), channel, QString("8002_%1").arg(serviceItem->text()).toStdString()))
    {
        qDebug()<<QString("Send ServiceDeviceData:%1,Size:%2").arg(serviceItem->text()).arg(safeManageMsg.ByteSize());
    }
}

void ServiceManage::modifyServiceStatus(QString serviceID, int status)
{
    mapServiceStatus_[serviceID] = status;

    QStandardItem* serviceItem = mapServiceItem_.value(serviceID, NULL);
    if(serviceItem == NULL)
        return;

    int rowCount = serviceItem->rowCount();
    bool enabled = status == 1 ? false : true;   //如果服务状态为1,配置为运行状态，不让改变设备节点的复选框
    for(int i = 0; i < rowCount; ++i)
    {
        QStandardItem* deviceItem = serviceItem->child(i);
        deviceItem->setEnabled(enabled);
    }
}

QString ServiceManage::getAreaNameById(QString areaId)
{
    QString areaName;
    QMapIterator<QString,stAreaData> itAreaData(mapAreaData);
    while(itAreaData.hasNext())
    {
        itAreaData.next();
        stAreaData areaData = itAreaData.value();
        if(areaData.AreaID_ == areaId)
        {
            areaName = areaData.AreadName_;
        }
    }
    return areaName;
}

DeviceType ServiceManage::getServiceTypeByServiceID(QString serviceID)
{
    QMapIterator<DeviceType,QStringList> itServiceID(mapServiceID_);
    while(itServiceID.hasNext())
    {
        itServiceID.next();
        QStringList listServiceID = itServiceID.value();
        bool isExist = listServiceID.contains(serviceID);
        if(isExist)
        {
            return itServiceID.key();
        }
    }
}


bool ServiceManage::eventFilter(QObject *obj, QEvent *event)
{
    QGroupBox *groupBox = dynamic_cast<QGroupBox*>(obj);
    if(listGroupBox_.contains(groupBox) && event->type() == QEvent::MouseButtonDblClick)
    {
        QObjectList listObject = groupBox->children();
        if(listObject.size() >= 0)
        {
            DeviceType deviceType_ = mapGroupBoxTitle_[groupBox->title()];

            QTableWidget *tableServiceID = new QTableWidget();
            tableServiceID->setColumnCount(1);
            tableServiceID->setWindowTitle(QString::fromLocal8Bit("选择服务"));
            tableServiceID->setEditTriggers(QAbstractItemView::NoEditTriggers);    //设置内容不可更改
            tableServiceID->horizontalHeader()->setStretchLastSection(true);
            tableServiceID->horizontalHeader()->hide();

            QStringList listService = mapServiceID_.value(deviceType_);  //获取该设备类型的所有服务
            for(int i = 0; i < listService.size(); ++i)
            {
                int rowCount = tableServiceID->rowCount();
                tableServiceID->insertRow(rowCount);

                QTableWidgetItem *item0 = new QTableWidgetItem(listService.at(i));
                item0->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

                tableServiceID->setItem(rowCount,0,item0);
            }
            tableServiceID->show();

            currentSelectDeviceGroup = groupBox;  //当前选中设备组
            connect(tableServiceID,SIGNAL(itemDoubleClicked(QTableWidgetItem *)), this, SLOT(selectServiceID(QTableWidgetItem *)));
        }
    }
    return QWidget::eventFilter(obj,event);
}


void ServiceManage::clearGroupBoxButton()
{
    for(int i = 0; i < listGroupBox_.size(); ++i)
    {
        QGroupBox *groupBox = listGroupBox_.at(i);
        QObjectList listObject = groupBox->children();

        for(int j = 0; j < listObject.size(); ++j)
        {
            QPushButton *button = dynamic_cast<QPushButton*>(listObject.at(j));
            if(button != NULL)
            {
                mapDeviceButton.remove(button->objectName());   //只保存当前显示的设备
                button->deleteLater();
            }
        }
    }
}

void ServiceManage::addDeviceItem(QString serviceId)
{
    QStandardItem *serviceItem = mapServiceItem_[serviceId];
    QObjectList listObject = currentSelectDeviceGroup->children();
    for(int i = 0; i < listObject.size(); ++i)
    {
        if(QPushButton *button = dynamic_cast<QPushButton*>(listObject.at(i)))
        {
            QString deviceID = button->objectName();
            QString deviceName = button->text();
            stDeviceData* deviceData = findDeviceDataByID(deviceID);

            if(findDeviceItemByID(deviceID) != NULL) //查看所有树节点是否已有该设备ID
                continue;

            static QString Factory = deviceData->Factory_;

            if(Factory != deviceData->Factory_)      //只添加第一个设备厂家的设备
                continue;

            if(SingletonDBHelper->modifyDeviceServiceID(deviceID, 1, serviceId))
            {
                QStandardItem *deviceItem = new QStandardItem(deviceName);
                deviceItem->setCheckable(true);
                deviceItem->setCheckState(Qt::Checked);
                deviceItem->setEditable(false);
                serviceItem->appendRow(deviceItem);

                mapDeviceButton[deviceID]->setStyleSheet(CkeckedStyleSheet);

                stDeviceData *deviceData = findDeviceDataByID(deviceID);
                deviceData->ServiceID_ = serviceId;       //修改设备服务ID
                deviceData->Checkable_ = 1;               //设备选中
                mapDeviceItem_[deviceItem] = deviceData;      //保存所有设备树节点信息
            }
        }
    }

}

QStandardItem *ServiceManage::findDeviceItemByID(QString deviceID)
{
    QStandardItem * deviceItem = NULL;
    QMapIterator<QStandardItem*, stDeviceData*> it(mapDeviceItem_);

    while(it.hasNext())
    {
        it.next();
        stDeviceData* deviceData = it.value();
        if(deviceData->DeviceID_ == deviceID)
        {
            deviceItem = it.key();
            break;
        }
    }

    return deviceItem;
}

stDeviceData* ServiceManage::findDeviceDataByID(QString deviceID)
{
    for(int i = 0 ; i < listDeviceData_.size(); ++i)
    {
        stDeviceData* deviceData = listDeviceData_.at(i);
        if(deviceData->DeviceID_ == deviceID)
        {
            return deviceData;
        }
    }
    return NULL;
}


void ServiceManage::receiveDeviceInitRequest(DeviceInitRequestMsg deviceInitRequest)
{
    QString serviceID = deviceInitRequest.serviceid().c_str();
    qDebug()<<"receiveDeviceInitRequest:"<<serviceID<<" Type:"<<deviceInitRequest.type();
    if(serviceID == QString(DATACENTER))
    {
        QMapIterator<QString, int> itServiceStatus(mapServiceStatus_);
        while(itServiceStatus.hasNext())
        {
            itServiceStatus.next();
            if(itServiceStatus.value() == 1)
            {
                if(getServiceTypeByServiceID(itServiceStatus.key()) == deviceInitRequest.type())
                {
                    QStandardItem* serviceItem = mapServiceItem_.value(itServiceStatus.key(), NULL);
                    if(serviceItem == NULL)
                    {
                        qDebug()<<itServiceStatus.key()<<" not exist QStandardItem";
                        return;
                    }
                    sendDeviceConfigMsg(serviceItem);
                }
            }
        }
    }
    else
    {
        QStandardItem* serviceItem = mapServiceItem_.value(serviceID, NULL);
        if(serviceItem == NULL)
        {
            qDebug()<<serviceID<<" not exist QStandardItem";
            return;
        }
        SingletonDBHelper->modifyService(serviceID, 1);
        modifyServiceStatus(serviceID, 1);              //收到服务程序设备请求，修改服务状态
        sendDeviceConfigMsg(serviceItem);
    }
}
