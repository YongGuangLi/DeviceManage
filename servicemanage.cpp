#include "servicemanage.h"
#include "ui_servicemanage.h"


#define CkeckedStyleSheet   "background-color: rgb(225, 225, 225);"
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

    mapServiceIcon_[ACCESSCTRL] =QIcon(":/images/Door.png");
    mapServiceIcon_[INFRARED] =QIcon(":/images/Infrared.png");
    mapServiceIcon_[CAMERA] =QIcon(":/images/Camera.png");
    mapServiceIcon_[ALARMLAMP] =QIcon(":/images/Warning.png");
    mapServiceIcon_[IPSOUND] =QIcon(":/images/Speaker.png");
    mapServiceIcon_[LOCATION] =QIcon(":/images/Location.png");

    mapServiceName_[ACCESSCTRL] = "门禁服务程序";
    mapServiceName_[INFRARED] = "红外服务程序";
    mapServiceName_[CAMERA] = "视频服务程序";
    mapServiceName_[ALARMLAMP] = "声光告警服务程序";
    mapServiceName_[IPSOUND] = "语音广播服务程序";
    mapServiceName_[LOCATION] = "定位服务程序";

    initGroupBox();

    SingletonDBHelper->readAreaDataFromDB(this->mapAreaData);
    SingletonDBHelper->readDeviceDataFromDB(this->listDeviceData_);
    SingletonDBHelper->readServiceDataFromDB(this->mapServiceID_);

    QMapIterator<QString,QString> areaDataIt(this->mapAreaData);

    QButtonGroup *ButtonGroup = new QButtonGroup();
    ButtonGroup->setExclusive(true);
    int i = 0;
    while(areaDataIt.hasNext())
    {
        areaDataIt.next();
        QPushButton *areaButton = new QPushButton(areaDataIt.value(), ui->frame_Area);  //显示区域名称
        areaButton->setObjectName(areaDataIt.key());  //把区域ID设置成对象名字，便于单机获取区域下的设备
        areaButton->setMinimumHeight(30);
        areaButton->setCheckable(true);
        ButtonGroup->addButton(areaButton);

        areaButton->show();
        ui->gridLayout_Area->addWidget(areaButton, i/2, i%2);
        connect(areaButton,SIGNAL(clicked()),this,SLOT(areaButtonClicked()));
        i++;
    }

    //ui->treeView->setEditTriggers(false);  //设置树节点不可编辑

    connect(ui->treeView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(treeViewClicked(QModelIndex)));
    connect(ui->treeView,SIGNAL(customContextMenuRequested(const QPoint &)),this, SLOT(slotCustomContextMenu(const QPoint &)));

    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);

    model = new QStandardItemModel(ui->treeView);
    connect(model, SIGNAL(itemChanged(QStandardItem*)), this,SLOT(deviceCheckChange(QStandardItem*)));

    itemProject = new QStandardItem(QString("湘潭电厂硬件配置"));
    itemProject->setEditable(false);
    model->appendRow(itemProject);

    for(int i = 0; i < mapServiceName_.size(); ++i)
    {
        QStandardItem* servicesitem = new QStandardItem(QString(mapServiceName_[ServiceNo(i)]));
        servicesitem->setEditable(false);
        itemProject->appendRow(servicesitem);

        QStringList listServiceID = mapServiceID_[ServiceNo(i)];
        for(int j = 0; j < listServiceID.size(); ++j)
        {
            QStandardItem* servicitem = new QStandardItem(mapServiceIcon_[ServiceNo(i)],listServiceID.at(j));
            servicitem->setEditable(false);
            servicesitem->appendRow(servicitem);
            mapServiceItem_[listServiceID.at(j)] = servicitem;  //保存所有服务的树节点
            for(int z = 0; z < listDeviceData_.size(); z++)
            {
                stDeviceData *deviceData = listDeviceData_.at(z);
                if(deviceData->ServiceID_ == listServiceID.at(j))
                {
                    QStandardItem* deviceitem = new QStandardItem(deviceData->DeviceID_);
                    deviceitem->setCheckable(true);
                    if(deviceData->Checkable_)
                    {
                        deviceitem->setCheckState(Qt::Checked);
                    }
                    deviceitem->setEditable(false);
                    servicitem->appendRow(deviceitem);
                }
            }
        }
    }

    ui->treeView->setModel(model);
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

    listQGroupBox_.push_back(ui->groupBox_Door);
    listQGroupBox_.push_back(ui->groupBox_Infrared);
    listQGroupBox_.push_back(ui->groupBox_Camera);
    listQGroupBox_.push_back(ui->groupBox_AlarmLamp);
    listQGroupBox_.push_back(ui->groupBox_IpSound);
    listQGroupBox_.push_back(ui->groupBox_Location);

    mapGroupBoxName_["门禁设备"] = ACCESSCTRL;
    mapGroupBoxName_["红外设备"] = INFRARED;
    mapGroupBoxName_["摄像设备"] = CAMERA;
    mapGroupBoxName_["声光告警"] = ALARMLAMP;
    mapGroupBoxName_["语音广播"] = IPSOUND;
    mapGroupBoxName_["定位设备"] = LOCATION;

    ui->groupBox_Door->installEventFilter(this);
    ui->groupBox_Infrared->installEventFilter(this);
    ui->groupBox_Camera->installEventFilter(this);
    ui->groupBox_AlarmLamp->installEventFilter(this);
    ui->groupBox_IpSound->installEventFilter(this);
    ui->groupBox_Location->installEventFilter(this);
}

