
#include "recipeeditmodel.h"

recipeEditModel::recipeEditModel(QObject *parent)
    : recipeTableModel{parent}
{
    setColumnCount(11);
    setHeaderData(0,Qt::Horizontal,tr("ID"));
    setHeaderData(1,Qt::Horizontal,tr("Name"));
    setHeaderData(2,Qt::Horizontal,tr("Desc."));
    setHeaderData(3,Qt::Horizontal,tr("Serving"));
    setHeaderData(4,Qt::Horizontal,tr("Units"));
    setHeaderData(5,Qt::Horizontal,tr("Calories"));
    setHeaderData(6,Qt::Horizontal,tr("Fat (g)"));
    setHeaderData(7,Qt::Horizontal,tr("Carbs (g)"));
    setHeaderData(8,Qt::Horizontal,tr("Protein (g)"));
    setHeaderData(9,Qt::Horizontal,tr("Serving size base"));
    setHeaderData(10,Qt::Horizontal,tr("Old serving size"));
    QFont font;
    font.setBold(true);



}

void recipeEditModel::loadRecipe(int recipe_id, QSqlQueryModel *queryModel)
{
    currentRecipeID = recipe_id;

    // remove any existing data
    int nRows = rowCount();
    if (nRows > 0)
        removeRows(0,nRows);


    int nColumns = queryModel->columnCount();
    nRows = queryModel->rowCount();
    for (int row{0}; row < nRows; ++row)
    {
        QList<QStandardItem*> newRow;
        for (int col{0}; col < nColumns; ++col)
        {
            QStandardItem *item = new QStandardItem(queryModel->index(row,col).data().toString());
            newRow.push_back(item);
        }
        // Add one more item that stores what the serving size was most recently set at.
        // This is used to calculate the old "serving multiplier" (i.e., serving_size/base_serving_size)
        // This is so we can revert the macro values back to their base value, so they can be correctly updated for
        // the new serving size.
        QStandardItem *old_serving_size = new QStandardItem(queryModel->index(row,3).data().toString());
        newRow.push_back(old_serving_size);
        appendRow(newRow);
    }
}

