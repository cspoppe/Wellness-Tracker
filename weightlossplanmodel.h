
#ifndef WEIGHTLOSSPLANMODEL_H
#define WEIGHTLOSSPLANMODEL_H

#include <QObject>
#include <QGroupBox>
#include <QDate>
#include "statsplotmodel.h"


class weightLossPlanModel : public QObject
{
    Q_OBJECT
public:
    weightLossPlanModel(statsPlotModel *stats, QCustomPlot *weightPlot, QCustomPlot *weeklyPlot, bool loggingCompleted);
    void loadWeightLossPlan();
    void setWeightLossPlan(QDate startDate, double tdee, double weight, double lossRate);
    void setPlanSummaryFields();
    void calculateProgress();
    void setWeightVectorPointers(const QVector<double> *weight, const QVector<double> *dates);
    void setLoggingCompletedFlag(bool status);
    void updateTodaysCaloriesBar();

signals:
    void setPlanSummary(QDate start_date, QVector<double> plan_values);
    void setProgressSummary(QVector<double> progress_values);
private:
    statsPlotModel *statsModel;
    QDate planStartDate;
    double TDEE_estimated;
    double goalWeight;
    double weight_loss_rate_weekly;
    double daily_caloric_deficit;
    void calculateWeightLossFit();
    void plotWeeklyCalories();

    const QVector<double> *weightVector;
    const QVector<double> *weightDatesVector;

    QVector<double> tickToday;
    QVector<double> todayCalories;
    QVector<double> weeklyExcessCaloriesPlotVector;
    QVector<double> weeklyDeficitCaloriesPlotVector;
    QVector<double> weeklyNetCaloriesPlotVector;

    int todayIndex;
    bool loggingCompletedTodayFlag;

    /*
    QVector<double> netDateVector;
    QVector<double> excessDateVector;
    QVector<double> deficitDateVector;
    */

    double averageDailyCalories;
    double totalCalorieDelta;
    double averageDailyDelta;
    double estimatedWeightChange;
    double actualWeightChange;
    double actualWeightChangeRate;
    double calculatedTDEE;
    double daysToGoal;

    double weight_fit_m;
    double weight_fit_b;

    double weight_change_lbs_per_day;

    QCustomPlot *weightLossPlot;
    QCustomPlot *weeklyWeightPlot;
    QCPBars *todayCaloriesBar;
    QCPBars *weeklyNetCaloriesBar;
    QCPBars *weeklyExcessCaloriesBar;
    QCPBars *weeklyDeficitCaloriesBar;
    QCPGraph *weightGraph;
    QCPGraph *weightFitGraph;
    QCPGraph *TDEEGraph;
    double minWeight;
    double maxWeight;
};

#endif // WEIGHTLOSSPLANMODEL_H
