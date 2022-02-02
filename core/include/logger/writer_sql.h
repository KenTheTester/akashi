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
#ifndef WRITER_SQL_H
#define WRITER_SQL_H

//If the DB is ever updated, this needs to be increased in order to allow updateDB to run.
#define TARGET_DB 1

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDir>
#include <QDebug>

class WriterSQL : public QObject
{
    Q_OBJECT
public:

    /**
     * @brief Constructor for an SQL log writer. Creates and updates its DB if necessary.
     *
     * @param Pointer to the logger.
     */
    WriterSQL(QObject* parent = nullptr);

    /**
     * @brief Deconstructor for the SQL writer.
     */
    virtual ~WriterSQL();

    /**
     * @brief Executes an SQL query on the log database.
     *
     * @param SQL query to execute
     */
    void flush(QSqlQuery f_query);

private:
    /**
     * @brief The name of the database connection driver.
     */
    const QString DRIVER;

    /**
     * @brief The backing database that stores logfiles.
     */
    QSqlDatabase m_db;

    /**
     * @brief The current server DB version.
     */
    int db_version;

    /**
     * @brief checkVersion Checks the current server DB version.
     *
     * @return Returns the server DB version.
     */
    int checkVersion();

    /**
     * @brief updateDB Updates the server DB to the latest version.
     *
     * @param current_version The current DB version.
     */
    void updateDB(int current_version);
};



#endif //WRITER_SQL_H
