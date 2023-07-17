
#ifndef NUTRIENTSMODEL_H
#define NUTRIENTSMODEL_H

#include <QStandardItemModel>
#include "mealsstructure.h"
#include "exercisemodel.h"

class nutrientsModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit nutrientsModel(mealsStructure *meals, exerciseModel *exer, QObject *parent = nullptr);
    QVariant data(const QModelIndex& index, int role) const override;

signals:
    nutrientsUpdated(QVector<double> stats);
public slots:
    void updateData();
private:
    mealsStructure *mealsStruct;
    exerciseModel *exercise;
    std::vector<QString> labels;
};

#endif // NUTRIENTSMODEL_H
