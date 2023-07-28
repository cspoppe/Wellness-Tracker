
#ifndef MEALPROXYMODEL_H
#define MEALPROXYMODEL_H

#include <QSortFilterProxyModel>
#include "sqlmealmodel.h"

class MealProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit MealProxyModel(sqlMealModel *meal, QObject *parent = nullptr);
    bool filterAcceptsColumn(int source_column, const QModelIndex& source_parent) const override;
private:
    sqlMealModel *mealModel;
};

#endif // MEALPROXYMODEL_H
