
#ifndef MACROSMODEL_H
#define MACROSMODEL_H

#include <QStandardItemModel>
#include "sqlmealmodel.h"

class macrosModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit macrosModel(sqlMealModel *meal, QObject *parent = nullptr);
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
public slots:
    void updateData();
private:
    sqlMealModel *mealModel;
    std::vector<QString> labels;
};

#endif // MACROSMODEL_H
