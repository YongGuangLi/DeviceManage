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

    publicIconMap_[1] =QIcon(":/images/Camera.png");
    publicIconMap_[0] =QIcon(":/images/Door.png");
    publicIconMap_[4] =QIcon(":/images/Infrared.png");
    publicIconMap_[5] =QIcon(":/images/Speaker.png");
    publicIconMap_[3] =QIcon(":/images/Warning.png");
    publicIconMap_[2] =QIcon(":/images/Location.png");

    serviceName_<<"门禁服务程序"<<"视频服务程序"<<"定位服务程序"<<"声光告警服务程序"<<"红外服务程序"<<"语音广播服务程序";
    //ui->treeView->setEditTriggers(false);  //设置树节点不可编辑
    connect(ui->treeView,SIGNAL(clicked(QModelIndex)),this,SLOT(treeViewClicked(QModelIndex)));

    model = new QStandardItemModel(ui->treeView);
    model->setHorizontalHeaderLabels(QStringList()<<QString("名称"));

    itemProject = new QStandardItem(QString("硬件接口服务程序"));
    model->appendRow(itemProject);
    itemProject->setEditable(false);
    QStandardItem* itemFolder = new QStandardItem(QString("新建硬件接口服务程序1"));
    itemProject->appendRow(itemFolder);

    for(int i = 0; i < serviceName_.size(); ++i)
    {
        QStandardItem* itemgroup = new QStandardItem(publicIconMap_[i],QString("%1 (%2)").arg(serviceName_.at(i)).arg(QUuid::createUuid().toString().mid(1,8)));
        itemgroup->setEditable(false);
        itemFolder->appendRow(itemgroup);
        for(int j = 0; j < (i+1); ++j)
        {
            QStandardItem* itemchannel = new QStandardItem(publicIconMap_[i],QString("123%1").arg(j+1));
            itemchannel->setCheckable(true);
            itemchannel->setEditable(false);
            itemchannel->setCheckState(Qt::Checked);
            itemgroup->appendRow(itemchannel);
        }
    }

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
        QAction *actionFinish = new QAction("配置完毕",ui->treeView);
        connect(actionFinish,SIGNAL(triggered()),this,SLOT(serviceConfigFinish()));

        QAction *actionDelete = new QAction("删除",ui->treeView);
        connect(actionDelete,SIGNAL(triggered()),this,SLOT(deleteServiceGroup()));
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
    itemProject->appendRow(itemFolder);
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
            qDebug()<<deviceItem->text();
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


