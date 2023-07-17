
#ifndef RECIPEEDITMODEL_H
#define RECIPEEDITMODEL_H

#include <QtSql>
#include "recipetablemodel.h"



class recipeEditModel : public recipeTableModel
{
public:
    explicit recipeEditModel(QObject *parent = nullptr);
    void loadRecipe(int recipe_id, QSqlQueryModel *queryModel);
    int getCurrentRecipeID() const {return currentRecipeID;}
private:
    int currentRecipeID;
};

#endif // RECIPEEDITMODEL_H
