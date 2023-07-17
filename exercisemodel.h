
#ifndef EXERCISEMODEL_H
#define EXERCISEMODEL_H

#include <QSqlQueryModel>



class exerciseModel : public QSqlQueryModel
{
    Q_OBJECT

public:
    explicit exerciseModel(QString date, QObject *parent = nullptr);
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    void refresh();
    void setDate(QString date) {currentDate = date;}
    double getCalories() {return total_calories;}
private:
    bool setDuration(double duration,int exercise_id);
    bool setCalories(double calories,int exercise_id);
    double total_calories;
    QString currentDate;
    bool updateCaloriesTotal();
};

#endif // EXERCISEMODEL_H
