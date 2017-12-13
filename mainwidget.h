#ifndef MAINWIDGET_H
#define MAINWIDGET_H


#include "databasehelper.h"
#include "configini.h"
#include "servicemanage.h"
#include <QWidget>
#include <QDebug>
#include <QStandardItemModel>
#include <QGridLayout>

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();
protected:
    bool eventFilter(QObject *, QEvent *);
private:
    Ui::MainWidget *ui;
    ServiceManage *serviceManage_;
};

#endif // MAINWIDGET_H