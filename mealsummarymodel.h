
#ifndef MEALSUMMARYMODEL_H
#define MEALSUMMARYMODEL_H

#include <QStandardItemModel>
#include <QSqlQueryModel>



class mealSummaryModel : public QStandardItemModel
{
public:
    explicit mealSummaryModel(QSqlQueryModel *queryModel, QObject *parent = nullptr);
private:
    QSqlQueryModel *query;
    std::vector<QString> meals {"Breakfast","Lunch","Dinner","Snacks"};
};

#endif // MEALSUMMARYMODEL_H
