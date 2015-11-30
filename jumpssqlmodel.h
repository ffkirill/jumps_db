#ifndef JUMPSSQLMODEL_H
#define JUMPSSQLMODEL_H

#include <QObject>
#include <QSqlQueryModel>

class JumpsSqlModel : public QSqlQueryModel
{
    Q_OBJECT
    Q_PROPERTY(QString filter READ filter WRITE setFilter NOTIFY filterChanged)
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

signals:
    void filterChanged();
    
public slots:

private:
   const static char* COLUMN_NAMES[];
   const static char* SQL_SELECT;
   const static char* SQL_INSERT;
   const static char* SQL_INIT;
   const static char* GROUPER_COLUMN;
   QHash<int, QByteArray> m_roleNames;
   QSqlDatabase m_db;
   QString m_filter;
};

#endif // JUMPSSQLMODEL_H
