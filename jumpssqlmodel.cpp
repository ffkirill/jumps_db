#include "jumpssqlmodel.h"
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QDateTime>

JumpsSqlModel::JumpsSqlModel(QObject *parent) :
    QSqlQueryModel(parent)
{
    int idx = 0;
    while(COLUMN_NAMES[idx]) {
        m_roleNames[Qt::UserRole + idx + 1] = COLUMN_NAMES[idx];
        idx++;
    }
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("jumps.db");
    bool is_open = m_db.open();
    if (is_open == true){
        qDebug() << "Connection to database established." << endl;
    } else {
        qDebug() << "Error for database " << m_db.databaseName()
                 << " :" << (m_db.lastError().text()) << endl;
        return;
    }
    refresh();
    m_db.exec(SQL_INIT);
}

QVariant JumpsSqlModel::data(const QModelIndex &index, int role) const
{
    QVariant value = QSqlQueryModel::data(index, role);
    if (role < Qt::UserRole) {
        value = QSqlQueryModel::data(index, role);
    } else {
        int columnIdx = role - Qt::UserRole - 1;
        QModelIndex modelIndex = this->index(index.row(), columnIdx);
        value = QSqlQueryModel::data(modelIndex, Qt::DisplayRole);
    }
    return value;
}

int JumpsSqlModel::indexInGroup(int idx) const
{
    if (!idx) {
        return 0;
    }
    int grouperRole = m_roleNames.key(GROUPER_COLUMN);
    int itemIndex = idx;
    QString currentGrouper = data(index(idx, 0), grouperRole).toString();
    do {
        --idx;
    }
    while (idx >= 0
           && currentGrouper == data(index(idx, 0), grouperRole).toString());
    return itemIndex - idx - 1;
}

QHash<int, QByteArray> JumpsSqlModel::roleNames() const
{
    return m_roleNames;
}

void JumpsSqlModel::setFilter(const QString &val)
{
    if (val != m_filter) {
        m_filter = val;
        refresh();
        emit filterChanged();
    }
}

QString JumpsSqlModel::filter() const
{
    return m_filter;
}

void JumpsSqlModel::refresh()
{
    auto q = m_filter.trimmed();
    if (q.length()) {
        QSqlQuery query(m_db);
        query.prepare(QString(SQL_SELECT).arg(R"(
WHERE ((person LIKE '%' || :q || '%')
       OR (card=:q)
       OR (purpose LIKE '%' || :q || '%')
)
                                              )"
                                               ));
        query.bindValue(":q", q);
        query.exec();
        this->setQuery(query);
    } else {
        this->setQuery(QString(SQL_SELECT).arg(""), m_db);
    }
}

void JumpsSqlModel::raw(const QString &query)
{
    m_db.exec(query);
}

QString JumpsSqlModel::getLastDate()
{
    QSqlQuery query(m_db);
    query.prepare("SELECT max(date) from jumps");
    query.exec();
    if (query.next()) {
        return QDateTime::fromTime_t(
                    query.value(0).toInt()).toString("yyyy-MM-dd");
    }
    return "";
}

void JumpsSqlModel::addRecord(const QVariantMap &record) {
    QSqlQuery query(m_db);
    query.prepare(SQL_INSERT);
    query.bindValue(":person", record.value("person").toString());
    query.bindValue(":date", record.value("date").toString());
    query.bindValue(":load_num", record.value("load_num").toInt());
    query.bindValue(":purpose", record.value("purpose").toString());
    query.bindValue(":card", record.value("card").toInt());
    query.exec();
}

const char* JumpsSqlModel::COLUMN_NAMES[] = {
    "person",
    "date",
    "load_num",
    "purpose",
    "card",
    "group_val",
    NULL
};

const char* JumpsSqlModel::GROUPER_COLUMN = "group_val";

const char* JumpsSqlModel::SQL_SELECT = R"(
SELECT
    person,
    date(date, 'unixepoch'),
    load_num,
    purpose,
    card,
    "Load " || load_num || " " ||date(date, 'unixepoch') as group_val
FROM jumps %1
ORDER BY date DESC, load_num DESC;
)";

const char* JumpsSqlModel::SQL_INSERT = R"(
INSERT INTO jumps VALUES (
    :person,
    strftime('%s', :date),
    :load_num,
    :purpose,
    :card);
)";

const char* JumpsSqlModel::SQL_INIT = R"(
CREATE TABLE IF NOT EXISTS jumps
    ( person varchar(50),
      date integer,
      load_num integer,
      purpose varchar(130),
      card integer,
      PRIMARY KEY (person, date, card, load_num) );
)";

