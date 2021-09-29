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

#include <QObject>
#include <QtSql>

class WriterSQL : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Constructor to generate an logging DB if not created yet and setup SQL logging.
     *
     * @param QObject pointer to the parent object.
     */
    WriterSQL(QObject* parent = nullptr);

    /**
     * @brief Deconstructor of WriterSQL. Closes the DB connection if logmode is switched or the server shuts down.
     */
    virtual ~WriterSQL();

    /**
     * @brief Executed the logging query for the respective event. This method does not validate queries and expects queries to be
     *        checked before given to execution.
     */
    void flush(const QSqlQuery& f_query);

private:
    /**
     * @brief The name of the database connection driver.
     */
    const QString DRIVER;

    /**
     * @brief The backing database that stores user details.
     */
    QSqlDatabase m_log_db;

    /**
     * @brief Representation of the directory the DB is located in.
     */
    QDir l_dir;
};


#endif //WRITER_SQL_H
