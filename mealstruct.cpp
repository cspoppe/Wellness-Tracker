
#include "mealstruct.h"
#include "utilityfcns.h"
#include <QSqlError>
#include <QSqlQuery>

mealStruct::mealStruct(int mealNum, QString date) : mealNum(mealNum)
{
    mealModel = new sqlMealModel(mealNum,date);
    proxyModel = new MealProxyModel(mealModel);
    mealMacros = new macrosModel(mealModel);
}

// this function refreshes the model for the meal and updates the macros data
void mealStruct::refresh()
{
    mealModel->refresh();
    mealMacros->updateData();
}

void mealStruct::addFood(QItemSelectionModel *select)
{
    if (select != NULL && select->hasSelection()) {
        QModelIndexList selection = select->selectedRows();
        QModelIndex index = selection.at(0);
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

void mealStruct::addFoodToDb(int food_id, double serving_size) {

    QString insertString = "INSERT INTO food_log(date,meal,food_id,serving_size) VALUES('"+mealModel->getDate()+"',";
    insertString += QString::number(mealNum) + ",";
    insertString += QString::number(food_id) + ",";
    insertString += QString::number(serving_size) + ");";
    QSqlQueryModel queryModel;
    queryModel.setQuery(insertString);
    refresh();
    //breakfast->mealModel->refresh();
    emit logUpdated();
    //nutrients->updateData();
    //mealsStruct->meals[meal]->mealMacros->updateData();
}

void mealStruct::addMeal(QItemSelectionModel *select)
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
        qDebug() << "addMeal:  selectionModel has NO selection" << Qt::endl;
    }
}

void mealStruct::addRecipe(QItemSelectionModel *select)
{
    if (select != NULL && select->hasSelection()) {
        QModelIndexList selection = select->selectedRows();
        QModelIndex index = selection.at(0);
        qDebug() << index.row();
        int recipe_id = select->model()->index(index.row(),0).data().toInt();
        // add this food to our database for the day's food log
        addRecipeToDb(recipe_id);
    } else {
        qDebug() << "addRecipe:  selectionModel has NO selection" << Qt::endl;
    }
}

void mealStruct::addRecipeToDb(int recipeID)
{
    QString insertString = "WITH myconst (var1,var2) as (values (date('"+mealModel->getDate()+"'),"+QString::number(mealNum)+")) "
                           "INSERT INTO food_log SELECT var1,var2,food_id,serving_size from myconst,recipe_ingredients "
                           "WHERE recipe_id=" + QString::number(recipeID);

    QSqlQueryModel queryModel;
    queryModel.setQuery(insertString);
    QSqlError err = queryModel.lastError();
    if (err.type() != QSqlError::NoError) qDebug() << err.text() << Qt::endl;
    refresh();
    emit logUpdated();
}

void mealStruct::addMealToDb(QString date, int sourceMealNum) {
    QString insertString = "WITH myconst (var1,var2) as (values (date('"+mealModel->getDate()+"'),"+QString::number(mealNum)+")) "
                           "INSERT INTO food_log SELECT var1,var2,food_id,serving_size from myconst,food_log "
                           "WHERE date='"+date+"' AND meal=" + QString::number(sourceMealNum);
    qDebug() << "addMealToLog:" << Qt::endl << "dest meal: " << QString::number(mealNum) << ", date: " << date << ", source meal: " << QString::number(sourceMealNum) << Qt::endl;
    //queryModel = new QSqlQueryModel();
    QSqlQueryModel queryModel;
    queryModel.setQuery(insertString);
    QSqlError err = queryModel.lastError();
    if (err.type() != QSqlError::NoError) qDebug() << err.text() << Qt::endl;
    refresh();
    emit logUpdated();
    //mealsStruct->meals[meal]->mealMacros->updateData();
}

void mealStruct::deleteFood(QItemSelectionModel *select)
{
    if (select != NULL && select->hasSelection()) {
        qDebug() << "selectionModel has selection" << Qt::endl;
        QModelIndexList selection = select->selectedRows();
        QModelIndex index = selection.at(0);
        qDebug() << index.row();
        QAbstractProxyModel *proxyModel = qobject_cast<QAbstractProxyModel*>(select->model());
        int food_id = proxyModel->sourceModel()->index(index.row(),15).data().toInt();
        // delete this food from our database for the day's food log
        deleteFoodFromDb(select, food_id,index.row());
    } else {
        qDebug() << "selectionModel has NO selection" << Qt::endl;
    }
}

void mealStruct::deleteFoodFromDb(QItemSelectionModel *select, int food_id, int row_index) {
    QSqlQuery query;
    query.prepare("DELETE FROM food_log where date='"+mealModel->getDate()+"' AND meal=:meal AND food_id=:food_id");
    query.bindValue(":meal",mealNum);
    query.bindValue(":food_id",food_id);
    if (query.exec()) {
        //std::vector<QTableView*> tables = {ui->tableBreakfast,ui->tableLunch,ui->tableDinner,ui->tableSnacks};
        refresh();
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
        //meals[meal]->mealModel->refresh();
        emit logUpdated();
        //meals[meal]->mealMacros->updateData();
    }
}
