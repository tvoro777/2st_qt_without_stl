#include "opener_filter.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>

opener_filter::opener_filter(QObject *parent) : QObject(parent)
{
}

QList<QStringList> opener_filter::readCsv(const QString &filePath) {
    QList<QStringList> data;
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file:" << filePath;
        return data;
    }

    QTextStream in(&file);
    bool isPipeFormat = true;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();

        bool containsPipe = line.contains('|');
        bool containsSemicolon = line.contains(';');

        QStringList row;

        if (containsPipe) {
            line = line.mid(1, line.length() - 2);
            row = line.split('|', Qt::KeepEmptyParts);
        }
        else if (containsSemicolon) {
            row = line.split(';', Qt::KeepEmptyParts);
        }
        else if (line.contains(':')) {
            row = line.split(':', Qt::KeepEmptyParts);
        }

        for (QString &field : row) {
            field = field.trimmed();
        }

        if (!row.isEmpty()) {
            data.append(row);
        }
    }

    file.close();
    return data;
}

QList<QStringList> opener_filter::filterByFrequency(const QList<QStringList> &data, double minFreq, double maxFreq)
{
    QList<QStringList> filteredByFreqs;
    if (data.isEmpty())
        return filteredByFreqs;

    filteredByFreqs.append(data.first());

    const int freqColumn = 0;

    for (int i = 1; i < data.size(); ++i) {
        const QStringList &row = data[i];
        if (row.size() > freqColumn) {
            QString freqStr = row[freqColumn];

            freqStr.remove(QRegularExpression("[^0-9.]"));
            bool ok = false;
            double freq = freqStr.toDouble(&ok);

            if (ok && freq >= minFreq && freq <= maxFreq) {
                filteredByFreqs.append(row);
            } else {
                // qDebug() << "Строка не прошла фильтр. Частота:" << freqStr << "->" << freq << "ok:" << ok;
            }
        }
    }

    // qDebug() << "Найдено строк, соответствующих фильтру:" << filteredByFreqs.size() - 1;
    return filteredByFreqs;
}

QList<QStringList> opener_filter::filterByTime(const QList<QStringList> &data, const QString &currentDataTime)
{
    QList<QStringList> filteredByTime;
    if (data.isEmpty())
        return filteredByTime;

    filteredByTime.append(data.first());

    const int timeColumn = 1;
    const int dayColumn = 2;

    QDateTime currentDateTime = QDateTime::fromString(currentDataTime, "yyyy-MM-dd HH:mm:ss");
    if (!currentDateTime.isValid()) {
        qWarning() << "Неверный формат времени:" << currentDataTime;
        return filteredByTime;
    }

    QTime currentTime = currentDateTime.time();
    int currentTimeInt = currentTime.hour() * 100 + currentTime.minute();
    int currentDayOfWeek = currentDateTime.date().dayOfWeek();

    QMap<QString, int> dayMap = {
        {"Mo", 1}, {"Tu", 2}, {"We", 3}, {"Th", 4},
        {"Fr", 5}, {"Sa", 6}, {"Su", 7}
    };

    for (int i = 1; i < data.size(); ++i) {
        const QStringList &row = data[i];
        if (row.size() <= dayColumn) continue;

        QString timeRangeStr = row[timeColumn].trimmed();
        QString dayStr = row[dayColumn].trimmed();

        bool dayMatch = dayStr.isEmpty();

        if (!dayMatch) {
            QRegularExpression digitsOnly("^[1-7]+$");
            if (digitsOnly.match(dayStr).hasMatch()) {
                for (QChar ch : dayStr) {
                    if (ch.digitValue() == currentDayOfWeek) {
                        dayMatch = true;
                        break;
                    }
                }
            } else {
                QStringList dayItems = dayStr.split(',', Qt::SkipEmptyParts);
                for (const QString &item : dayItems) {
                    QString trimmedItem = item.trimmed();
                    QStringList dayParts = trimmedItem.split('-');

                    if (dayParts.size() == 1) {
                        if (dayMap.contains(dayParts[0]) && dayMap[dayParts[0]] == currentDayOfWeek) {
                            dayMatch = true;
                            break;
                        }
                    }
                    else if (dayParts.size() == 2) {
                        QString start = dayParts[0].trimmed();
                        QString end = dayParts[1].trimmed();

                        if (dayMap.contains(start) && dayMap.contains(end)) {
                            int startDay = dayMap[start];
                            int endDay = dayMap[end];

                            if (startDay <= endDay) {
                                if (currentDayOfWeek >= startDay && currentDayOfWeek <= endDay) {
                                    dayMatch = true;
                                    break;
                                }
                            } else {
                                if (currentDayOfWeek >= startDay || currentDayOfWeek <= endDay) {
                                    dayMatch = true;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }

        if (!dayMatch)
            continue;

        if (timeRangeStr == "0000-2400") {
            filteredByTime.append(row);
            continue;
        }

        QRegularExpression timeRegex("^(\\d{4})-(\\d{4})$");
        QRegularExpressionMatch match = timeRegex.match(timeRangeStr);

        if (match.hasMatch()) {
            bool ok1 = false, ok2 = false;
            int startTime = match.captured(1).toInt(&ok1);
            int endTime = match.captured(2).toInt(&ok2);

            if (ok1 && ok2) {
                if ((startTime <= endTime && currentTimeInt >= startTime && currentTimeInt <= endTime) ||
                    (startTime > endTime && (currentTimeInt >= startTime || currentTimeInt <= endTime))) {
                    filteredByTime.append(row);
                }
            } else {
                qDebug() << "Строка не соответствует формату времени:" << timeRangeStr;
            }
        } else {
            qDebug() << "Строка не соответствует формату времени:" << timeRangeStr;
        }
    }

    qDebug() << "Найдено строк, соответствующих фильтру:" << filteredByTime.size() - 1;
    return filteredByTime;
}

QList<QStringList> opener_filter::filterByFrequencySTL(const QList<QStringList> &data, double minFreq, double maxFreq) {
    QList<QStringList> filteredByFreqs;
    if (data.isEmpty()) return filteredByFreqs;

    filteredByFreqs.append(data.first());

    static QRegularExpression re("[^0-9.]");
    const int freqColumn = 0;

    std::copy_if(data.begin() + 1, data.end(), std::back_inserter(filteredByFreqs),
                 [&](const QStringList& row) {
                     if (row.size() <= freqColumn) return false;
                     QString freqStr = row[freqColumn];
                     freqStr.remove(re);
                     bool ok = false;
                     double freq = freqStr.toDouble(&ok);
                     return ok && freq >= minFreq && freq <= maxFreq;
                 });

    // qDebug() << "Найдено строк, соответствующих фильтру:" << filteredByFreqs.size() - 1;
    return filteredByFreqs;
}


