#include "arbitraryquerymodel.h"
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QDir>

ArbitraryQueryModel::ArbitraryQueryModel(QObject *parent) :
    QSqlQueryModel(parent)
{
}

void ArbitraryQueryModel::setQuery(const QString &query)
{
    QSqlQueryModel::setQuery(query, QSqlDatabase::database());
    generateRoleNames();
    saveQueryToLog(query);
}

void ArbitraryQueryModel::saveQueryToLog(const QString &text)
{
    QSqlQuery query(QSqlDatabase::database());
    query.prepare(SQL_INSERT);
    query.bindValue(":text", text);
    query.exec();
}

void ArbitraryQueryModel::generateRoleNames()
{
    m_roleNames.clear();
    for( int i = 0; i < record().count(); i ++) {
        m_roleNames.insert(Qt::UserRole + i + 1, record().fieldName(i).toUtf8());
    }
}

QVariant ArbitraryQueryModel::data(const QModelIndex &index, int role) const
{
    QVariant value;

    if(role < Qt::UserRole) {
        value = QSqlQueryModel::data(index, role);
    }
    else {
        int columnIdx = role - Qt::UserRole - 1;
        QModelIndex modelIndex = this->index(index.row(), columnIdx);
        value = QSqlQueryModel::data(modelIndex, Qt::DisplayRole);
    }
    return value;
}

QStringList ArbitraryQueryModel::getColumnNames()
{
    QStringList result;
    auto roleNames = m_roleNames.values();
    for (auto roleName: roleNames) {
        result.push_back(QString::fromUtf8(roleName));
    }
    return result;
}

QString ArbitraryQueryModel::getFromHistory(int offset) const
{
    QSqlQuery query(QSqlDatabase::database());
    query.prepare(SQL_SELECT_FROM_LOG);
    query.bindValue(":offset", offset);
    query.exec();
    if (query.next()) {
        return query.value(0).toString();
    }
    return "";
}

const char* ArbitraryQueryModel::SQL_INSERT = R"(
INSERT INTO log (query) VALUES (
    :text
))";

const char* ArbitraryQueryModel::SQL_SELECT_FROM_LOG = R"(
select query from log order by id desc limit 1 offset :offset;
)";
