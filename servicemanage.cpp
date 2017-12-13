#include "servicemanage.h"
#include "ui_servicemanage.h"

ServiceManage::ServiceManage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ServiceManage)
{
    ui->setupUi(this);
    QList<int> listInt;
    listInt.push_back(1);
    listInt.push_back(300);
    ui->splitter->setSizes(listInt);

    publicIconMap_[0] =QIcon(":/images/Door.png");
    publicIconMap_[1] =QIcon(":/images/Camera.png");
    publicIconMap_[2] =QIcon(":/images/Location.png");
    publicIconMap_[3] =QIcon(":/images/Warning.png");
    publicIconMap_[4] =QIcon(":/images/Infrared.png");
    publicIconMap_[5] =QIcon(":/images/Speaker.png");

    mapServiceName_[Door] = "门禁服务程序";
    mapServiceName_[Camera] = "视频服务程序";
    mapServiceName_[Location] = "定位服务程序";
    mapServiceName_[Warning] = "声光告警服务程序";
    mapServiceName_[Infrared] = "红外服务程序";
    mapServiceName_[Speaker] = "语音广播服务程序";

    SingletonDBHelper->readAreaDataFromDB();
    SingletonDBHelper->readDeviceDataFromDB();
    //ui->treeView->setEditTriggers(false);  //设置树节点不可编辑
    connect(ui->treeView,SIGNAL(clicked(QModelIndex)),this,SLOT(treeViewClicked(QModelIndex)));
    connect(ui->treeView,SIGNAL(customContextMenuRequested(const QPoint &)),this, SLOT(slotCustomContextMenu(const QPoint &)));
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);

    model = new QStandardItemModel(ui->treeView);
    model->setHorizontalHeaderLabels(QStringList()<<QString("名称"));

    itemProject = new QStandardItem(QString("湘潭电厂硬件配置"));
    itemProject->setEditable(false);
    model->appendRow(itemProject);


    for(int i = 0; i < mapServiceName_.size(); ++i)
    {
        QStandardItem* itemgroup = new QStandardItem(QString(mapServiceName_[serviceNo(i)]));
        itemgroup->setEditable(false);
        itemProject->appendRow(itemgroup);
    }


    ui->treeView->setModel(model);
}

ServiceManage::~ServiceManage()
{
    delete ui;
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
        connect(action,SIGNAL(triggered()),this,SLOT(createServiceGroup()));
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
        menu->move (cursor ().pos ());
        menu->show ();
    }
}

void ServiceManage::treeViewClicked(QModelIndex index)
{
    QStandardItem *currentItem = model->itemFromIndex(index);
    qDebug()<<currentItem->text();
}

void ServiceManage::createServiceGroup()
{
    QModelIndex currentIndex = ui->treeView->currentIndex();
    QStandardItem *currentItem = model->itemFromIndex(currentIndex);

    QStandardItem* itemFolder = new QStandardItem(publicIconMap_[currentIndex.row()], currentItem->text() + QString(" (%1)").arg(QUuid::createUuid().toString().mid(1,8)));
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


