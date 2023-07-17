
#ifndef FOODSUMMARYMODEL_H
#define FOODSUMMARYMODEL_H

#include <QStandardItemModel>
#include <QSqlQueryModel>



class foodSummaryModel : public QStandardItemModel
{
public:
    explicit foodSummaryModel(QSqlQueryModel *queryModel, QObject *parent = nullptr);
private:
    QSqlQueryModel *query;
};

#endif // FOODSUMMARYMODEL_H
