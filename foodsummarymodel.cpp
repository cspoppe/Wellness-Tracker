
#include "foodsummarymodel.h"
#include <qsqlrecord.h>

foodSummaryModel::foodSummaryModel(QSqlQueryModel *queryModel, QObject *parent)
    : QStandardItemModel{parent}, query(queryModel)
{

}

