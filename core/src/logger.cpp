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

#include <QDir>

#include "include/logger.h"

void Logger::logIC(const QString& f_areaName_r, const QString& f_charName_r, const QString& f_ipid_r, const QString& f_message_r)
{
    addEntry(f_areaName_r, f_charName_r, f_ipid_r, "IC", f_message_r);
}

void Logger::logOOC(const QString& f_areaName_r, const QString& f_charName_r, const QString& f_ipid_r, const QString& f_message_r)
{
    addEntry(f_areaName_r, f_charName_r, f_ipid_r, "OOC", f_message_r);
}

void Logger::logModcall(const QString& f_areaName_r, const QString& f_charName_r, const QString& f_ipid_r, const QString& f_modcallReason_r)
{
    addEntry(f_areaName_r, f_charName_r, f_ipid_r, "MODCALL", f_modcallReason_r);
}

void Logger::logCmd(const QString& f_areaName_r, const QString& f_charName_r, const QString& f_ipid_r,
                    const QString& f_oocMessage_r, const QString& f_cmd_r, const QStringList& f_cmdArgs_r)
{
    // Some commands contain sensitive data, like passwords
    // These must be filtered out
    if (f_cmd_r == "login") {
        addEntry(f_areaName_r, f_charName_r, f_ipid_r, "LOGIN", "Attempted login");
    }
    else if (f_cmd_r == "rootpass") {
        addEntry(f_areaName_r, f_charName_r, f_ipid_r, "USERS", "Root password created");
    }
    else if (f_cmd_r == "adduser" && !f_cmdArgs_r.isEmpty()) {
        addEntry(f_areaName_r, f_charName_r, f_ipid_r, "USERS", "Added user " + f_cmdArgs_r[0]);
    }
    else {
        logOOC(f_areaName_r, f_charName_r, f_ipid_r, f_oocMessage_r);
    }
}

void Logger::logLogin(const QString& f_areaName_r, const QString& f_charName_r, const QString& f_ipid_r, bool success, const QString& f_modname_r)
{
    QString l_message = success ? "Logged in as " + f_modname_r : "Failed to log in as " + f_modname_r;
    addEntry(f_areaName_r, f_charName_r, f_ipid_r, "LOGIN", l_message);
}

void Logger::addEntry(
        const QString& f_areaName_r,
        const QString& f_charName_r,
        const QString& f_ipid_r,
        const QString& f_type_r,
        const QString& f_message_r)
{
    QString l_time = QDateTime::currentDateTime().toString("ddd MMMM d yyyy | hh:mm:ss");

    QString l_logEntry = QStringLiteral("[%1][%2][%6] %3(%4): %5\n")
            .arg(l_time, f_areaName_r, f_charName_r, f_ipid_r, f_message_r, f_type_r);

    if (m_buffer.length() < m_maxLength) {
        m_buffer.enqueue(l_logEntry);

        if (m_logType == "full") {
           flush(f_areaName_r);
        }
    }
    else {
        m_buffer.dequeue();
        m_buffer.enqueue(l_logEntry);
    }
}

void Logger::flush(const QString& f_areaName_r)
{
    QDir l_dir("logs/");
    if (!l_dir.exists()) {
        l_dir.mkpath(".");
    }

    QFile l_logfile;

    if (m_logType == "modcall") {
        l_logfile.setFileName(QString("logs/report_%1_%2.log").arg(f_areaName_r, (QDateTime::currentDateTime().toString("yyyy-MM-dd_hhmmss"))));
    }
    else if (m_logType == "full") {
        l_logfile.setFileName(QString("logs/%1.log").arg(QDate::currentDate().toString("yyyy-MM-dd")));
    }
    else {
        qCritical("Invalid logger set!");
    }

    if (l_logfile.open(QIODevice::WriteOnly | QIODevice::Append)) {
        QTextStream file_stream(&l_logfile);

        while (!m_buffer.isEmpty())
            file_stream << m_buffer.dequeue();
    }

    l_logfile.close();
}
