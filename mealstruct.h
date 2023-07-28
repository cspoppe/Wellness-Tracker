
#ifndef MEALSTRUCT_H
#define MEALSTRUCT_H

#include "sqlmealmodel.h"
#include "mealproxymodel.h"
#include "macrosmodel.h"

#include <QObject>
#include <QItemSelectionModel>


class mealStruct : public QObject
{
    Q_OBJECT

public:
    mealStruct(int mealNum, QString date);
    sqlMealModel *mealModel;
    MealProxyModel *proxyModel;
    macrosModel *mealMacros;
    void refresh();
    void addFood(QItemSelectionModel *select);
    void addMeal(QItemSelectionModel *select);
    void addRecipe(QItemSelectionModel *select);
    void deleteFood(QItemSelectionModel *select);
    void setDate(QString date) {mealModel->setDate(date);}

signals:
    void logUpdated();

private:
    int mealNum;
    void addFoodToDb(int food_id, double serving_size);
    void addMealToDb(QString date, int sourceMealNum);
    void addRecipeToDb(int recipeID);
    void deleteFoodFromDb(QItemSelectionModel *select, int food_id, int row_index);
};

#endif // MEALSTRUCT_H
