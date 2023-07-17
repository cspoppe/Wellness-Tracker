
#include "nutrientsmodel.h"
#include <QDebug>

nutrientsModel::nutrientsModel(mealsStructure *meals, exerciseModel *exer, QObject *parent)
    : QStandardItemModel{11,3,parent}, mealsStruct(meals), exercise(exer)
{
    labels = {"Calories (food)","Calories (exercise)","Calories (net)","Total Fat","Saturated Fat","Cholesterol","Sodium","Carbs","Fiber","Sugar","Protein"};
    // initialize first column (labels)
    QFont font;
    font.setBold(true);
    for (int i = 0; i < (int)labels.size(); i++) {
        QStandardItem *labelItem = new QStandardItem(labels[i]);
        QStandardItem *valueItem = new QStandardItem(QString::number(0));
        setItem(i,0,labelItem);
        setItem(i,1,valueItem);
        if ((i == 2) || (i == 3) || (i == 7) || (i == 10)) setData(index(i,0),font,Qt::FontRole);
    }
}

QVariant nutrientsModel::data(const QModelIndex& index, int role) const
{
    if ((role == Qt::BackgroundRole) && (index.column() == 2))
    {
        if ((index.row() == 3 || index.row() == 7 || index.row() == 10))
            return QStandardItemModel::data(index,role);
        else
            return QColor(200,200,200);
    }
    else
    {
        return QStandardItemModel::data(index,role);
    }
}

void nutrientsModel::updateData() {
    // grab data from breakfast, lunch, dinner and snack tables and sum them.
    QVector<double> total_stats = mealsStruct->getNutritionTotals();
    double exercise_calories = exercise->getCalories();
    total_stats.insert(1,exercise_calories);
    total_stats.insert(2,total_stats[0]-exercise_calories);
    std::vector<int> macro_indices = {3,7,10};
    double calories = total_stats[0];
    std::vector<double> macro_calories = {total_stats[3]*9.0,total_stats[7]*4.0,total_stats[10]*4.0};
    int numDig;
    for (int i = 0; i < 11; i++) {
        numDig = (i <=2 || i == 6 ? 4 : 3);
        QStandardItem *valueItem = new QStandardItem(QString::number(total_stats[i],'g',numDig));
        setItem(i,1,valueItem);
    }

    // calculate macros percentage
    for (int i = 0; i < 3; i++) {
        int index = macro_indices[i];
        QString percentString;
        if (calories > 0) {
            double percent = macro_calories[i]/calories*100.0;
            percentString = QString::number(percent,'g',3) + QString("%");
        } else {
            percentString = QString("--");
        }
        QStandardItem *percentItem = new QStandardItem(percentString);
        setItem(index,2,percentItem);
    }

    emit nutrientsUpdated(total_stats);
}
