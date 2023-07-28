
#ifndef SQLMEALMODEL_H
#define SQLMEALMODEL_H

#include <QSqlQueryModel>

class sqlMealModel : public QSqlQueryModel
{
    Q_OBJECT

public:
    explicit sqlMealModel(int meal, QString date, QObject *parent = nullptr);
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    int columnCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    void refresh();
    std::vector<double> getNutritionStats() {return nutrition_stats;}
    void setDate(QString date) {currentDate = date;}
    QString getDate() {return currentDate;}
private:
    bool setServingSize(double servingSize, int meal, int food_id);
    int numColumns;
    int mealNo;
    QString currentDate;
    std::vector<double> nutrition_stats;
    bool updateNutritionTotals();

};

#endif // SQLMEALMODEL_H
