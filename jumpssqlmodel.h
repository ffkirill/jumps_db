#ifndef JUMPSSQLMODEL_H
#define JUMPSSQLMODEL_H

#include <QObject>
#include <QDate>
#include <QSqlQueryModel>
#include <QTimer>

class JumpsSqlModel : public QSqlQueryModel
{
    Q_OBJECT
    Q_PROPERTY(QString filter READ filter WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(QDate startDate READ startDate WRITE setStartDate NOTIFY startDateChanged)
    Q_PROPERTY(QDate endDate READ endDate WRITE setEndDate NOTIFY endDateChanged)

public:
    explicit JumpsSqlModel(QObject *parent = 0);
    Q_INVOKABLE void refresh();
    Q_INVOKABLE void raw(const QString &query);
    Q_INVOKABLE void addRecord(const QVariantMap &record);
    Q_INVOKABLE QString getLastDate();
    QVariant data(const QModelIndex &index, int role) const;
    Q_INVOKABLE int indexInGroup(int idx) const;
    QHash<int, QByteArray> roleNames() const;
    
    void setFilter(const QString &val);
    QString filter() const;

    void setStartDate(const QDate &val);
    QDate startDate() const;

    void setEndDate(const QDate &val);
    QDate endDate() const;

    const static char* SQL_INIT;

signals:
    void filterChanged();
    void startDateChanged();
    void endDateChanged();

public slots:
    void refreshTriggered();

private:
   const static char* COLUMN_NAMES[];
   const static char* SQL_SELECT;
   const static char* SQL_INSERT;
   const static char* GROUPER_COLUMN;
   QHash<int, QByteArray> m_roleNames;
   QSqlDatabase m_db;
   QString m_filter;
   QDate m_startDate;
   QDate m_endDate;
   QTimer m_refreshTimer;
   void runQuery();

};

#endif // JUMPSSQLMODEL_H
