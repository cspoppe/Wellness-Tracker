
#ifndef RUNNINGDATAMODEL_H
#define RUNNINGDATAMODEL_H

#include <QSqlTableModel>
#include "qcustomplot.h"


class runningDataModel : public QSqlTableModel
{
public:
    explicit runningDataModel(QCustomPlot *customPlotDistance, QCustomPlot *customPlotPace, QCustomPlot *customPlotHeartRate, QObject *parent = nullptr, QSqlDatabase db = QSqlDatabase());
    int columnCount(const QModelIndex& parent) const;
    void loadDataFromModel();
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
private:
    QCustomPlot *plotDistance;
    QCustomPlot *plotPace;
    QCustomPlot *plotHeartRate;
    bool dataLoaded;
    int numColumns;
    QVector<double> dateV;
    QVector<double> distanceV;
    QVector<double> timeV;
    QVector<double> caloriesV;
    QVector<double> paceV;
    QVector<double> heartRateV;

    QVector<double> paceMinMax;
    QVector<double> distanceMinMax;
    QVector<double> heartRateMinMax;

    template <typename T> void updateMinMax(const QVector<T> &dataVector, QVector<T> &minMaxVector);
    void updatePlot();
};

#endif // RUNNINGDATAMODEL_H
