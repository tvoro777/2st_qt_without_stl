#ifndef OPENER_FILTER_H
#define OPENER_FILTER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QList>
#include <QDateTime>
#include <QTime>

class opener_filter : public QObject
{
    Q_OBJECT
public:
    explicit opener_filter(QObject *parent = nullptr);

    QList<QStringList> readCsv(const QString &filePath);
    QList<QStringList> filterByFrequency(const QList<QStringList> &data, double minFreq, double maxFreq);
    QList<QStringList> filterByFrequencySTL(const QList<QStringList> &data, double minFreq, double maxFreq);
    QList<QStringList> filterByTime(const QList<QStringList> &data, const QString &currentDataTime);

};

#endif // OPENER_FILTER_H

