
#ifndef MEALSSTRUCTURE_H
#define MEALSSTRUCTURE_H


#include <QObject>
#include "mealstruct.h"

class mealsStructure
{
public:
    mealsStructure(QString date);
    std::vector<mealStruct*> meals;
    void updateDate(QString date);
    QVector<double> getNutritionTotals();
    void refresh();
private:
    QString currentDate;

};

#endif // MEALSSTRUCTURE_H
