
#ifndef FOODSEARCHPROXYMODEL_H
#define FOODSEARCHPROXYMODEL_H

#include "qsqlquerymodel.h"
#include <QSortFilterProxyModel>
//#include <QVector>



class foodSearchProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    //explicit foodSearchProxyModel(QSqlQueryModel *query, QObject *parent = nullptr);
    explicit foodSearchProxyModel(QAbstractItemModel *source, QObject *parent = nullptr);
    bool filterAcceptsColumn(int source_column, const QModelIndex& source_parent) const override;
    void addFilteredColumn(int col);
private:
    QAbstractItemModel *srcModel;
    QVector<int> filteredColumns;
};

#endif // FOODSEARCHPROXYMODEL_H