void ServiceManage::slotCustomContextMenu(const QPoint &pos)
{
    Q_UNUSED(pos);
    QModelIndex currentIndex = ui->treeView->currentIndex();
    QStandardItem* currentItem = model->itemFromIndex(currentIndex);

    if(mapServiceName_.values().contains(currentItem->text()))
    {
        QMenu *menu = new QMenu(ui->treeView);
        QAction *action = new QAction("新增硬件接口程序",ui->treeView);
        connect(action,SIGNAL(triggered()),this,SLOT(createService()));
        menu->addAction(action);
        menu->move (cursor ().pos ());
        menu->show ();
    }
    else if(currentItem->parent() != NULL && mapServiceName_.values().contains(currentItem->parent()->text()))
    {
        QMenu *menu = new QMenu(ui->treeView);
        QAction *actionModify = new QAction("修改名称",ui->treeView);
        connect(actionModify,SIGNAL(triggered()),this,SLOT(modifyServiceName()));

        QAction *actionFinish = new QAction("配置完毕",ui->treeView);
        connect(actionFinish,SIGNAL(triggered()),this,SLOT(serviceConfigFinish()));

        QAction *actionDelete = new QAction("删除",ui->treeView);
        connect(actionDelete,SIGNAL(triggered()),this,SLOT(deleteServiceItem()));

        menu->addAction(actionModify);
        menu->addAction(actionFinish);
        menu->addAction(actionDelete);
        menu->move(cursor().pos());
        menu->show();
    }
}

void ServiceManage::treeViewClicked(QModelIndex index)
{
    QStandardItem *currentItem = model->itemFromIndex(index);
    for(int i = 0; i < listDeviceData_.size(); ++i)
    {
        stDeviceData *deviceData = listDeviceData_.at(i);
        if(deviceData->DeviceID_ == currentItem->text())
        {
            dispDeviceData(deviceData->DeviceID_);
        }
    }
}

void ServiceManage::createService()
{
    QModelIndex currentIndex = ui->treeView->currentIndex();
    QStandardItem *currentItem = model->itemFromIndex(currentIndex);

    QStandardItem* serviceItem = new QStandardItem(mapServiceIcon_[ServiceNo(currentIndex.row())], currentItem->text() + QString(" (%1)").arg(QUuid::createUuid().toString().mid(1,8)));
    serviceItem->setEditable(false);
    currentItem->appendRow(serviceItem);

    mapServiceItem_[serviceItem->text()] = serviceItem;
    mapServiceID_[ServiceNo(currentIndex.row())].push_back(serviceItem->text());

    SingletonDBHelper->writeServiceDataToDB(serviceItem->text(),currentIndex.row());
}

void ServiceManage::modifyServiceName()
{
    QModelIndex currentIndex = ui->treeView->currentIndex();
    QStandardItem *currentItem = model->itemFromIndex(currentIndex);
    currentItem->setEditable(true);
}

void ServiceManage::serviceConfigFinish()
{
    QModelIndex currentIndex = ui->treeView->currentIndex();
    QStandardItem *currentItem = model->itemFromIndex(currentIndex);
    int rows = currentItem->rowCount();

    for(int i = 0; i < rows; ++i)
    {
        QStandardItem *deviceItem = currentItem->child(i);
        if(deviceItem->checkState() == Qt::Checked)
        {
            SafeManageMsg safeManageMsg;

            qDebug()<<"device id:"<<deviceItem->text();
        }
    }
}

