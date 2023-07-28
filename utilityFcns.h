
#ifndef UTILITYFCNS_H
#define UTILITYFCNS_H

#include <QObject>
#include <QDateTime>

#define SEC_IN_DAY (60*60*24)

enum dateFormat {displayFormat, dBFormat};

QString dateToString(QDate date, dateFormat format = dBFormat);
double roundDec(double val, int nDec);
double convertDateToDouble(QDate date);
double convertDateToDouble(QString dateString);
bool areSameDay(double day1, double day2);
bool areSameDay(QDate day1, double day2);
bool areSameDay(double day1, QDate day2);
int mealLabelMap(QString meal);

template <typename T>
T vectorMin(QVector<T> vec)
{
    return *std::min_element(vec.begin(),vec.end());
}

template <typename T>
T vectorMax(QVector<T> vec)
{
    return *std::max_element(vec.begin(),vec.end());
}

#endif // UTILITYFCNS_H
