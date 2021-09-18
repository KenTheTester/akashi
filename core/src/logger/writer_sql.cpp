//////////////////////////////////////////////////////////////////////////////////////
//    akashi - a server for Attorney Online 2                                       //
//    Copyright (C) 2020  scatterflower                                             //
//                                                                                  //
//    This program is free software: you can redistribute it and/or modify          //
//    it under the terms of the GNU Affero General Public License as                //
//    published by the Free Software Foundation, either version 3 of the            //
//    License, or (at your option) any later version.                               //
//                                                                                  //
//    This program is distributed in the hope that it will be useful,               //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of                //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                 //
//    GNU Affero General Public License for more details.                           //
//                                                                                  //
//    You should have received a copy of the GNU Affero General Public License      //
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.        //
//////////////////////////////////////////////////////////////////////////////////////
#include "include/logger/writer_sql.h"

WriterSQL::WriterSQL(QObject *parent) :
    QObject(parent),
    DRIVER("QSQLITE")
{
    l_dir.setPath("logs/");
    if (!l_dir.exists()) {
        l_dir.mkpath(".");
    }

    l_dir.setPath("logs/sql");
    if (!l_dir.exists()) {
        l_dir.mkpath(".");
    }

    const QString db_filename = "logs/sql/log.db";
    QFileInfo db_info(db_filename);
    if(!db_info.isReadable() || !db_info.isWritable())
        qCritical() << tr("Database Error: Missing permissions. Check if \"%1\" is writable.").arg(db_filename);
    db = QSqlDatabase::addDatabase(DRIVER);
    db.setDatabaseName("logs/sql/log.db");
    if (!db.open())
        qCritical() << "Database Error:" << db.lastError();
}

WriterSQL::~WriterSQL()
{

}

void WriterSQL::flush(const QSqlQuery& f_query)
{

}
