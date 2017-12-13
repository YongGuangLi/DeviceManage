#include "mainwidget.h"
#include "ui_mainwidget.h"

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    this->showMaximized();
    this->setWindowTitle(QString::fromLocal8Bit("硬件配置管理"));

    DBHelper_ = new DataBaseHelper(this);
    if(!DBHelper_->open(SingletonConfig->getIpMySql(),SingletonConfig->getPortMySql(),SingletonConfig->getDbName(), SingletonConfig->getUserMysql(),SingletonConfig->getPasswdMysql()))
    {
        qDebug()<<DBHelper_->getError();
    }


    serviceManage_ = new ServiceManage(ui->stackedWidget->widget(0));
    ui->gridLayout_3->addWidget(serviceManage_);
    serviceManage_->show();
}

MainWidget::~MainWidget()
{
    delete ui;
}
