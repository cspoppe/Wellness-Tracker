#include "utilityfcns.h"

QString dateToString(QDate date, dateFormat format)
{
    if (format == displayFormat){
        return date.toString("ddd MMM d yyyy");
    } else {
        return date.toString("yyyy-MM-dd");
    }
}

double roundDec(double val, int nDec)
{
    double ret;
    double multiplier = std::pow(10.0,nDec);
    ret = std::ceil(val * multiplier)/multiplier;

    return ret;
}

double convertDateToDouble(QDate date)
{
    return QDateTime(date,QTime(0,0)).toSecsSinceEpoch();
}

double convertDateToDouble(QString dateString)
{

    return QDateTime::fromString(dateString,"yyyy-MM-dd").toSecsSinceEpoch();
}

// This function determines if the two days, given in seconds since the start of year 1970 (UTC), are the same day.
// This function allows for some margin of error of a few seconds just in case there are rounding issues:
// e.g., the two days are the same but day 2 is 0.1 seconds larger than day 1
bool areSameDay(double day1, double day2)
{
    double margin_sec = 60; // margin of error allowed, in seconds.

    return (fabs(day1-day2) < margin_sec);
}

bool areSameDay(QDate day1, double day2)
{
    // first convert the QDate to a double.
    double day1Double = convertDateToDouble(day1);
    return areSameDay(day1Double, day2);
}

bool areSameDay(double day1, QDate day2)
{
    // first convert the QDate to a double.
    double day2Double = convertDateToDouble(day2);
    return areSameDay(day1, day2Double);
}

int mealLabelMap(QString meal)
{
    std::unordered_map<QString, int> mealMap =
        {
            {"Breakfast",0},
            {"Lunch",1},
            {"Dinner",2},
            {"Snacks",3}
        };

    return mealMap[meal];
}