void ServiceManage::deleteServiceItem()               //删除树上的服务节点
{
    QModelIndex currentIndex = ui->treeView->currentIndex();
    QStandardItem *currentItem = model->itemFromIndex(currentIndex);

    if(QMessageBox::Ok == QMessageBox::warning(NULL, "警告","确认删除",QMessageBox::Ok | QMessageBox::No))
    {
        QString serviceID = currentItem->text();
        SingletonDBHelper->deleteService(serviceID);
        mapServiceItem_.remove(serviceID);                                             //删除服务节点

        ServiceNo serviceNo = ServiceNo(currentItem->parent()->row());
        mapServiceID_[serviceNo].removeOne(serviceID);   //在保存所有服务的map中删除服务
        int rowCount = currentItem->rowCount();
        for(int i = 0; i < rowCount; ++i)
        {
            QString deviceID = currentItem->child(i)->text();

            SingletonDBHelper->modifyDeviceServiceID(deviceID, "");
            SingletonDBHelper->modifyDeviceCheck(deviceID, 0);

            mapDeviceStatus[deviceID] = false;
            QPushButton *deviceButton = mapDeviceButton.value(deviceID, NULL);
            if(deviceButton != NULL)
            {
                deviceButton->setStyleSheet(UnSelectStyleSheet);
            }
        }
        currentItem->parent()->removeRow(currentIndex.row());
    }
}

void ServiceManage::areaButtonClicked()
{
    QPushButton *areaButton = dynamic_cast<QPushButton*>(sender());
    if(areaButton != NULL && mapAreaData.count(areaButton->objectName()))
    {
        clearGroupBoxButton();
        for(int i = 0; i < listDeviceData_.size(); ++i)
        {
            stDeviceData *deviceData = listDeviceData_.at(i);
            if(deviceData->AreaID_ == areaButton->objectName())
            {
                QPushButton *deviceButton = new QPushButton(deviceData->DeviceName_);  //显示区域名称
                connect(deviceButton,SIGNAL(clicked()),this,SLOT(deviceButtonClicked()));
                if(deviceData->ServiceID_.isEmpty())                   //设备没有分配服务
                {
                      deviceButton->setStyleSheet(UnSelectStyleSheet);
                }
                else if(deviceData->Checkable_ == 1)                   //设备分配服务,被选中
                {

                    deviceButton->setStyleSheet(CkeckedStyleSheet);
                }
                else                                                   //设备分配服务，但是没被选中
                {
                    deviceButton->setStyleSheet(UnCkeckStyleSheet);
                }

                deviceButton->setObjectName(deviceData->DeviceID_);        //把设备ID设置成对象名字，便于单击获取设备详细信息
                deviceButton->setMinimumWidth(150);
                mapDeviceButton[deviceData->DeviceID_] = deviceButton;                 //保存当前显示的设备

                bool status = mapDeviceStatus.value(deviceData->DeviceID_, false);     //如果为true,已被选中
                if(status == true)
                {
                    deviceButton->setStyleSheet(CkeckedStyleSheet);
                }

                switch(deviceData->serviceNo_)
                {
                case ACCESSCTRL:
                    ui->groupBox_Door->layout()->addWidget(deviceButton);
                    break;
                case INFRARED:
                    ui->groupBox_Infrared->layout()->addWidget(deviceButton);
                    break;
                case CAMERA:
                    ui->groupBox_Camera->layout()->addWidget(deviceButton);
                    break;
                case ALARMLAMP:
                    ui->groupBox_AlarmLamp->layout()->addWidget(deviceButton);
                    break;
                case IPSOUND:
                    ui->groupBox_IpSound->layout()->addWidget(deviceButton);
                    break;
                case LOCATION:
                    ui->groupBox_Location->layout()->addWidget(deviceButton);
                    break;
                }
            }
        }
    }
}

void ServiceManage::deviceCheckChange(QStandardItem *deviceItem)
{
    if(deviceItem->checkState() == Qt::Checked)             //选中
    {
        if(mapDeviceButton.value(deviceItem->text(), NULL))
        {
            mapDeviceButton[deviceItem->text()]->setStyleSheet(CkeckedStyleSheet);
        }
        SingletonDBHelper->modifyDeviceCheck(deviceItem->text(), 1);
        mapDeviceStatus[deviceItem->text()] = true;
    }
    else if(deviceItem->checkState() == Qt::Unchecked)      //取消选中
    {
        if(mapDeviceButton.value(deviceItem->text(), NULL))
        {
            mapDeviceButton[deviceItem->text()]->setStyleSheet(UnCkeckStyleSheet);
        }
        SingletonDBHelper->modifyDeviceCheck(deviceItem->text(), 0);
        mapDeviceStatus[deviceItem->text()] = false;
    }
}

