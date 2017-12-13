#ifndef DATABASEHELPER_H
#define DATABASEHELPER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlDriver>
#include <QDebug>

class DataBaseHelper : public QObject
{
    Q_OBJECT
public:
    explicit DataBaseHelper(QObject *parent = 0);
    bool open(QString ip,int port, QString dbName, QString user, QString passwd);

    QString getError();
private:
    QSqlDatabase sqlDatabase;
    QString ip_;
    int port_;
    QString dbName_;
    QString user_;
    QString passwd_;
signals:
    
public slots:
    
};

#endif // DATABASEHELPER_H
