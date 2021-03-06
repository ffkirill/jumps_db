#include "jumpssqlmodel.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QDateTime>
#include <QDate>
#include <QCoreApplication>
#include <QDir>
#include <QStandardPaths>
#include <QStringList>
#include <QMap>
#include <QTimer>
#include <QDebug>

JumpsSqlModel::JumpsSqlModel(QObject *parent) :
    QSqlQueryModel(parent)
{
    int idx = 0;
    while(COLUMN_NAMES[idx]) {
        m_roleNames[Qt::UserRole + idx + 1] = COLUMN_NAMES[idx];
        ++idx;
    }
    m_db = QSqlDatabase::database();
    for (QString queryText: QString(SQL_INIT).split(";")) {
        m_db.exec(queryText);
    }
    refreshTriggered();
    m_refreshTimer.setSingleShot(true);
    connect(&m_refreshTimer, SIGNAL(timeout()), this, SLOT(refreshTriggered()));
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

//Filter property
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

//Start date property
void JumpsSqlModel::setStartDate(const QDate &val)
{
    if (val != m_startDate) {
        m_startDate = val;
        refresh();
        emit startDateChanged();
    }
}

QDate JumpsSqlModel::startDate() const
{
    return m_startDate;
}


//End date property
void JumpsSqlModel::setEndDate(const QDate &val)
{
    if (val != m_endDate) {
        m_endDate = val;
        refresh();
        emit endDateChanged();
    }
}

QDate JumpsSqlModel::endDate() const
{
    return m_endDate;
}

void JumpsSqlModel::refreshTriggered()
{
    this->runQuery();
}

void JumpsSqlModel::runQuery()
{
    QSqlQuery query(m_db);
    QStringList conditions;
    QMap <QString, QString> params;

    QString filter = m_filter.trimmed();
    if (filter.length()) {
        conditions.push_back("(person LIKE '%' || :q || '%') "
            "OR (card=:q) "
            "OR (purpose LIKE '%' || :q || '%')");
        params[":q"] = filter;
    }

    if (m_startDate.isValid()) {
        conditions.push_back("date >= strftime('%s', :startDate)");
        params[":startDate"] = m_startDate.toString(Qt::ISODate);
    }

    if (m_endDate.isValid()) {
        conditions.push_back("date <= strftime('%s', :endDate)");
        params[":endDate"] = m_endDate.toString(Qt::ISODate);
    }

    QString queryText = conditions.empty() ?
                QString(SQL_SELECT).arg("") :
                QString(SQL_SELECT).arg("WHERE (" +
                                        conditions.join(") and (") + ")");

    query.prepare(queryText);
    for (auto paramName: params.keys()) {
        query.bindValue(paramName, params.value(paramName));
    }
    query.exec();
    this->setQuery(query);
}

void JumpsSqlModel::refresh()
{
    m_refreshTimer.start(1000);
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
        auto val = query.value(0).toUInt();
        if (val)
            return QDateTime::fromTime_t(val).toString("yyyy-MM-dd");
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
);
CREATE TABLE IF NOT EXISTS log
    ( id INTEGER PRIMARY KEY ASC,
      query TEXT ))";

//select count(1), person from jumps where  date = strftime('%s', "2016-06-05") and purpose like "TM инструктор%" group by person;
//select person, sum(1) from jumps where date=strftime('%s', "2016-06-23") and (purpose = "AFF REZ" or purpose like "Оператор%") group by person
