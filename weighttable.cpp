
#include "weighttable.h"
#include "qsqlrecord.h"
#include "utilityfcns.h"
#include <QDebug>
#include <QString>

#define SEC_IN_DAY (60*60*24)

WeightTable::WeightTable(QCustomPlot *customWeightPlot, QCustomPlot *customBodyFatPlot, QObject *parent, QSqlDatabase db)
    : QSqlTableModel{parent, db}, weightPlot(customWeightPlot), bodyFatPlot(customBodyFatPlot), dataLoaded(false)
{
    weightPlot->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
    bodyFatPlot->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    QSharedPointer<QCPAxisTickerDateTime> dateTicker2(new QCPAxisTickerDateTime);
    dateTicker->setDateTimeFormat("MMM dd yyyy");
    dateTicker2->setDateTimeFormat("MMM dd yyyy");
    weightPlot->xAxis->setTicker(dateTicker);
    weightPlot->xAxis->setLabel("Date");
    weightPlot->yAxis->setLabel("Weight (lbs)");

    bodyFatPlot->xAxis->setTicker(dateTicker);
    bodyFatPlot->xAxis->setLabel("Date");
    bodyFatPlot->yAxis->setLabel("Body Fat %");

    weightGraph = weightPlot->addGraph();
    weightGraph->setPen(QPen(Qt::blue));
    weightGraph->setName("Weight");
    weightGraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,5));
    bodyFatGraph = bodyFatPlot->addGraph();
    bodyFatGraph->setPen(QPen(Qt::red));
    bodyFatGraph->setName("Body Fat");
    bodyFatGraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,5));
}

void WeightTable::loadDataFromModel()
{
    int nRows = rowCount();
    if (dataLoaded)
    {
        // This flag indicates that we have already grabbed all previous data when the application was first launched,
        // and that we only need to add the measurements added today.
        int i = 0;
        QString dateString = record(i).value("date").toString();
        /*
         * First we need to check whether today's data has already been logged.
         * If the user already logged today's weight once and is just updating it, we don't want to add to our vectors.
         * We simply want to replace the last entry.
         * */
        double dateDouble = QDateTime::fromString(dateString,"yyyy-MM-dd").toSecsSinceEpoch();
        double weight = record(i).value("weight").toDouble();
        double bodyFat = record(i).value("bodyfat").toDouble();
        qDebug() << "Nth weight data points:" << Qt::endl;
        qDebug() << "date: " << dateString << ", weight: " << QString::number(weight) << ", body fat: " << QString::number(bodyFat) << Qt::endl;
        int n = dateVector.size()-1;
        if (dateDouble == dateVector[n])
        {
            weightVector[n] = weight;
            bodyFatVector[n] = bodyFat;
        }
        else
        {
            dateVector.push_back(dateDouble);
            weightVector.push_back(weight);
            bodyFatVector.push_back(bodyFat);
            n++;
        }
        // update min and max values
        if (weightVector[n] < minWeight) minWeight = weightVector[n];
        if (weightVector[n] > maxWeight) maxWeight = weightVector[n];
        if (bodyFatVector[n] < minBodyFat) minBodyFat = bodyFatVector[n];
        if (bodyFatVector[n] > maxBodyFat) maxBodyFat = bodyFatVector[n];
    }
    else
    {
        // We need to load all previous data.
        minBodyFat = 100;
        maxBodyFat = 0;
        minWeight = 1000;
        maxWeight = 0;
        for (int i{0}; i < nRows; ++i)
        {
            weightVector.push_back(record(i).value("weight").toDouble());
            if (weightVector[i] < minWeight) minWeight = weightVector[i];
            if (weightVector[i] > maxWeight) maxWeight = weightVector[i];
            bodyFatVector.push_back(record(i).value("bodyfat").toDouble());
            if (bodyFatVector[i] < minBodyFat) minBodyFat = bodyFatVector[i];
            if (bodyFatVector[i] > maxBodyFat) maxBodyFat = bodyFatVector[i];
            QString dateString = record(i).value("date").toString();
            dateVector.push_back(QDateTime::fromString(dateString,"yyyy-MM-dd").toSecsSinceEpoch());
            //qDebug() << "i: " << QString::number(i) << ", date: " << dateString << Qt::endl;
        }

        dataLoaded = true;
    }
    updatePlot();
}

void WeightTable::updatePlot()
{
    weightGraph->setData(dateVector,weightVector);
    bodyFatGraph->setData(dateVector,bodyFatVector);
    weightPlot->xAxis->setRange(dateVector[0],dateVector[dateVector.size()-1]+SEC_IN_DAY);
    bodyFatPlot->xAxis->setRange(dateVector[0],dateVector[dateVector.size()-1]+SEC_IN_DAY);
    weightPlot->yAxis->setRange(minWeight-5,maxWeight+5);
    bodyFatPlot->yAxis->setRange(minBodyFat-2,maxBodyFat+2);
    weightPlot->replot();
    bodyFatPlot->replot();
}

int WeightTable::columnCount(const QModelIndex& parent) const
{
    return QSqlTableModel::columnCount(parent) + 1;
}

QVariant WeightTable::data(const QModelIndex &index, int role) const
{
    if ((index.column() == 4) && (role == Qt::DisplayRole)){
        QModelIndex i1 = index.siblingAtColumn(2);
        QModelIndex i2 = index.siblingAtColumn(3);
        double weight = QSqlTableModel::data(i1,role).toDouble();
        double bf_percent = QSqlTableModel::data(i2,role).toDouble();
        //qDebug() << "weight: " << weight << Qt::endl;
        //qDebug() << "bodyfat %: " << bf_percent << Qt::endl;
        double fat = weight*0.01*bf_percent;
        QString str = QString::number(fat,'g',3);
        return QVariant(str);
    }
    else {
        return QSqlTableModel::data(index,role);
    }
}

const QVector<double>* WeightTable::getWeightVectorPtr() const
{
    return &weightVector;
}

const QVector<double>* WeightTable::getDateVectorPtr() const
{
    return &dateVector;
}

void WeightTable::resizeXAxis()
{
    double startDay = dateVector[0];
    double endDay = dateVector[dateVector.size()-1] + SEC_IN_DAY;
    weightPlot->xAxis->setRange(startDay,endDay);
    bodyFatPlot->xAxis->setRange(startDay,endDay);
    weightPlot->replot();
    bodyFatPlot->replot();
}

void WeightTable::resizeXAxis(QDate startDate, QDate endDate)
{
    QString startDateString = dateToString(startDate);
    QString endDateString = dateToString(endDate);
    double startDateDouble = QDateTime::fromString(startDateString,"yyyy-MM-dd").toSecsSinceEpoch();
    double endDateDouble = QDateTime::fromString(endDateString,"yyyy-MM-dd").toSecsSinceEpoch() + SEC_IN_DAY;
    weightPlot->xAxis->setRange(startDateDouble,endDateDouble);
    bodyFatPlot->xAxis->setRange(startDateDouble,endDateDouble);
    weightPlot->replot();
    bodyFatPlot->replot();
}
