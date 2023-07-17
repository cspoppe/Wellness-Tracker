
#include "mealstruct.h"

mealStruct::mealStruct(int mealNum, QString date) : mealNum(mealNum)
{
    mealModel = new EditableSqlModel(mealNum,date);
    proxyModel = new MealProxyModel(mealModel);
    mealMacros = new macrosModel(mealModel);
}

// this function refreshes the model for the meal and updates the macros data
void mealStruct::refresh()
{
    mealModel->refresh();
    mealMacros->updateData();
}
