#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDir>
#include <QStandardPaths>
#include <QtQml/qqml.h>
#include "jumpssqlmodel.h"
#include "arbitraryquerymodel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    qmlRegisterType<JumpsSqlModel>("org.ffkirill.jumpsdb", 1, 0,
                                   "JumpsSqlModel");
    qmlRegisterType<ArbitraryQueryModel>("org.ffkirill.jumpsdb", 1, 0,
                                   "ArbitraryQueryModel");
    QQmlApplicationEngine engine;

    QSqlDatabase m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(QDir(QStandardPaths::writableLocation(
        QStandardPaths::DocumentsLocation)).filePath("jumps.db"));
    if (m_db.open()) {
        m_db.exec(JumpsSqlModel::SQL_INIT);
        engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    } else {
        return -1;
    };
    return app.exec();
}
