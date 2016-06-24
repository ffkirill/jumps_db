#ifndef ARBITRARYQUERYMODEL_H
#define ARBITRARYQUERYMODEL_H

#include <QSqlQueryModel>
#include <QStringList>

class ArbitraryQueryModel : public QSqlQueryModel
{
    Q_OBJECT

public:
    explicit ArbitraryQueryModel(QObject *parent = 0);

    Q_INVOKABLE void setQuery(const QString &query);
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const {
        QHash<int, QByteArray> result;
        for (auto roleName: m_roleNames.keys()) {
            result[roleName] = m_roleNames[roleName];
        }
        return result;
    }
    Q_INVOKABLE QStringList getColumnNames();
    Q_INVOKABLE QString getFromHistory(int offset) const;

private:
    void generateRoleNames();
    void saveQueryToLog(const QString &text);
    QMap<int, QByteArray> m_roleNames;
    const static char* SQL_INSERT;
    const static char* SQL_SELECT_FROM_LOG;
};

#endif // ARBITRARYQUERYMODEL_H
