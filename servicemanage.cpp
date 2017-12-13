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

    //ui->treeView->setEditTriggers(false);  //设置树节点不可编辑
    connect(ui->treeView,SIGNAL(clicked(QModelIndex)),this,SLOT(treeViewClicked(QModelIndex)));

    model = new QStandardItemModel(ui->treeView);
    model->setHorizontalHeaderLabels(QStringList()<<QString("名称"));

    itemProject = new QStandardItem(QString("硬件接口服务程序"));
    itemProject->setEditable(false);
    model->appendRow(itemProject);


    /*
    for(int i = 0; i < mapServiceName_.size(); ++i)
    {
        QStandardItem* itemgroup = new QStandardItem(publicIconMap_[i],QString("%1 (%2)").arg(mapServiceName_.[i]).arg(QUuid::createUuid().toString().mid(1,8)));
        itemFolder->appendRow(itemgroup);
        for(int j = 0; j < (i+1); ++j)
        {
            QStandardItem* itemchannel = new QStandardItem(publicIconMap_[i],QString("123%1").arg(j+1));
            itemchannel->setCheckable(true);
            itemchannel->setCheckState(Qt::Checked);
            itemgroup->appendRow(itemchannel);
        }
    }
    */

    ui->treeView->setModel(model);

    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeView,SIGNAL(customContextMenuRequested(const QPoint &)),this, SLOT(slotCustomContextMenu(const QPoint &)));

}

ServiceManage::~ServiceManage()
{
    delete ui;
}

void ServiceManage::slotCustomContextMenu(const QPoint &pos)
{
    QStandardItemModel* model = static_cast<QStandardItemModel*>(ui->treeView->model());
    QModelIndex currentIndex = ui->treeView->currentIndex();
    QStandardItem* currentItem = model->itemFromIndex(currentIndex);

    if(currentItem->text().compare("硬件接口服务程序") == 0)
    {
        QMenu *menu = new QMenu(ui->treeView);
        QAction *action = new QAction("新增硬件接口程序",ui->treeView);
        connect(action,SIGNAL(triggered()),this,SLOT(createServiceGroup()));
        menu->addAction(action);
        menu->move (cursor ().pos ());
        menu->show ();
    }
    else if(currentItem->text().compare("新建硬件接口服务程序1") == 0)
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
//     QStandardItemModel* model = static_cast<QStandardItemModel*>(ui->treeView->model());
//     QStandardItem* currentItem = model->itemFromIndex(index);
//     QString text = currentItem->parent()->text().left(currentItem->parent()->text().indexOf(" "));
//     if(serviceName_.contains(text))
//     {
//         qDebug()<<currentItem->text();
    //     }
}

void ServiceManage::createServiceGroup()
{
    QStandardItem* itemFolder = new QStandardItem(QString("新建硬件接口服务程序1"));
    itemFolder->setEditable(false);
    itemProject->appendRow(itemFolder);
    for(int i = 0; i < mapServiceName_.size(); ++i)
    {
        QStandardItem* itemgroup = new QStandardItem(publicIconMap_[i],QString("%1 (%2)").arg(mapServiceName_[serviceNo(i)]).arg(QUuid::createUuid().toString().mid(1,8)));
        itemgroup->setEditable(false);
        itemFolder->appendRow(itemgroup);
    }
}

void ServiceManage::modifyServiceGroupName()
{
    QModelIndex currentIndex = ui->treeView->currentIndex();
    QStandardItem *currentItem = model->itemFromIndex(currentIndex);
    qDebug()<<currentItem->text();
    currentItem->setEditable(true);
}

void ServiceManage::serviceConfigFinish()
{
    QModelIndex currentIndex = ui->treeView->currentIndex();
    QStandardItem *currentItem = model->itemFromIndex(currentIndex);
    int rows = currentItem->rowCount();

    for(int i = 0; i < rows; ++i)
    {
        QStandardItem *serviceItem = currentItem->child(i);
        for(int j = 0; j < serviceItem->rowCount(); ++j)
        {
            QStandardItem *deviceItem = serviceItem->child(i);
            if(deviceItem->checkState() == Qt::Checked)
            {
                qDebug()<<deviceItem->text();
            }
        }
    }
}

void ServiceManage::deleteServiceGroup()
{
    QModelIndex currentIndex = ui->treeView->currentIndex();
    if(QMessageBox::Ok == QMessageBox::warning(NULL, "警告","确认删除",QMessageBox::Ok | QMessageBox::No))
    {
         itemProject->removeRow(currentIndex.row());
    }
}


