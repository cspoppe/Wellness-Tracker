
#ifndef WEEKLYCALORIESMODEL_H
#define WEEKLYCALORIESMODEL_H

#include <QStandardItemModel>



class weeklyCaloriesModel : public QStandardItemModel
{
public:
    explicit weeklyCaloriesModel(bool loggingStatus, QObject *parent = nullptr);
    void loadData(double tdee, const QVector<double> *dates,const QVector<double> *calories);
    void setTDEE(double TDEE_input) {TDEE = TDEE_input;}

private:
    double TDEE;
    int todayColumn;
    QColor colorUnder = QColor(0,255,0);
    QColor colorOver = QColor(255,0,0);
    QColor colorToday = QColor(255,108,34);
    bool loggingCompletedTodayFlag;
    void updateWeeksDelta();
    // QAbstractItemModel interface
public:
    virtual QVariant data(const QModelIndex &index, int role) const override;
    void updateTodaysCount(double calories);
    void setLoggingStatus(bool status);
};

#endif // WEEKLYCALORIESMODEL_H
