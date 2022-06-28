/**
 * akashi - a server for Attorney Online 2
 * Copyright (C) 2020 - 2022  scatterflower
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "include/logger/u_logger.h"
#include "include/data_types.h"

using LogType = DataTypes::LogType;

ULogger::ULogger(QObject *parent) :
    QObject(parent)
{
    switch (ConfigManager::loggingType()) {
    case LogType::MODCALL:
        writerModcall = new WriterModcall(this);
        break;
    case LogType::FULL:
        Q_FALLTHROUGH();
    case LogType::FULLAREA:
        writerFull = new WriterFull(this);
        break;
    case LogType::SQL:
        writerSQL = new WriterSQL(this);
        m_log_db = writerSQL->getDatabase();
        break;
    }
    loadLogtext();
}

ULogger::~ULogger()
{
    switch (ConfigManager::loggingType()) {
    case LogType::MODCALL:
        writerModcall->deleteLater();
        break;
    case LogType::FULL:
    case LogType::FULLAREA:
        writerFull->deleteLater();
        break;
    case LogType::SQL:
        writerSQL->deleteLater();
        break;
    }
}

void ULogger::logIC(const QString &f_char_name, const QString &f_show_name, const QString &f_ooc_name,
                    const QString &f_ipid, const QString &f_hwid, const QString &f_area_name,
                    const QString &f_message)
{
    QString l_time =
        QDateTime::currentDateTime().toString("ddd MMMM d yyyy | hh:mm:ss");
    QString l_logEntry =
        QString(m_logtext.value("ic") + "\n")
            .arg(l_time, (f_char_name + " " + f_show_name), f_ooc_name, (f_ipid + " " + f_hwid), f_area_name, f_message);
    updateAreaBuffer(f_area_name, l_logEntry);
    {
        if (ConfigManager::loggingType() == LogType::SQL) {
            QSqlQuery query(m_log_db);
            query.prepare("INSERT INTO ic_events (event_time, room_name, char_name, ic_name, message, hwip) VALUES "
                          "(?, ?, ?, ?, ?, (SELECT hwip FROM hwips WHERE hwid=? AND ipid=?))");
            query.addBindValue(l_time);
            query.addBindValue(f_area_name);
            query.addBindValue(f_char_name);
            query.addBindValue(f_show_name);
            query.addBindValue(f_message);
            query.addBindValue(f_hwid);
            query.addBindValue(f_ipid);

            writerSQL->flush(query);
        }
    }
}

void ULogger::logOOC(const QString &f_char_name, const QString &f_ooc_name, const QString &f_ipid,
                     const QString &f_hwid, const QString &f_area_name, const QString &f_message)
{
    QString l_time =
        QDateTime::currentDateTime().toString("ddd MMMM d yyyy | hh:mm:ss");
    QString l_logEntry =
        QString(m_logtext.value("ooc") + "\n")
            .arg(l_time, f_char_name, f_ooc_name, (f_ipid + " " + f_hwid) , f_area_name, f_message);
    updateAreaBuffer(f_area_name, l_logEntry);
    {
        QSqlQuery query(m_log_db);
        query.prepare("Yolo");
    }
}

void ULogger::logLogin(const QString &f_char_name, const QString &f_ooc_name,
                       const QString &f_moderator_name, const QString &f_ipid,
                       const QString &f_area_name, const bool &f_success)
{
    QString l_time =
        QDateTime::currentDateTime().toString("ddd MMMM d yyyy | hh:mm:ss");
    QString l_success = f_success ? "SUCCESS][" + f_moderator_name
                                  : "FAILED][" + f_moderator_name;
    QString l_logEntry =
        QString(m_logtext.value("login") + "\n")
            .arg(l_time, l_success, f_ipid, f_char_name, f_ooc_name);
    updateAreaBuffer(f_area_name, l_logEntry);
}

void ULogger::logCMD(const QString &f_char_name, const QString &f_ipid, const QString &f_hwid,
                     const QString &f_ooc_name, const QString &f_command,
                     const QStringList &f_args, const QString &f_area_name)
{
    QString l_time =
        QDateTime::currentDateTime().toString("ddd MMMM d yyyy | hh:mm:ss");
    QString l_logEntry;
    // Some commands contain sensitive data, like passwords
    // These must be filtered out
    if (f_command == "login") {
        l_logEntry = QString(m_logtext.value("cmdlogin") + "\n")
                         .arg(l_time, f_area_name, f_char_name, f_ooc_name, (f_ipid + " " + f_hwid));
    }
    else if (f_command == "rootpass") {
        l_logEntry = QString(m_logtext.value("cmdrootpass") + "\n")
                         .arg(l_time, f_area_name, f_char_name, f_ooc_name, (f_ipid + " " + f_hwid));
    }
    else if (f_command == "adduser" && !f_args.isEmpty()) {
        l_logEntry = QString(m_logtext.value("adduser") + "\n")
                         .arg(l_time, f_area_name, f_char_name, f_ooc_name,
                              f_args.at(0), (f_ipid + " " + f_hwid));
    }
    else {
        l_logEntry = QString(m_logtext.value("cmd") + "\n")
                         .arg(l_time, f_area_name, f_char_name, f_ooc_name,
                              f_command, f_args.join(" "), (f_ipid + " " + f_hwid));
    }
    updateAreaBuffer(f_area_name, l_logEntry);
}

void ULogger::logKick(const QString &f_moderator,
                      const QString &f_target_ipid)
{
    QString l_time =
        QDateTime::currentDateTime().toString("ddd MMMM d yyyy | hh:mm:ss");
    QString l_logEntry = QString(m_logtext.value("kick") + "\n")
                             .arg(l_time, f_moderator, f_target_ipid);
    updateAreaBuffer("SERVER", l_logEntry);
}

void ULogger::logBan(const QString &f_moderator, const QString &f_target_ipid,
                     const QString &f_duration)
{
    QString l_time =
        QDateTime::currentDateTime().toString("ddd MMMM d yyyy | hh:mm:ss");
    QString l_logEntry = QString(m_logtext.value("ban") + "\n")
                             .arg(l_time, f_moderator, f_target_ipid, f_duration);
    updateAreaBuffer("SERVER", l_logEntry);
}

void ULogger::logModcall(const QString &f_char_name, const QString &f_ipid,
                         const QString &f_ooc_name,
                         const QString &f_area_name)
{
    QString l_time =
        QDateTime::currentDateTime().toString("ddd MMMM d yyyy | hh:mm:ss");
    QString l_logEvent =
        QString(m_logtext.value("modcall") + "\n")
            .arg(l_time, f_area_name, f_char_name, f_ooc_name, f_ipid);
    updateAreaBuffer(f_area_name, l_logEvent);

    if (ConfigManager::loggingType() == LogType::MODCALL) {
        writerModcall->flush(f_area_name, buffer(f_area_name));
    }
}

void ULogger::logConnectionAttempt(const QString &f_ip_address,
                                   const QString &f_ipid,
                                   const QString &f_hwid)
{
    QString l_time =
        QDateTime::currentDateTime().toString("ddd MMMM d yyyy | hh:mm:ss");
    QString l_logEntry = QString(m_logtext.value("connect") + "\n")
                             .arg(l_time, f_ip_address, f_ipid, f_hwid);
    updateAreaBuffer("SERVER", l_logEntry);
    {
        if (ConfigManager::loggingType() == LogType::SQL) {
            {
                QSqlQuery query(m_log_db);
                query.prepare("INSERT OR IGNORE INTO ipids(ipid, ip_address)"
                              "VALUES(?, ?)");
                query.addBindValue(f_ipid);
                query.addBindValue(f_ip_address);
                writerSQL->flush(query);
            }
            {
                QSqlQuery query(m_log_db);
                query.prepare("INSERT OR IGNORE INTO hwips(hwid, ipid)"
                              "VALUES(?, ?)");
                query.addBindValue(f_hwid);
                query.addBindValue(f_ipid);
                writerSQL->flush(query);
            }
            {
                QSqlQuery query(m_log_db);
                query.prepare("INSERT OR IGNORE INTO connect_events()");
            }
        }
    }
}

void ULogger::loadLogtext()
{
    // All of this to prevent one single clazy warning from appearing.
    for (auto iterator = m_logtext.keyBegin(), end = m_logtext.keyEnd();
         iterator != end; ++iterator) {
        QString l_tempstring = ConfigManager::LogText(iterator.operator*());
        if (!l_tempstring.isEmpty()) {
            m_logtext[iterator.operator*()] = l_tempstring;
        }
    }
}

void ULogger::updateAreaBuffer(const QString &f_area_name,
                               const QString &f_log_entry)
{
    QQueue<QString> l_buffer = m_bufferMap.value(f_area_name);

    if (l_buffer.length() <= ConfigManager::logBuffer()) {
        l_buffer.enqueue(f_log_entry);
    }
    else {
        l_buffer.dequeue();
        l_buffer.enqueue(f_log_entry);
    }
    m_bufferMap.insert(f_area_name, l_buffer);

    if (ConfigManager::loggingType() == LogType::FULL) {
        writerFull->flush(f_log_entry);
    }
    if (ConfigManager::loggingType() == LogType::FULLAREA) {
        writerFull->flush(f_log_entry, f_area_name);
    }
}

QQueue<QString> ULogger::buffer(const QString &f_area_name)
{
    return m_bufferMap.value(f_area_name);
}