void ServiceManage::selectServiceID(QTableWidgetItem *item)
{
    QTableWidget *table = dynamic_cast<QTableWidget*>(sender());
    if(table != NULL)
    {
        addDeviceItem(item->text(), currentSelectDeviceGroup->children());
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
        deviceDataDisp->setWindowIcon(mapServiceIcon_[deviceData->serviceNo_]);
        deviceDataDisp->setWindowTitle("设备详细信息");
        deviceDataDisp->setDeviceData(deviceData->DeviceID_,deviceData->DeviceName_,mapAreaData[deviceData->AreaID_], deviceData->AreaID_,deviceData->DeviceIp_,deviceData->DevicePort_,deviceData->DeviceUser_,deviceData->DevicePasswd,deviceData->ServiceID_);
        deviceDataDisp->show();
    }
}


bool ServiceManage::eventFilter(QObject *obj, QEvent *event)
{
    QGroupBox *groupBox = dynamic_cast<QGroupBox*>(obj);
    if(listQGroupBox_.contains(groupBox) && event->type() == QEvent::MouseButtonDblClick)
    {
        QObjectList listObject = groupBox->children();
        if(listObject.size() > 1)
        {
            ServiceNo serviceNo = mapGroupBoxName_[groupBox->title()];

            QTableWidget *tableServiceID = new QTableWidget();
            tableServiceID->setColumnCount(1);
            tableServiceID->setWindowTitle(QString::fromLocal8Bit("选择服务"));
            tableServiceID->setEditTriggers(QAbstractItemView::NoEditTriggers);    //设置内容不可更改
            tableServiceID->horizontalHeader()->setStretchLastSection(true);
            tableServiceID->horizontalHeader()->hide();
            for(int i = 0; i < mapServiceID_[serviceNo].size(); ++i)
            {
                int rowCount = tableServiceID->rowCount();
                tableServiceID->insertRow(rowCount);

                QTableWidgetItem *item0 = new QTableWidgetItem(mapServiceID_[serviceNo].at(i));
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
    for(int i = 0; i < listQGroupBox_.size(); ++i)
    {
        QGroupBox *groupBox = listQGroupBox_.at(i);
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

void ServiceManage::addDeviceItem(QString serviceId, QObjectList listObject)
{
    QStandardItem *serviceItem = mapServiceItem_[serviceId];

    for(int i = 0; i < listObject.size(); ++i)
    {
        if(QPushButton *button = dynamic_cast<QPushButton*>(listObject.at(i)))
        {
            QString deviceID = button->objectName();
            //QString deviceName = button->text();
            QStandardItem * item = findDeviceItemByID(itemProject, deviceID); //查看树节点是否已有该设备ID
            if(item == NULL)                                     //没有
            {
                QStandardItem* deviceItem = new QStandardItem(deviceID);
                deviceItem->setCheckable(true);
                deviceItem->setCheckState(Qt::Checked);
                deviceItem->setEditable(false);
                serviceItem->appendRow(deviceItem);
                mapDeviceStatus[deviceID] = true;              //设备选中服务
                mapDeviceButton[deviceID]->setStyleSheet(CkeckedStyleSheet);
                findDeviceDataByID(deviceID)->ServiceID_ = serviceId;   //修改设备服务ID
                findDeviceDataByID(deviceID)->Checkable_ = 1;
                SingletonDBHelper->modifyDeviceServiceID(deviceID, serviceId);
            }
        }
    }

}

QStandardItem *ServiceManage::findDeviceItemByID(QStandardItem* item,QString deviceID)
{
    int rowCount = item->rowCount();
    for(int i = 0; i < rowCount; ++i)
    {
        QStandardItem *servicesItem  = item->child(i);
        int servicesCount = servicesItem->rowCount();
        for(int j = 0; j < servicesCount; ++j)
        {
            QStandardItem *serviceItem  = servicesItem->child(j);

            int deviceCount = serviceItem->rowCount();
            for(int z = 0; z < deviceCount; ++z)
            {
                QStandardItem *deviceItem = serviceItem->child(z);
                if(deviceItem->text() == deviceID)
                {
                    return deviceItem;
                }
            }
        }
    }
    return NULL;
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
