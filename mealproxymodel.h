
#ifndef MEALPROXYMODEL_H
#define MEALPROXYMODEL_H

#include <QSortFilterProxyModel>
#include "editablesqlmodel.h"

class MealProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit MealProxyModel(EditableSqlModel *meal, QObject *parent = nullptr);
    bool filterAcceptsColumn(int source_column, const QModelIndex& source_parent) const override;
private:
    EditableSqlModel *mealModel;
};

#endif // MEALPROXYMODEL_H
