
#include "foodsearchproxymodel.h"

foodSearchProxyModel::foodSearchProxyModel(QAbstractItemModel *source, QObject *parent)
    : QSortFilterProxyModel{parent}, srcModel(source)
{
    setSourceModel(srcModel);
    filteredColumns = {0};
}

void foodSearchProxyModel::addFilteredColumn(int col)
{
    // check first that this column has not already been added
    if (filteredColumns.indexOf(col) < 0) filteredColumns.push_back(col);
}

bool foodSearchProxyModel::filterAcceptsColumn(int source_column, const QModelIndex& source_parent) const {
    Q_UNUSED(source_parent);
    if (filteredColumns.indexOf(source_column) < 0) return true;
    return false;
}
