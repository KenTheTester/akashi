#include "include/logger/writer_sql.h"

WriterSQL::WriterSQL(QObject *parent) :
    QObject(parent),
    DRIVER("QSQLITE")
{
    QDir l_dir("storage/database");
    if (!l_dir.exists())
        l_dir.mkpath(".");

    const QString db_filename = "storage/database/log.db";
    QFileInfo db_info(db_filename);
    if (!db_info.exists()) {
        qWarning().noquote() << tr("Database Info: Log database not found. "
                                   "Attempting to create new database.");
    }
    else {
        // We should only check if a file is readable/writeable when it actually
        // exists.
        if (!db_info.isReadable() || !db_info.isWritable()) {
            qCritical() << tr("Database Error: Missing permissions. Check if \"%1\" "
                              "is writable.")
                               .arg(db_filename);
            return;
        }
    }

    m_db = QSqlDatabase::addDatabase(DRIVER, "LOGDB");
    m_db.setDatabaseName("storage/database/log.db");
    if (!m_db.open())
        qCritical() << "Database Error:" << m_db.lastError();
    db_version = checkVersion();
    if (db_version != TARGET_DB) {
        updateDB(db_version);
    }
}

WriterSQL::~WriterSQL() { m_db.close(); }

void WriterSQL::flush(QSqlQuery f_query)
{
    f_query.exec();
    if (!f_query.exec())
        qDebug() << "SQL Error:" << f_query.lastError().text();
}

const QSqlDatabase WriterSQL::getDatabase() { return m_db; }

int WriterSQL::checkVersion()
{
    QSqlQuery query(m_db);
    query.prepare("PRAGMA user_version");
    query.exec();
    if (query.first()) {
        return query.value(0).toInt();
    }
    else {
        return 0;
    }
}

void WriterSQL::updateDB(int current_version)
{
    QSqlQuery l_query(m_db);
    switch (current_version) {
    case 0:
        l_query.exec("PRAGMA foreign_keys = ON");
        l_query.exec("CREATE TABLE IF NOT EXISTS ipids(ipid TEXT PRIMARY KEY, "
                     "ip_address TEXT UNIQUE NOT NULL)");
        l_query.exec("CREATE TABLE hwips(hwip INTEGER PRIMARY KEY,hwid TEXT,ipid "
                     "TEXT NOT NULL,FOREIGN KEY (ipid) REFERENCES ipids(ipid)ON "
                     "DELETE SET NULL,UNIQUE (hwid, ipid) ON CONFLICT IGNORE)");
        l_query.exec(
            "CREATE TABLE IF NOT EXISTS ic_events(event_time DATETIME DEFAULT "
            "CURRENT_TIMESTAMP,hwip INTEGER NOT NULL,room_name TEXT,char_name "
            "TEXT,ic_name TEXT,message TEXT NOT NULL,FOREIGN KEY (hwip) REFERENCES "
            "hwips(hwip)ON DELETE CASCADE)");
        l_query.exec("CREATE TABLE IF NOT EXISTS room_event_types(type_id INTEGER "
                     "PRIMARY KEY,type_name TEXT NOT NULL UNIQUE)");
        l_query.exec(
            "CREATE TABLE IF NOT EXISTS room_events(event_id PRIMARY "
            "KEY,event_time DATETIME DEFAULT CURRENT_TIMESTAMP,hwip INTEGER NOT "
            "NULL,target_hwip INTEGER,room_name TEXT,char_name TEXT,ooc_name "
            "TEXT,event_subtype INTEGER NOT NULL,message TEXT,FOREIGN KEY (hwip) "
            "REFERENCES hwips(hwip)ON DELETE CASCADE,FOREIGN KEY (target_hwip) "
            "REFERENCES hwips(hwip)ON DELETE CASCADE,FOREIGN KEY (event_subtype) "
            "REFERENCES room_event_types(type_id))");
        l_query.exec(
            "CREATE TABLE IF NOT EXISTS login_events(event_time DATETIME DEFAULT "
            "CURRENT_TIMESTAMP,hwip INTEGER NOT NULL,profile_name TEXT,FOREIGN KEY "
            "(hwip) REFERENCES hwips(hwip)ON DELETE CASCADE)");
        l_query.exec("CREATE TABLE IF NOT EXISTS connect_events(event_time "
                     "DATETIME DEFAULT CURRENT_TIMESTAMP,hwip INTEGER NOT "
                     "NULL,hdid TEXT NOT NULL,failed INTEGER DEFAULT 0,FOREIGN KEY "
                     "(hwip) REFERENCES hwips(hwip)ON DELETE CASCADE)");
        l_query.exec("CREATE TABLE IF NOT EXISTS misc_event_types(type_id INTEGER "
                     "PRIMARY KEY,type_name TEXT NOT NULL UNIQUE)");
        l_query.exec(
            "CREATE TABLE IF NOT EXISTS misc_events(event_time DATETIME DEFAULT "
            "CURRENT_TIMESTAMP,hwip INTEGER, target_hwip INTEGER,event_subtype "
            "INTEGER NOT NULL,event_data TEXT,FOREIGN KEY (hwip) REFERENCES "
            "hwips(hwip)ON DELETE CASCADE,FOREIGN KEY (target_hwip) REFERENCES "
            "hwips(hwip)ON DELETE CASCADE,FOREIGN KEY (event_subtype) REFERENCES "
            "misc_event_types(type_id))");
        l_query.exec("PRAGMA user_version = 1");
        Q_FALLTHROUGH();
    case 1:
        // Currently unused.
        ;
    }
}
