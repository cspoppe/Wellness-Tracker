
#include "recipetablemodel.h"
#include "utilityFcns.h"

recipeTableModel::recipeTableModel(QObject *parent)
    : QStandardItemModel{parent}, macroTotals(4,0.0)
{
    connect(this,&QAbstractItemModel::rowsInserted,this,&recipeTableModel::sumMacroTotals);
    connect(this,&QAbstractItemModel::rowsRemoved,this,&recipeTableModel::sumMacroTotals);
    connect(this,&QAbstractItemModel::dataChanged,this,&recipeTableModel::sumMacroTotals);
}

/*
QStandardItemModel model;
QStandardItem *parentItem = model.invisibleRootItem();
for (int i = 0; i < 4; ++i) {
    QStandardItem *item = new QStandardItem(QString("item %0").arg(i));
    parentItem->appendRow(item);
    parentItem = item;
}
*/

Qt::ItemFlags recipeTableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QStandardItemModel::flags(index);

    // only column 3 should be editable
    if (index.column() != 3)
        flags &= ~Qt::ItemIsEditable;
    return flags;
}

void recipeTableModel::sumMacroTotals()
{
    qDebug() << "sumMacroTotals called" << Qt::endl;
    QVector<double> stats(4,0.0);
    for (int row{0}; row < QStandardItemModel::rowCount(); ++row)
    {
        for (int col{5}; col < 9; ++col)
        {
            stats[col-5] += data(index(row,col)).toDouble();
        }
    }

    macroTotals = stats;
    emit macrosUpdated();
}

bool recipeTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    // for any column other than column 3, just set data as normal
    if (index.column() != 3)
        return QStandardItemModel::setData(index, value, role);

    // if column 3 is changed, we need to update values in columns 5-8 to reflect calories and macro totals for new serving size
    double new_serving_size = value.toDouble();
    double serving_size_base = QStandardItemModel::data(index.siblingAtColumn(9),role).toDouble();
    double old_serving_size = QStandardItemModel::data(index.siblingAtColumn(10),role).toDouble();

    double old_serving_multiplier = old_serving_size/serving_size_base;
    double new_serving_multiplier = new_serving_size/serving_size_base;
    bool ok = QStandardItemModel::setData(index, value, role);
    for (int i{5}; i < 9; ++i)
    {
        QModelIndex val_index = index.siblingAtColumn(i);
        double val = QStandardItemModel::data(val_index,role).toDouble();
        val /= old_serving_multiplier;
        val *= new_serving_multiplier;
        val = roundDec(val,1);
        ok = (ok && QStandardItemModel::setData(val_index, QVariant(val), role));
    }
    // save the new serving size in column 10
    ok = (ok && QStandardItemModel::setData(index.siblingAtColumn(10), QVariant(new_serving_size), role));
    return ok;
}

void recipeTableModel::getFoodIDs(QVector<int> &ids, QVector<double> &serving_size)
{
    for (int row{0}; row < QStandardItemModel::rowCount(); ++row)
    {
        ids.push_back(data(index(row,0)).toInt());
        serving_size.push_back(data(index(row,3)).toDouble());
    }
}
