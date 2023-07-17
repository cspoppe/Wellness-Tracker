
#ifndef MEALSTRUCT_H
#define MEALSTRUCT_H

#include "editablesqlmodel.h"
#include "mealproxymodel.h"
#include "macrosmodel.h"


class mealStruct
{
public:
    mealStruct(int mealNum, QString date);
    EditableSqlModel *mealModel;
    MealProxyModel *proxyModel;
    macrosModel *mealMacros;
    void refresh();
    void setDate(QString date) {mealModel->setDate(date);};
private:
    int mealNum;
};

#endif // MEALSTRUCT_H
