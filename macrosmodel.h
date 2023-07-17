
#ifndef MACROSMODEL_H
#define MACROSMODEL_H

#include <QStandardItemModel>
#include "editablesqlmodel.h"

class macrosModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit macrosModel(EditableSqlModel *meal, QObject *parent = nullptr);
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
public slots:
    void updateData();
private:
    EditableSqlModel *mealModel;
    std::vector<QString> labels;
};

#endif // MACROSMODEL_H
