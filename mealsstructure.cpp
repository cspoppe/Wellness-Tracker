
#include "mealsstructure.h"

mealsStructure::mealsStructure(QString date) : meals(4), currentDate(date)
{
    for (int i = 0; i < 4; i++) {
        meals[i] = new mealStruct(i, date);
    }
}

QVector<double> mealsStructure::getNutritionTotals()
{
    QVector<double> totals(9,0.0);
    for (int i = 0; i < 4; i++) {
        std::vector<double> mealStats = meals[i]->mealModel->getNutritionStats();
        for (int j = 0; j < 9; j++) {
            totals[j] += mealStats[j];
        }
    }

    return totals;
}

void mealsStructure::updateDate(QString date) {
    for (int i = 0; i < 4; i++) {
        meals[i]->setDate(date);
    }
}

void mealsStructure::refresh()
{
    for (int i = 0; i < 4; i++)
    {
        meals[i]->refresh();
    }
}
