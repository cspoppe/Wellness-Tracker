
#include "mealsummarymodel.h"
#include <qsqlrecord.h>

mealSummaryModel::mealSummaryModel(QSqlQueryModel *queryModel, QObject *parent)
    : QStandardItemModel{parent}, query(queryModel)
{
    // one by one, grab rows from queryModel and insert into QStandardItemModel
    int nRows = query->rowCount();
    setColumnCount(3);
    QString lastDate, lastMeal;
    QString foods;
    bool firstEntry = true;
    int rowCount = 0;
    for (int i = 0; i < nRows; ++i) {
        QString date = query->record(i).value("food_log.date").toString();
        int mealNum = query->record(i).value("food_log.meal").toInt();
        QString meal = meals[mealNum];
        QString food = query->record(i).value("food_library.name").toString();
        if (firstEntry)
        {
            firstEntry =false;
            foods.push_back(food);
            lastDate = date;
            lastMeal = meal;
        }
        else if (date == lastDate && meal == lastMeal)
        {
            // append ingredient to vector of foods
            foods.push_back(QString(", ") + food);
        }
        else
        {
            // add previous row to the table model
            QStandardItem *dateItem = new QStandardItem(lastDate);
            setItem(rowCount,0,dateItem);
            QStandardItem *mealItem = new QStandardItem(lastMeal);
            setItem(rowCount,1,mealItem);
            QStandardItem *foodsItem = new QStandardItem(foods);
            setItem(rowCount,2,foodsItem);
            rowCount++;
            lastDate = date;
            lastMeal = meal;
            foods = food;
        }
        // add last remaining row to the model
        QStandardItem *dateItem = new QStandardItem(lastDate);
        setItem(rowCount,0,dateItem);
        QStandardItem *mealItem = new QStandardItem(lastMeal);
        setItem(rowCount,1,mealItem);
        QStandardItem *foodsItem = new QStandardItem(foods);
        setItem(rowCount,2,foodsItem);
    }
    setHeaderData(0,Qt::Horizontal, tr("Date"));
    setHeaderData(1,Qt::Horizontal, tr("Meal"));
    setHeaderData(2,Qt::Horizontal, tr("Foods"));
}

