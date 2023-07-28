#include <QtSql>
#include "sqlmealmodel.h"

sqlMealModel::sqlMealModel(int meal, QString date, QObject *parent)
    : QSqlQueryModel{parent}, mealNo(meal), currentDate(date), nutrition_stats(9,0.0)
{
}

Qt::ItemFlags sqlMealModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QSqlQueryModel::flags(index);
    if (index.column() == 3)
        flags |= Qt::ItemIsEditable;
    return flags;
}

int sqlMealModel::columnCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return 16;
}

bool sqlMealModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    // we can only edit column 3 (serving size)
    if (index.column() != 3)
        return false;

    QModelIndex i15 = index.siblingAtColumn(15);
    int food_id = data(i15,role).toInt();

    bool ok = setServingSize(value.toDouble(),mealNo,food_id);
    refresh();
    emit dataChanged(index,index);
    return ok;
}

QVariant sqlMealModel::data(const QModelIndex& index, int role) const {
    if ((index.column() == 5) && (role == Qt::DisplayRole || role == Qt::EditRole)) {
        QModelIndex i_servings_base = index.siblingAtColumn(2);
        QModelIndex i_servings_tot = index.siblingAtColumn(3);
        QModelIndex i_calories_base = index.siblingAtColumn(6);
        double servings_base = QSqlQueryModel::data(i_servings_base,role).toDouble();
        double servings_tot = QSqlQueryModel::data(i_servings_tot,role).toDouble();
        double calories_base = QSqlQueryModel::data(i_calories_base,role).toDouble();
        double calories_tot = servings_tot/servings_base*calories_base;
        QString str = QString::number(calories_tot,'g',3);
        return QVariant(str);
    }
    else {
        return QSqlQueryModel::data(index,role);
    }
}

bool sqlMealModel::updateNutritionTotals() {
    // sum the values of each column containing nutrition stats and save them to nutrition_stats vector.
    std::vector<double> stats(9,0.0);
    for (int row = 0; row < QSqlQueryModel::rowCount(); row++) {
        QModelIndex i_servings_base = QSqlQueryModel::index(row,2);
        QModelIndex i_servings_tot = QSqlQueryModel::index(row,3);
        double servings_base = QSqlQueryModel::data(i_servings_base).toDouble();
        double servings_tot = QSqlQueryModel::data(i_servings_tot).toDouble();
        for (int col = 6; col <= 14; col++) {
            QModelIndex i = QSqlQueryModel::index(row,col);
            double nutrition_base = QSqlQueryModel::data(i).toDouble();
            double nutrition_tot = servings_tot/servings_base*nutrition_base;
            stats[col-6] += nutrition_tot;
        }
    }

    nutrition_stats = stats;

    return true;
}

bool sqlMealModel::setServingSize(double servingSize, int meal, int food_id) {
    QSqlQuery query;
    query.prepare("UPDATE food_log set serving_size = :serving_size where date = '" + currentDate + "' and meal = :mealNo and food_id = :food_id");
    query.bindValue(":serving_size", servingSize);
    query.bindValue(":mealNo", meal);
    query.bindValue(":food_id", food_id);
    return query.exec();
}

void sqlMealModel::refresh() {
    QString query = "SELECT name,description,food_library.serving_size,food_log.serving_size,units,calories,calories,"
                    "tot_fat,sat_fat,cholesterol,sodium,carbs,fiber,sugar,protein,food_log.food_id FROM"
                    " food_log INNER JOIN food_library ON food_log.food_id = food_library.id WHERE"
                    " meal = " + QString::number(mealNo) + " and food_log.date = '" + currentDate + "'";
    setQuery(query);
    QSqlError err = lastError();
    if (err.type() != QSqlError::NoError) qDebug() << err.text() << Qt::endl;
    //insertColumns(5,1);
    setHeaderData(0,Qt::Horizontal, QObject::tr("Food"));
    setHeaderData(1,Qt::Horizontal, QObject::tr("Desc."));
    setHeaderData(2,Qt::Horizontal, QObject::tr("Serving Size - Base"));
    setHeaderData(3,Qt::Horizontal, QObject::tr("Serving"));
    setHeaderData(4,Qt::Horizontal, QObject::tr("Units"));
    setHeaderData(5,Qt::Horizontal, QObject::tr("Calories"));
    setHeaderData(6,Qt::Horizontal, QObject::tr("Calories - Base"));
    setHeaderData(7,Qt::Horizontal, QObject::tr("Tot Fat - Base"));
    setHeaderData(8,Qt::Horizontal, QObject::tr("Sat Fat - Base"));
    setHeaderData(9,Qt::Horizontal, QObject::tr("Cholesterol - Base"));
    setHeaderData(10,Qt::Horizontal, QObject::tr("Sodium - Base"));
    setHeaderData(11,Qt::Horizontal, QObject::tr("Carbs - Base"));
    setHeaderData(12,Qt::Horizontal, QObject::tr("Fiber - Base"));
    setHeaderData(13,Qt::Horizontal, QObject::tr("Sugar - Base"));
    setHeaderData(14,Qt::Horizontal, QObject::tr("Protein - Base"));
    setHeaderData(15,Qt::Horizontal, QObject::tr("Food ID"));
    updateNutritionTotals();
    //int idIdx = weightModel->fieldIndex("id");
}
