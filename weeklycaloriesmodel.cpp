
#include "weeklycaloriesmodel.h"
#include "utilityfcns.h"

#include <QDateTime>
#include <QTime>

weeklyCaloriesModel::weeklyCaloriesModel(bool loggingStatus, QObject *parent)
    : QStandardItemModel{parent}, loggingCompletedTodayFlag(loggingStatus)
{
    setColumnCount(11);
    setHeaderData(0,Qt::Horizontal,tr("Start"));
    setHeaderData(1,Qt::Horizontal,tr("End"));
    setHeaderData(2,Qt::Horizontal,tr("Mon"));
    setHeaderData(3,Qt::Horizontal,tr("Tues"));
    setHeaderData(4,Qt::Horizontal,tr("Wed"));
    setHeaderData(5,Qt::Horizontal,tr("Thur"));
    setHeaderData(6,Qt::Horizontal,tr("Fri"));
    setHeaderData(7,Qt::Horizontal,tr("Sat"));
    setHeaderData(8,Qt::Horizontal,tr("Sun"));
    setHeaderData(9,Qt::Horizontal,tr("Tot. Cal."));
    setHeaderData(10,Qt::Horizontal,tr("Tot. Delta"));
}

void weeklyCaloriesModel::loadData(double tdee, const QVector<double> *dates,const QVector<double> *calories)
{

    TDEE = tdee;
    // this function return 1 for Monday, 2 for Tues.,
    // For our purposes, we want to begin with 0 for Monday.
    QDate today = QDate::currentDate();
    int dayOfWeek = today.dayOfWeek()-1;
    QDate monday = today.addDays(-dayOfWeek);
    //double currentDate = QDateTime(today,QTime()).toSecsSinceEpoch();
    double startOfWeek = convertDateToDouble(monday);
    /* if the dates vector does not include today (i.e., today's log has not been completed),
     * then we need to add an entry for today so that we can update the calorie count as we
     * fill out the log during the day.
     */



    int i = dates->size()-1;

    while (startOfWeek > (*dates)[0] - 7*SEC_IN_DAY + 60)
    {
        // working backwards, find the most recent Monday in the dates vector.
        // if that monday is not in the vector, we want the first day that occurs after it
        while ( (i > 0) && ((*dates)[i] > startOfWeek) && ((*dates)[i-1] > startOfWeek - 60) )
        {
            --i;
        }
        QList<QStandardItem*> newRow;
        // Add beginning and end dates
        QStandardItem *startDate = new QStandardItem(monday.toString("MMM d"));
        QStandardItem *endDate = new QStandardItem(monday.addDays(6).toString("MMM d"));
        newRow.push_back(startDate);
        newRow.push_back(endDate);
        double total_cal = 0;
        QDate day = monday;
        int nDays = 0;
        int index = i;
        for (int j = 0; j < 7; ++j)
        {
            if ( (index < dates->size()) && areSameDay( (*dates)[index],day) )
            {
                total_cal += (*calories)[index];
                QStandardItem *item = new QStandardItem(QString::number( (*calories)[index] - TDEE));
                newRow.push_back(item);
                ++index;
                ++nDays;
            }
            else
            {
                // complete data was not logged for this day, so we simply push "N/A" to the table
                QStandardItem *item;
                if (day < today) {
                    item = new QStandardItem(QString("N/A"));
                } else {
                    item = new QStandardItem(QString("--"));
                }
                newRow.push_back(item);
            }
            if (day == today) todayColumn = j+2;
            day = day.addDays(1);
        }
        double total_delta = total_cal - nDays*TDEE;
        QStandardItem *totalCalories = new QStandardItem(QString::number( total_cal ));
        QStandardItem *totalDelta = new QStandardItem(QString::number( total_delta ));
        newRow.push_back(totalCalories);
        newRow.push_back(totalDelta);

        appendRow(newRow);

        // step back one week
        monday = monday.addDays(-7);
        startOfWeek = convertDateToDouble(monday);
    }

    // save index for today's calories so we can update it on the fly.

}


QVariant weeklyCaloriesModel::data(const QModelIndex &index, int role) const
{
    // for calorie totals that are under TDEE, display calorie count as green
    // if total is over TDEE, display as red
    if ((role == Qt::BackgroundRole) && (index.column() > 1))
    {
        if (index.row() == 0 && index.column() == todayColumn && !loggingCompletedTodayFlag) return colorToday.lighter(170);
        QString str = QStandardItemModel::data(index,Qt::DisplayRole).toString();
        if (str != "N/A" && str != "--")
        {
            int calorie_delta = str.toInt();
            if (index.column() != 9)
            {
                if (calorie_delta <= 0)
                {
                    // scale the brightness of the background by how far underneath the TDEE we are
                    // A deficit of 500 calories is the maximum
                    double deficit = -calorie_delta;
                    deficit = (deficit < 500) ? deficit : 500;
                    int lightness = static_cast<int>(200 - deficit/5);
                    return colorUnder.lighter(lightness);
                }
                else
                {
                    double excess = calorie_delta;
                    excess = (excess < 500) ? excess : 500;
                    int lightness = static_cast<int>(200 - excess/5);
                    return colorOver.lighter(lightness);
                }
            }
        }
    }
    return QStandardItemModel::data(index,role);
}

void weeklyCaloriesModel::updateTodaysCount(double calories)
{
    QStandardItem *item = new QStandardItem(QString::number( static_cast<int>(calories - TDEE)));
    setItem(0,todayColumn,item);
    updateWeeksDelta();
}

void weeklyCaloriesModel::updateWeeksDelta()
{
    //
    int calories_total = 0;
    for (int col = 2; col < 9; ++col)
    {
        QString str = QStandardItemModel::data(index(0,col),Qt::DisplayRole).toString();
        if (str != "N/A" && str != "--")
        {
            calories_total += str.toInt();
        }
    }
    QStandardItem *item = new QStandardItem(QString::number( calories_total ));
    setItem(0,10,item);
}

void weeklyCaloriesModel::setLoggingStatus(bool status)
{
    loggingCompletedTodayFlag = status;
    QModelIndex index = QStandardItemModel::index(0,todayColumn);
    QVariant color = QStandardItemModel::data(index,Qt::BackgroundRole);
    setData(index,color,Qt::BackgroundRole);
}
