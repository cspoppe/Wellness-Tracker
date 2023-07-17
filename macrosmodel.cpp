
#include "macrosmodel.h"
#include <numeric>

macrosModel::macrosModel(EditableSqlModel *meal, QObject *parent)
    : QStandardItemModel{4,3,parent}, mealModel(meal)
{
    labels = {"Calories","Fat (g)","Carbs (g)","Protein (g)"};
    QFont font;
    font.setBold(true);
    for (int i = 0; i < 4; i++) {
        QStandardItem *labelItem = new QStandardItem(labels[i]);
        QStandardItem *valueItem = new QStandardItem(QString::number(0));
        //QStandardItem *percentItem = new QStandardItem(QString("0%"));
        setItem(i,0,labelItem);
        setItem(i,1,valueItem);
        //setItem(i,2,percentItem);
        setData(index(i,0),font,Qt::FontRole);
    }
}

QVariant macrosModel::data(const QModelIndex& index, int role) const
{
    if ((index.row() == 0) && (index.column() == 2) && (role == Qt::BackgroundRole))
        return QColor(200,200,200);
    else
        return QStandardItemModel::data(index,role);
}

void macrosModel::updateData() {
    std::vector<double> nutrients = mealModel->getNutritionStats();
    // total fat: i = 1
    // carbs: i = 5
    // protein: i = 8
    std::vector<int> indices = {1,5,8};
    std::vector<double> macro_calories = {nutrients[1]*9.0,nutrients[5]*4.0,nutrients[8]*4.0};
    // get total calories
    double tot_calories = std::accumulate(macro_calories.begin(),macro_calories.end(),0);
    // first, set total calories
    QStandardItem *caloriesItem = new QStandardItem(QString::number(tot_calories,'g',3));
    setItem(0,1,caloriesItem);
    for (int i = 0; i < 3; i++) {
        int index = indices[i];
        QString percentString;
        if (tot_calories > 0) {
            double percent = macro_calories[i]/tot_calories*100.0;
            percentString = QString::number(percent,'g',3) + QString("%");
        } else {
            percentString = QString("--");
        }
        QStandardItem *valueItem = new QStandardItem(QString::number(nutrients[index],'g',3));
        QStandardItem *percentItem = new QStandardItem(percentString);
        setItem(i+1,1,valueItem);
        setItem(i+1,2,percentItem);
    }
}
