
#include "runningdatamodel.h"
#include "qsqlrecord.h"
#include "utilityfcns.h"

runningDataModel::runningDataModel(QCustomPlot *customPlotDistance, QCustomPlot *customPlotPace, QCustomPlot *customPlotHeartRate, QObject *parent, QSqlDatabase db)
    : QSqlTableModel{parent, db}, plotDistance(customPlotDistance), plotPace(customPlotPace), plotHeartRate(customPlotHeartRate), dataLoaded(false)
{
    tickLabelFont.setPointSize(12);
    labelFont.setPointSize(14);
    labelFont.setStyleStrategy(QFont::PreferAntialias);

    // initially we set extremely large min values and small max values
    distanceMinMax = {1000.0,0.0};
    paceMinMax = {1000.0,0.0};
    heartRateMinMax = {1000,0};

    // initialize plots
    plotDistance->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
    plotPace->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
    plotHeartRate->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    dateTicker->setDateTimeFormat("MMM dd yyyy");
    plotDistance->xAxis->setTicker(dateTicker);
    plotPace->xAxis->setTicker(dateTicker);
    plotHeartRate->xAxis->setTicker(dateTicker);
    //plotDistance->xAxis->setLabel("Date");
    //plotPace->xAxis->setLabel("Date");
    //plotHeartRate->xAxis->setLabel("Date");
    plotDistance->yAxis->setLabel("Miles");
    plotPace->yAxis->setLabel("Min/mi");
    plotHeartRate->yAxis->setLabel("BPM");

    plotDistance->xAxis->setTickLabelFont(tickLabelFont);
    plotDistance->yAxis->setTickLabelFont(tickLabelFont);
    plotDistance->yAxis->setLabelFont(labelFont);

    plotPace->xAxis->setTickLabelFont(tickLabelFont);
    plotPace->yAxis->setTickLabelFont(tickLabelFont);
    plotPace->yAxis->setLabelFont(labelFont);

    plotHeartRate->xAxis->setTickLabelFont(tickLabelFont);
    plotHeartRate->yAxis->setTickLabelFont(tickLabelFont);
    plotHeartRate->yAxis->setLabelFont(labelFont);

    plotDistance->addGraph();
    plotDistance->graph(0)->setPen(QPen(Qt::blue));
    plotDistance->graph(0)->setName("Distance");
    plotDistance->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,5));

    plotPace->addGraph();
    plotPace->graph(0)->setPen(QPen(Qt::red));
    plotPace->graph(0)->setName("Pace");
    plotPace->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,5));

    plotHeartRate->addGraph();
    plotHeartRate->graph(0)->setPen(QPen(Qt::darkGreen));
    plotHeartRate->graph(0)->setName("Heart Rate");
    plotHeartRate->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,5));
}

void runningDataModel::loadDataFromModel()
{
    int nRows = rowCount();
    if (dataLoaded)
    {
        // This flag indicates that we have already grabbed all previous data when the application was first launched,
        // and that we only need to add the measurements added today.
        int i = 0;
        QString dateString = record(i).value("date").toString();
        double dateDouble = QDateTime::fromString(dateString,"yyyy-MM-dd").toSecsSinceEpoch();

        double time = record(i).value("duration_min").toDouble();
        double calories = record(i).value("calories").toDouble();
        double distance = record(i).value("distance_mi").toDouble();
        int heart_rate = record(i).value("avg_heart_rate").toInt();

        qDebug() << "Just added running data points:" << Qt::endl;
        qDebug() << "date: " << dateString << ", time: " << QString::number(time) << ", distance: " << QString::number(distance) << Qt::endl;

        dateV.push_back(dateDouble);
        distanceV.push_back(distance);
        timeV.push_back(time);
        caloriesV.push_back(calories);
        heartRateV.push_back(heart_rate);
        paceV.push_back(timeV[i]/distanceV[i]);

        // update min and max values
        updateMinMax(distanceV,distanceMinMax);
        updateMinMax(heartRateV,heartRateMinMax);
        updateMinMax(paceV,paceMinMax);
    }
    else
    {
        // We need to load all previous data.
        for (int i{0}; i < nRows; ++i)
        {
            QString dateString = record(i).value("date").toString();

            dateV.push_back(QDateTime::fromString(dateString,"yyyy-MM-dd").toSecsSinceEpoch());
            distanceV.push_back(record(i).value("distance_mi").toDouble());
            timeV.push_back(record(i).value("duration_min").toDouble());
            caloriesV.push_back(record(i).value("calories").toDouble());
            heartRateV.push_back(record(i).value("avg_heart_rate").toDouble());
            paceV.push_back(timeV[i]/distanceV[i]);

            updateMinMax(distanceV,distanceMinMax);
            updateMinMax(heartRateV,heartRateMinMax);
            updateMinMax(paceV,paceMinMax);
        }

        dataLoaded = true;
    }
    updatePlot();
}

