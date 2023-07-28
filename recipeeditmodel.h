
#ifndef RECIPEEDITMODEL_H
#define RECIPEEDITMODEL_H

#include <QtSql>
#include "recipetablemodel.h"
#include <QItemSelectionModel>


class recipeEditModel : public recipeTableModel
{
    Q_OBJECT
public:
    explicit recipeEditModel(QObject *parent = nullptr);
    void loadRecipe() {loadRecipe(currentRecipeID);}
    void loadRecipe(int recipe_id);
    int getCurrentRecipeID() const {return currentRecipeID;}
    void addFood(QItemSelectionModel *select);
    void addMeal(QItemSelectionModel *select);
    void deleteFood(QItemSelectionModel *select);

signals:
    void recipeUpdated();
private:
    int currentRecipeID;
    void addFoodToDb(int food_id, double serving_size);
    void addMealToDb(QString date, int sourceMealNum);
    void deleteFoodFromDb(QItemSelectionModel *select, int food_id, int row_index);

    // QAbstractItemModel interface
public:
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role) override;
};

#endif // RECIPEEDITMODEL_H
