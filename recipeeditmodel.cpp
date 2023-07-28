
#include "recipeeditmodel.h"
#include "utilityfcns.h"

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
}

void recipeEditModel::loadRecipe(int recipe_id)
{
    currentRecipeID = recipe_id;

    QString queryString = "select food_library.id,food_library.name,description,recipe_ingredients.serving_size,units,"
                          "round(recipe_ingredients.serving_size/food_library.serving_size*calories) AS calories,"
                          "round(recipe_ingredients.serving_size/food_library.serving_size*tot_fat) AS tot_fat,"
                          "round(recipe_ingredients.serving_size/food_library.serving_size*carbs) AS carbs,"
                          "round(recipe_ingredients.serving_size/food_library.serving_size*protein) AS protein,"
                          "food_library.serving_size "
                          "FROM recipe_ingredients INNER JOIN food_library ON recipe_ingredients.food_id = food_library.id "
                          "WHERE recipe_id = " + QString::number(currentRecipeID);

    QSqlQueryModel queryModel;
    queryModel.setQuery(queryString);

    QSqlError err = queryModel.lastError();
    if (err.type() != QSqlError::NoError) qDebug() << err.text() << Qt::endl;


    // remove any existing data
    int nRows = rowCount();
    if (nRows > 0)
        removeRows(0,nRows);


    int nColumns = queryModel.columnCount();
    nRows = queryModel.rowCount();
    for (int row{0}; row < nRows; ++row)
    {
        QList<QStandardItem*> newRow;
        for (int col{0}; col < nColumns; ++col)
        {
            QStandardItem *item = new QStandardItem(queryModel.index(row,col).data().toString());
            newRow.push_back(item);
        }
        // Add one more item that stores what the serving size was most recently set at.
        // This is used to calculate the old "serving multiplier" (i.e., serving_size/base_serving_size)
        // This is so we can revert the macro values back to their base value, so they can be correctly updated for
        // the new serving size.
        QStandardItem *old_serving_size = new QStandardItem(queryModel.index(row,3).data().toString());
        newRow.push_back(old_serving_size);
        appendRow(newRow);
    }
}

void recipeEditModel::addFood(QItemSelectionModel *select)
{
    if (select != NULL && select->hasSelection()) {
        QModelIndexList selection = select->selectedRows();
        QModelIndex index = selection.at(0); // gets first row (should only be one)
        qDebug() << index.row();
        QAbstractProxyModel *proxyModel = qobject_cast<QAbstractProxyModel*>(select->model());
        int food_id = proxyModel->sourceModel()->index(index.row(),0).data().toInt();
        double serving_size = proxyModel->sourceModel()->index(index.row(),3).data().toDouble();
        // add this food to our database for the day's food log
        addFoodToDb(food_id,serving_size);
    } else {
        qDebug() << "selectionModel has NO selection" << Qt::endl;
    }
}

void recipeEditModel::addFoodToDb(int food_id, double serving_size) {

    QString insertString = "INSERT INTO recipe_ingredients(recipe_id,food_id,serving_size) VALUES(";
    insertString += QString::number(currentRecipeID) + ",";
    insertString += QString::number(food_id) + ",";
    insertString += QString::number(serving_size) + ");";
    QSqlQueryModel queryModel;
    queryModel.setQuery(insertString);
    loadRecipe();
    emit recipeUpdated();
}

void recipeEditModel::addMeal(QItemSelectionModel *select)
{
    if (select != NULL && select->hasSelection()) {
        qDebug() << "addMealToMeal: selectionModel has selection" << Qt::endl;
        QModelIndexList selection = select->selectedRows();
        QModelIndex index = selection.at(0);
        qDebug() << index.row();
        QString date = select->model()->index(index.row(),0).data().toString();
        QString sourceMeal = select->model()->index(index.row(),1).data().toString();
        int sourceMealNum = mealLabelMap(sourceMeal);
        // add this food to our database for the day's food log
        addMealToDb(date,sourceMealNum);
    } else {
        qDebug() << "addMealToMeal:  selectionModel has NO selection" << Qt::endl;
    }
}

void recipeEditModel::addMealToDb(QString date, int sourceMealNum) {
    QString insertString = "WITH myconst (var1) as (values ("+QString::number(currentRecipeID)+")) "
                           "INSERT INTO recipe_ingredients SELECT var1,food_id,serving_size from myconst,food_log "
                           "WHERE date='"+date+"' AND meal=" + QString::number(sourceMealNum);

    QSqlQueryModel queryModel;
    queryModel.setQuery(insertString);
    QSqlError err = queryModel.lastError();
    if (err.type() != QSqlError::NoError) qDebug() << err.text() << Qt::endl;
    loadRecipe();
    emit recipeUpdated();
    //mealsStruct->meals[meal]->mealMacros->updateData();
}

void recipeEditModel::deleteFood(QItemSelectionModel *select)
{
    if (select != NULL && select->hasSelection()) {
        QModelIndexList selection = select->selectedRows();
        QModelIndex index = selection.at(0);
        qDebug() << index.row();
        //QAbstractProxyModel *proxyModel = qobject_cast<QAbstractProxyModel*>(select->model());
        int food_id = select->model()->index(index.row(),0).data().toInt();
        // delete this food from our database for the day's food log
        deleteFoodFromDb(select,food_id,index.row());
    } else {
        qDebug() << "selectionModel has NO selection" << Qt::endl;
    }
}

void recipeEditModel::deleteFoodFromDb(QItemSelectionModel *select, int food_id, int row_index) {
    QSqlQuery query;
    query.prepare("DELETE FROM recipe_ingredients where recipe_id=:recipe_id AND food_id=:food_id");
    query.bindValue(":recipe_id",currentRecipeID);
    query.bindValue(":food_id",food_id);
    if (query.exec()) {
        //std::vector<QTableView*> tables = {ui->tableBreakfast,ui->tableLunch,ui->tableDinner,ui->tableSnacks};
        loadRecipe();
        int lastRowIndex = select->model()->rowCount()-1;
        if (lastRowIndex >= 0)
        {
            QModelIndex idx;
            if (row_index < lastRowIndex)
                idx = select->model()->index(row_index,0);
            else
                idx = select->model()->index(lastRowIndex,0);
            select->select(idx,QItemSelectionModel::Select | QItemSelectionModel::Rows);
        }
        emit recipeUpdated();
    }
}



bool recipeEditModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    // in addition to modifying the calories and macros listed in the table when we edit the serving size,
    // as this function does in the recipeTableModel class, we also need to update the database for this recipe
    // to reflect the new value of the serving size.
    bool ok = recipeTableModel::setData(index,value,role);
    QSqlQueryModel queryModel;

    if (index.column() == 3) // not sure if needed, but this is to make sure we only carry out this step when the serving size field has been edited.
    {
        double serving_size = value.toDouble();
        int food_id = QStandardItemModel::data(index.siblingAtColumn(0),role).toInt();
        QString updateString = "UPDATE recipe_ingredients SET serving_size = "+QString::number(serving_size)+
                               " where recipe_id = " + QString::number(currentRecipeID) + " and food_id = " + QString::number(food_id);
        queryModel.setQuery(updateString);
    }

    return ok;
}