void runningDataModel::updatePlot()
{
    plotDistance->graph(0)->setData(dateV,distanceV);
    plotPace->graph(0)->setData(dateV,paceV);
    plotHeartRate->graph(0)->setData(dateV,heartRateV);

    plotDistance->xAxis->setRange(dateV[0],dateV[dateV.size()-1]+SEC_IN_DAY);
    plotPace->xAxis->setRange(dateV[0],dateV[dateV.size()-1]+SEC_IN_DAY);
    plotHeartRate->xAxis->setRange(dateV[0],dateV[dateV.size()-1]+SEC_IN_DAY);

    plotDistance->yAxis->setRange(distanceMinMax[0]-1,distanceMinMax[1]+1);
    plotPace->yAxis->setRange(paceMinMax[0]-2,paceMinMax[1]+2);
    plotHeartRate->yAxis->setRange(heartRateMinMax[0]-10,heartRateMinMax[1]+10);

    plotDistance->replot();
    plotPace->replot();
    plotHeartRate->replot();
}

template <typename T>
void runningDataModel::updateMinMax(const QVector<T> &dataVector, QVector<T> &minMaxVector)
{
    int n = dataVector.size()-1;
    if (dataVector[n] < minMaxVector[0]) minMaxVector[0] = dataVector[n];
    if (dataVector[n] > minMaxVector[1]) minMaxVector[1] = dataVector[n];
}

int runningDataModel::columnCount(const QModelIndex& parent) const
{
    return QSqlTableModel::columnCount(parent) + 1;
}

QVariant runningDataModel::data(const QModelIndex &index, int role) const
{
    if ((index.column() == 6) && (role == Qt::DisplayRole)) {
        // return pace
        double distance_mi = QSqlTableModel::data(index.siblingAtColumn(2),role).toDouble();
        double duration_minutes = QSqlTableModel::data(index.siblingAtColumn(3),role).toDouble();
        double pace = duration_minutes/distance_mi;
        int pace_minutes = static_cast<int>(std::floor(pace));
        int pace_seconds = static_cast<int>(std::round(60*(pace - pace_minutes)));
        QString pace_str = QString::number(pace_minutes) + ":";
        if (pace_seconds < 10) pace_str += "0";
        pace_str += QString::number(pace_seconds);
        return QVariant(pace_str);
    }
    else {
        return QSqlTableModel::data(index,role);
    }
}


QVariant runningDataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((orientation == Qt::Horizontal) && (role == Qt::DisplayRole))
    {
        QVariant ret;
        switch (section){
        case 1:
            ret = tr("Date");
            break;
        case 2:
            ret = tr("Miles");
            break;
        case 3:
            ret = tr("Minutes");
            break;
        case 4:
            ret = tr("Heart Rate");
            break;
        case 5:
            ret = tr("Calories");
            break;
        case 6:
            ret = tr("Pace (min/mi)");
            break;
        }

        return ret;

    }
    else
    {
        return QSqlTableModel::headerData(section, orientation, role);
    }
}
