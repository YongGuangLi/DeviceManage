#include "servicemanage.h"
#include "ui_servicemanage.h"

ServiceManage::ServiceManage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ServiceManage)
{
    ui->setupUi(this);
    QList<int> listInt;
    listInt.push_back(1);
    listInt.push_back(150);
    listInt.push_back(550);
    ui->splitter->setSizes(listInt);

    mapServiceIcon_[TYPE_ACCESSCTRL] =QIcon(":/images/Door.png");
    mapServiceIcon_[TYPE_INFRARED] =QIcon(":/images/Infrared.png");
    mapServiceIcon_[TYPE_CAMERA] =QIcon(":/images/Camera.png");
    mapServiceIcon_[TYPE_ALARMLAMP] =QIcon(":/images/Warning.png");
    mapServiceIcon_[TYPE_IPSOUND] =QIcon(":/images/Speaker.png");
    mapServiceIcon_[TYPE_LOCATION] =QIcon(":/images/Location.png");

    mapServiceName_[TYPE_ACCESSCTRL] = "门禁服务程序";
    mapServiceName_[TYPE_INFRARED] = "红外服务程序";
    mapServiceName_[TYPE_CAMERA] = "视频服务程序";
    mapServiceName_[TYPE_ALARMLAMP] = "声光告警服务程序";
    mapServiceName_[TYPE_IPSOUND] = "语音广播服务程序";
    mapServiceName_[TYPE_LOCATION] = "定位服务程序";

    initGroupBox();

    SingletonDBHelper->readAreaDataFromDB(this->mapAreaData);
    SingletonDBHelper->readDeviceDataFromDB(this->listDeviceData);

    QMapIterator<QString,QString> areaDataIt(this->mapAreaData);
    int i = 0;
    while(areaDataIt.hasNext())
    {
        areaDataIt.next();
        QPushButton *areaButton = new QPushButton(areaDataIt.value(), ui->frame_Area);  //显示区域名称
        areaButton->setObjectName(areaDataIt.key());  //把区域ID设置成对象名字，便于单机获取区域下的设备
        areaButton->setMinimumHeight(30);
        areaButton->show();
        ui->gridLayout_Area->addWidget(areaButton, i/2, i%2);
        connect(areaButton,SIGNAL(clicked()),this,SLOT(areaButtonClicked()));
        i++;
    }

    //ui->treeView->setEditTriggers(false);  //设置树节点不可编辑

    connect(ui->treeView,SIGNAL(clicked(QModelIndex)),this,SLOT(treeViewClicked(QModelIndex)));
    connect(ui->treeView,SIGNAL(customContextMenuRequested(const QPoint &)),this, SLOT(slotCustomContextMenu(const QPoint &)));
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);

    model = new QStandardItemModel(ui->treeView);

    itemProject = new QStandardItem(QString("湘潭电厂硬件配置"));
    itemProject->setEditable(false);
    model->appendRow(itemProject);

    for(int i = 0; i < mapServiceName_.size(); ++i)
    {
        QStandardItem* itemgroup = new QStandardItem(QString(mapServiceName_[ServiceNo(i)]));
        itemgroup->setEditable(false);
        itemProject->appendRow(itemgroup);
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

    listQGroupBox.push_back(ui->groupBox_Door);
    listQGroupBox.push_back(ui->groupBox_Infrared);
    listQGroupBox.push_back(ui->groupBox_Camera);
    listQGroupBox.push_back(ui->groupBox_AlarmLamp);
    listQGroupBox.push_back(ui->groupBox_IpSound);
    listQGroupBox.push_back(ui->groupBox_Location);

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
        connect(actionModify,SIGNAL(triggered()),this,SLOT(modifyServiceGroupName()));

        QAction *actionFinish = new QAction("配置完毕",ui->treeView);
        connect(actionFinish,SIGNAL(triggered()),this,SLOT(serviceConfigFinish()));

        QAction *actionDelete = new QAction("删除",ui->treeView);
        connect(actionDelete,SIGNAL(triggered()),this,SLOT(deleteServiceGroup()));

        menu->addAction(actionModify);
        menu->addAction(actionFinish);
        menu->addAction(actionDelete);
        menu->move(cursor().pos ());
        menu->show();
    }
}

void ServiceManage::treeViewClicked(QModelIndex index)
{
    QStandardItem *currentItem = model->itemFromIndex(index);
    // qDebug()<<currentItem->text();
}

void ServiceManage::createService()
{
    QModelIndex currentIndex = ui->treeView->currentIndex();
    QStandardItem *currentItem = model->itemFromIndex(currentIndex);

    QStandardItem* itemFolder = new QStandardItem(mapServiceIcon_[ServiceNo(currentIndex.row())], currentItem->text() + QString(" (%1)").arg(QUuid::createUuid().toString().mid(1,8)));
    itemFolder->setEditable(false);
    currentItem->appendRow(itemFolder);
}

void ServiceManage::modifyServiceGroupName()
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
        qDebug()<<deviceItem->text();
    }
}

void ServiceManage::deleteServiceGroup()
{
    QModelIndex currentIndex = ui->treeView->currentIndex();
    QStandardItem *currentItem = model->itemFromIndex(currentIndex);

    if(QMessageBox::Ok == QMessageBox::warning(NULL, "警告","确认删除",QMessageBox::Ok | QMessageBox::No))
    {
        currentItem->parent()->removeRow(currentIndex.row());
    }
}

void ServiceManage::areaButtonClicked()
{
    QPushButton *areaButton = dynamic_cast<QPushButton*>(sender());
    if(areaButton != NULL && mapAreaData.count(areaButton->objectName()))
    {
        clearGroupBoxButton();
        for(int i = 0; i < listDeviceData.size(); ++i)
        {
            stDeviceData deviceData = listDeviceData.at(i);
            if(deviceData.AreaID_ == areaButton->objectName())
            {
                QPushButton *deviceButton = new QPushButton(deviceData.DeviceName_);  //显示区域名称
                deviceButton->setObjectName(deviceData.DeviceID_);        //把设备ID设置成对象名字，便于单击获取设备详细信息
                switch(deviceData.serviceNo_)
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
                }
            }
        }
    }
}


bool ServiceManage::eventFilter(QObject *obj, QEvent *event)
{
    QGroupBox *groupBox = dynamic_cast<QGroupBox*>(obj);
    if(listQGroupBox.contains(groupBox) && event->type() == QEvent::MouseButtonDblClick)
    {
        QObjectList listObject = groupBox->children();

        for(int j = 0; j < listObject.size(); ++j)
        {
            QPushButton *button = dynamic_cast<QPushButton*>(listObject.at(j));
            if(button != NULL)
            {
                qDebug()<<button->objectName();
            }
        }
    }
    return QWidget::eventFilter(obj,event);
}


void ServiceManage::clearGroupBoxButton()
{
    for(int i = 0; i < listQGroupBox.size(); ++i)
    {
        QGroupBox *groupBox = listQGroupBox.at(i);
        QObjectList listObject = groupBox->children();

        for(int j = 0; j < listObject.size(); ++j)
        {
            QPushButton *button = dynamic_cast<QPushButton*>(listObject.at(j));
            if(button != NULL)
            {
                button->deleteLater();
            }
        }
    }
}
