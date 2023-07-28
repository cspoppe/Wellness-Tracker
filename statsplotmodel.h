
#ifndef STATSPLOTMODEL_H
#define STATSPLOTMODEL_H
#include <QObject>
#include "qcustomplot.h"



class statsPlotModel : public QObject
{
    Q_OBJECT

public:
    statsPlotModel(QCustomPlot *customPlot, QObject *parent = nullptr);
    void getDataFromDB();
    enum plotNutrient {plotFoodCalories,plotExerciseCalories,plotNetCalories,plotTotFat,plotSatFat,plotCholesterol,plotSodium,plotCarbs,plotFiber,plotSugar,plotProtein};
    void addNutritionPlot(plotNutrient);
    void removeNutritionPlot(plotNutrient);
    void refreshPlot();
    void addCheckBoxToGroup();
    void setCheckBoxColors();
    void resizeXAxis();
    void resizeXAxis(QDate startDate, QDate endDate);
    void setCompleteDaysFlag(bool state) {plotCompleteDays = state;}
    bool getCompleteDaysStatus() {return plotCompleteDays;}
    void updateDaysData(QDate qdate, QVector<double> updatedStats);
    int findDateIndex(QDate qdate,QVector<double> dates);
    int findDateIndex(double date,QVector<double> dates);
    int insertDate(QDate qdate, QVector<double> &dates);
    double getTodaysNetCalories();
    void setLoggingCompleted(bool status);

    QButtonGroup *checkBoxGroup;
    QVector<QString> getCompletedDateStrings() const;
    const QVector<double> * getCompletedNetCaloriesPtr() const;
    const QVector<double> * getCompletedDatesPtr() const;

public slots:
    void toggleLegend(int show);

private:
    QCustomPlot *plot;
    QVector<double> dates;
    QVector<QString> date_strings;
    QVector<double> food_calories;
    QVector<double> exercise_calories;
    QVector<double> net_calories;
    QVector<double> tot_fat;
    QVector<double> sat_fat;
    QVector<double> cholesterol;
    QVector<double> sodium;
    QVector<double> carbs;
    QVector<double> fiber;
    QVector<double> sugar;
    QVector<double> protein;

    QVector<double> completed_dates;
    QVector<QString> completed_date_strings;
    QVector<double> completed_food_calories;
    QVector<double> completed_exercise_calories;
    QVector<double> completed_net_calories;
    QVector<double> completed_tot_fat;
    QVector<double> completed_sat_fat;
    QVector<double> completed_cholesterol;
    QVector<double> completed_sodium;
    QVector<double> completed_carbs;
    QVector<double> completed_fiber;
    QVector<double> completed_sugar;
    QVector<double> completed_protein;

    QFont tickLabelFont;
    QFont labelFont;

    std::unordered_map<plotNutrient,Qt::GlobalColor> plotColors;
    std::unordered_map<plotNutrient,QVector<double>*> nutrientStats;
    std::unordered_map<plotNutrient,QVector<double>*> completeNutrientStats;
    std::unordered_map<plotNutrient,QCPGraph*> activePlots;
    std::unordered_map<plotNutrient,QString> legendNames;
    QVector<plotNutrient> leftYAxisSignals;
    double plotStartDay;
    double plotEndDay;
    void setVectorsToZero();
    void populateCompletedVectors();
    void pushDataToCompletedVectors();
    void pushDataToCompletedVectors(int date_ptr);
    void popDataFromCompletedVectors();
    void autoResizeAxes();
    bool plotCompleteDays;
};

#endif // STATSPLOTMODEL_H
