
#ifndef WEIGHTTABLE_H
#define WEIGHTTABLE_H

#include <QSqlTableModel>
#include "qcustomplot.h"


class WeightTable : public QSqlTableModel
{
public:
    explicit WeightTable(QCustomPlot *customWeightPlot, QCustomPlot *customBodyFatPlot, QObject *parent = nullptr, QSqlDatabase db = QSqlDatabase());
    int columnCount(const QModelIndex& parent) const;
    void loadDataFromModel();
    void resizeXAxis();
    void resizeXAxis(QDate startDate, QDate endDate);
    QVariant data(const QModelIndex &index, int role) const;
    const QVector<double> * getWeightVectorPtr() const;
    const QVector<double> * getDateVectorPtr() const;

private:
    QCustomPlot *weightPlot;
    QCustomPlot *bodyFatPlot;
    QCPGraph *weightGraph;
    QCPGraph *bodyFatGraph;
    bool dataLoaded;
    int numColumns;
    QVector<double> weightVector;
    double minWeight, maxWeight;
    QVector<double> bodyFatVector;
    double minBodyFat, maxBodyFat;
    QVector<double> dateVector;
    void updatePlot();
};

#endif // WEIGHTTABLE_H
