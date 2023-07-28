
#include "mealproxymodel.h"
#include <qDebug>

MealProxyModel::MealProxyModel(sqlMealModel *meal, QObject *parent)
    : QSortFilterProxyModel{parent}, mealModel(meal)
{
    setSourceModel(mealModel);
}

bool MealProxyModel::filterAcceptsColumn(int source_column, const QModelIndex& source_parent) const {
    Q_UNUSED(source_parent);
    if (source_column == 1 || source_column == 2 || source_column >= 6) {
        return false;
    }
    return true;
}
