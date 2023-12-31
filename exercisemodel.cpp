#include <QtSql>
#include "exercisemodel.h"

exerciseModel::exerciseModel(QString date, QObject *parent)
    : QSqlQueryModel{parent}, currentDate(date)
{

}

Qt::ItemFlags exerciseModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QSqlQueryModel::flags(index);
    if ((index.column() == 2) || (index.column() == 3))
        flags |= Qt::ItemIsEditable;
    return flags;
}

bool exerciseModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    // we can only edit columns 2 or 3
    if ((index.column() != 2) && (index.column() != 3))
        return false;

    QModelIndex i = index.siblingAtColumn(0);
    int exercise_id = data(i,role).toInt();

    bool ok;
    if (index.column()==2)
        ok = setDuration(value.toDouble(),exercise_id);
    else
        ok = setCalories(value.toDouble(),exercise_id);
    refresh();
    emit dataChanged(index,index);
    return ok;
}

bool exerciseModel::setDuration(double duration,int exercise_id)
{
    QSqlQuery query;
    query.prepare("UPDATE exercise_log set duration_min = :duration where date = '" + currentDate + "' and exercise_id = :exercise_id");
    query.bindValue(":duration", duration);
    query.bindValue(":exercise_id", exercise_id);
    return query.exec();
}

bool exerciseModel::setCalories(double calories,int exercise_id)
{
    QSqlQuery query;
    query.prepare("UPDATE exercise_log set calories = :calories where date = '" + currentDate + "' and exercise_id = :exercise_id");
    query.bindValue(":calories", calories);
    query.bindValue(":exercise_id", exercise_id);
    return query.exec();
}

bool exerciseModel::updateCaloriesTotal()
{
    double tot_calories{0};
    for (int row = 0; row < QSqlQueryModel::rowCount(); row++)
    {
        int column = record(row).indexOf("calories");
        QModelIndex i_calories = QSqlQueryModel::index(row,column);
        double calories = QSqlQueryModel::data(i_calories).toDouble();
        tot_calories += calories;
    }

    total_calories = tot_calories;
    return true;
}

void exerciseModel::refresh() {
    QString query = "SELECT exercise_log.exercise_id,exercise_library.exercise_name,duration_min,calories FROM "
                    "exercise_log INNER JOIN exercise_library ON exercise_log.exercise_id = exercise_library.exercise_id WHERE "
                    "exercise_log.date = '" + currentDate + "'";
    setQuery(query);
    QSqlError err = lastError();
    if (err.type() != QSqlError::NoError) qDebug() << err.text() << Qt::endl;
    setHeaderData(0,Qt::Horizontal, QObject::tr("Id"));
    setHeaderData(1,Qt::Horizontal, QObject::tr("Exercise"));
    setHeaderData(2,Qt::Horizontal, QObject::tr("Duration (min)"));
    setHeaderData(3,Qt::Horizontal, QObject::tr("Calories"));
    updateCaloriesTotal();
}
