#include "mainwidget.h"
#include "ui_mainwidget.h"

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    this->showMaximized();
    this->setWindowTitle(QString::fromLocal8Bit("硬件配置管理"));

    if(!SingletonDBHelper->open(SingletonConfig->getIpMySql(),SingletonConfig->getPortMySql(),SingletonConfig->getDbName(), SingletonConfig->getUserMysql(),SingletonConfig->getPasswdMysql()))
    {
        qDebug()<<SingletonDBHelper->getError();
    }

    SingletonRedis->open(SingletonConfig->getIpRedis(), SingletonConfig->getPortRedis());
    ui->label_ProgressManage->installEventFilter(this);
    ui->label_ServiceManage->installEventFilter(this);


    serviceManage_ = new ServiceManage(ui->stackedWidget->widget(0));
    ui->gridLayout_3->addWidget(serviceManage_);
    serviceManage_->show();
}

MainWidget::~MainWidget()
{
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
