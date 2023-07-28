
#include "statsplotmodel.h"
#include "utilityfcns.h"
//#include "qsqlerror.h"
//#include "qsqlquerymodel.h"
#include <QtSql>

statsPlotModel::statsPlotModel(QCustomPlot *customPlot, QObject *parent) : QObject{parent}, plot(customPlot), plotCompleteDays(false)
{

    tickLabelFont.setPointSize(12);
    labelFont.setPointSize(14);
    labelFont.setStyleStrategy(QFont::PreferAntialias);

    nutrientStats[plotFoodCalories] = &food_calories;
    nutrientStats[plotExerciseCalories] = &exercise_calories;
    nutrientStats[plotNetCalories] = &net_calories;
    nutrientStats[plotTotFat] = &tot_fat;
    nutrientStats[plotSatFat] = &sat_fat;
    nutrientStats[plotCholesterol] = &cholesterol;
    nutrientStats[plotSodium] = &sodium;
    nutrientStats[plotCarbs] = &carbs;
    nutrientStats[plotFiber] = &fiber;
    nutrientStats[plotSugar] = &sugar;
    nutrientStats[plotProtein] = &protein;

    completeNutrientStats[plotFoodCalories] = &completed_food_calories;
    completeNutrientStats[plotExerciseCalories] = &completed_exercise_calories;
    completeNutrientStats[plotNetCalories] = &completed_net_calories;
    completeNutrientStats[plotTotFat] = &completed_tot_fat;
    completeNutrientStats[plotSatFat] = &completed_sat_fat;
    completeNutrientStats[plotCholesterol] = &completed_cholesterol;
    completeNutrientStats[plotSodium] = &completed_sodium;
    completeNutrientStats[plotCarbs] = &completed_carbs;
    completeNutrientStats[plotFiber] = &completed_fiber;
    completeNutrientStats[plotSugar] = &completed_sugar;
    completeNutrientStats[plotProtein] = &completed_protein;

    plotColors[plotFoodCalories] = Qt::black;
    plotColors[plotExerciseCalories] = Qt::cyan;
    plotColors[plotNetCalories] = Qt::blue;
    plotColors[plotTotFat] = Qt::red;
    plotColors[plotSatFat] = Qt::darkRed;
    plotColors[plotCholesterol] = Qt::darkMagenta;
    plotColors[plotSodium] = Qt::darkYellow;
    plotColors[plotCarbs] = Qt::magenta;
    plotColors[plotFiber] = Qt::darkGreen;
    plotColors[plotSugar] = Qt::gray;
    plotColors[plotProtein] = Qt::green;

    legendNames[plotFoodCalories] = QString("Food Calories");
    legendNames[plotExerciseCalories] = QString("Exercise Calories");
    legendNames[plotNetCalories] = QString("Net Calories");
    legendNames[plotTotFat] = QString("Total Fat");
    legendNames[plotSatFat] = QString("Sat. Fat");
    legendNames[plotCholesterol] = QString("Cholesterol");
    legendNames[plotSodium] = QString("Sodium");
    legendNames[plotCarbs] = QString("Carbs");
    legendNames[plotFiber] = QString("Fiber");
    legendNames[plotSugar] = QString("Sugar");
    legendNames[plotProtein] = QString("Protein");

    leftYAxisSignals.push_back(plotFoodCalories);
    leftYAxisSignals.push_back(plotExerciseCalories);
    leftYAxisSignals.push_back(plotNetCalories);
    leftYAxisSignals.push_back(plotSodium);

    checkBoxGroup = new QButtonGroup();
    checkBoxGroup->setExclusive(false);
    plot->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    dateTicker->setDateTimeFormat("MMM dd yyyy");
    plot->xAxis->setTicker(dateTicker);
    //plot->xAxis->setLabel("Date");
    plot->yAxis->setLabel("Calories/mg");
    plot->yAxis2->setVisible(true);
    plot->yAxis2->setTicks(true);
    plot->yAxis2->setTickLabels(true);
    plot->yAxis2->setLabel("Grams");
    plot->yAxis->setRange(0,3000);
    plot->yAxis2->setRange(0,150);

    plot->xAxis->setTickLabelFont(tickLabelFont);
    plot->yAxis->setLabelFont(labelFont);
    plot->yAxis->setTickLabelFont(tickLabelFont);
    plot->yAxis2->setLabelFont(labelFont);
    plot->yAxis2->setTickLabelFont(tickLabelFont);
    plot->legend->setFont(tickLabelFont);

}

void statsPlotModel::toggleLegend(int show)
{
    plot->legend->setVisible(show);
    plot->replot();
}

void statsPlotModel::getDataFromDB()
{
    QSqlQueryModel model;
    QString queryString = "DROP TABLE IF EXISTS exercise_table; DROP TABLE IF EXISTS food_table; "
                           "SELECT date,SUM(calories) AS exercise_calories INTO TEMP TABLE exercise_table "
                           "FROM exercise_log GROUP BY date; ";
    model.setQuery(queryString);
    queryString = "SELECT food_log.date,"
                          "ROUND(SUM(food_log.serving_size/food_library.serving_size*food_library.calories),0) AS food_calories, "
                          "ROUND(SUM(food_log.serving_size/food_library.serving_size*food_library.tot_fat),0) AS tot_fat, "
                          "ROUND(SUM(food_log.serving_size/food_library.serving_size*food_library.sat_fat),0) AS sat_fat, "
                          "ROUND(SUM(food_log.serving_size/food_library.serving_size*food_library.cholesterol),0) AS cholesterol, "
                          "ROUND(SUM(food_log.serving_size/food_library.serving_size*food_library.sodium),0) AS sodium, "
                          "ROUND(SUM(food_log.serving_size/food_library.serving_size*food_library.carbs),0) AS carbs, "
                          "ROUND(SUM(food_log.serving_size/food_library.serving_size*food_library.fiber),0) AS fiber, "
                          "ROUND(SUM(food_log.serving_size/food_library.serving_size*food_library.sugar),0) AS sugar, "
                          "ROUND(SUM(food_log.serving_size/food_library.serving_size*food_library.protein),0) AS protein "
                          "INTO TEMP TABLE food_table "
                          "FROM food_log "
                          "INNER JOIN food_library ON food_log.food_id = food_library.id "
                          "GROUP BY food_log.date ORDER BY food_log.date ASC; ";
    model.setQuery(queryString);
    queryString = "SELECT food_table.date,food_calories,COALESCE(exercise_calories,0) AS exercise_calories,tot_fat,sat_fat,cholesterol,sodium,carbs,fiber,sugar,protein from food_table "
                   "FULL OUTER JOIN exercise_table ON food_table.date = exercise_table.date "
                   "ORDER BY food_table.date ASC;";



    /*
    QString queryAppend = "GROUP BY date ORDER BY date ASC";

    if (!startDate.isNull())
    {
        // start date provided.
        QString startDateString = dateToString(startDate);
        QString queryDates = "WHERE date >='"+startDateString+"' ";
        if (!endDate.isNull())
        {
            QString endDateString = dateToString(endDate);
            queryDates += "AND date <='"+endDateString+"' ";
        }
        queryString += queryDates;
    }

    queryString += queryAppend;
    */

    model.setQuery(queryString);
    QSqlError err = model.lastError();
    if (err.type() != QSqlError::NoError) qDebug() << "Query error: " << err.text() << Qt::endl;

    int nRows = model.rowCount();

    // reset vectors back to zero size
    setVectorsToZero();
    for (int i{0}; i < nRows; i++)
    {
        double food_cal = model.record(i).value("food_calories").toDouble();
        double exer_cal = model.record(i).value("exercise_calories").toDouble();
        food_calories.push_back(food_cal);
        exercise_calories.push_back(exer_cal);
        net_calories.push_back(food_cal-exer_cal);
        tot_fat.push_back(model.record(i).value("tot_fat").toDouble());
        sat_fat.push_back(model.record(i).value("sat_fat").toDouble());
        cholesterol.push_back(model.record(i).value("cholesterol").toDouble());
        sodium.push_back(model.record(i).value("sodium").toDouble());
        carbs.push_back(model.record(i).value("carbs").toDouble());
        fiber.push_back(model.record(i).value("fiber").toDouble());
        sugar.push_back(model.record(i).value("sugar").toDouble());
        protein.push_back(model.record(i).value("protein").toDouble());
        QString dateString = model.record(i).value("date").toString();
        date_strings.push_back(dateString);
        dates.push_back(QDateTime::fromString(dateString,"yyyy-MM-dd").toSecsSinceEpoch());
    }

    /* Get flags for which days are complete days, then structure new versions of vectors that only contain data from complete days. */

    queryString = "SELECT * FROM day_completed ORDER BY date ASC";

    model.setQuery(queryString);
    err = model.lastError();
    if (err.type() != QSqlError::NoError) qDebug() << "Query error: " << err.text() << Qt::endl;

    nRows = model.rowCount();

    for (int i{0}; i < nRows; i++)
    {
        bool day_completed = model.record(i).value("is_completed").toBool();
        if (day_completed)
        {
            QString dateString = model.record(i).value("date").toString();
            double dateDouble = QDateTime::fromString(dateString,"yyyy-MM-dd").toSecsSinceEpoch();
            completed_dates.push_back(dateDouble);
            completed_date_strings.push_back(dateString);
        }
    }

    populateCompletedVectors();


    // initially, set x range to be the last 30 days
    plotEndDay = dates[dates.size()-1];
    plotStartDay = plotEndDay - (30*SEC_IN_DAY);
    plot->xAxis->setRange(plotStartDay,plotEndDay);

}

void statsPlotModel::updateDaysData(QDate qdate, QVector<double> updatedStats)
{
    // first, find the date in our data vectors
    int dateIndex = findDateIndex(qdate,dates);
    if (dateIndex < 0)
    {
        // date not found, which means we need to add it.
        dateIndex = insertDate(qdate,dates);
        for (unsigned int i{0}; i < nutrientStats.size(); ++i)
        {
            plotNutrient n = static_cast<plotNutrient>(i);
            // if dateIndex is still -1, that means the new value was pushed onto the end of the vector
            if (dateIndex < 0)
                nutrientStats[n]->push_back(updatedStats[i]);
            else
                *nutrientStats[n]->insert(dateIndex,updatedStats[i]);
        }
    }
    else
    {
        //replace values in data vectors at this index
        for (unsigned int i{0}; i < nutrientStats.size(); ++i)
        {
            plotNutrient n = static_cast<plotNutrient>(i);
            QVector<double> *statPtr = nutrientStats[n];
            (*statPtr)[dateIndex] = updatedStats[i];
        }
    }

    // check if the date is also in the vector for completed dates
    // If it is, update the data points, but otherwise, do not bother
    // adding the data,
    dateIndex = findDateIndex(qdate,completed_dates);
    if (dateIndex >=0)
    {
        //replace values in data vectors at this index
        for (unsigned int i{0}; i < completeNutrientStats.size(); ++i)
        {
            plotNutrient n = static_cast<plotNutrient>(i);
            QVector<double> *statPtr = completeNutrientStats[n];
            (*statPtr)[dateIndex] = updatedStats[i];
        }
    }
    refreshPlot();
}

void statsPlotModel::setLoggingCompleted(bool status)
{
    if (status)
    {
        // logging has been completed for the day, so we need to push the last entries from the stats vectors
        // onto the comleted stats vectors
        pushDataToCompletedVectors();
    }
    else
    {
        // this will only occur if the day had previously been checked as completed and then unchecked,
        // which means we simply need to remove the last entryies from the completed vectors
        popDataFromCompletedVectors();
    }
    plot->replot();
}

// function inserts date in the date vector in chronological spot and returns index of position
// if the date ends up simply being pushed onto the end of the vector, we return -1
int statsPlotModel::insertDate(QDate qdate, QVector<double> &dates)
{
    double date = convertDateToDouble(qdate);
    for (int i{0}; i < dates.size(); ++i)
    {
        if (date < dates[i]-5) // include five second buffer
        {
            dates.insert(i,date);
            return i;
        }
    }

    // if we have reached the end of hte for loop without returning, this means that
    // all values in the dates vector are smaller than the insertion date, so we simply
    // push the insertion date onto the end of the vector.
    dates.push_back(date);
    return -1;
}

int statsPlotModel::findDateIndex(QDate qdate, QVector<double> dates)
{
    // first convert QDate to double:
    double date = convertDateToDouble(qdate);
    return findDateIndex(date,dates);
}

int statsPlotModel::findDateIndex(double date, QVector<double> dates)
{
    for (int i{0}; i < dates.size(); ++i)
    {
        if (areSameDay(date,dates[i])) return i;
    }

    // if date is not found, return -1
    return -1;
}

double statsPlotModel::getTodaysNetCalories()
{
    QDate qdate = QDate::currentDate();
    int i = findDateIndex(qdate,dates);
    if (i < 0) return 0.0;

    return net_calories[i];
}

void statsPlotModel::populateCompletedVectors()
{
    int date_ptr = 0;
    for (int i = 0; i < completed_dates.size(); ++i)
    {
        while (dates[date_ptr] < completed_dates[i]) date_ptr++;
        // date_ptr is now set to the index where the nutrient vectors are containing data for a completed data.
        // We push this day's data onto the vectors that only contain data from completed days.
        pushDataToCompletedVectors(date_ptr);
    }

}

void statsPlotModel::popDataFromCompletedVectors()
{
    completed_dates.pop_back();
    completed_food_calories.pop_back();
    completed_exercise_calories.pop_back();
    completed_net_calories.pop_back();
    completed_tot_fat.pop_back();
    completed_sat_fat.pop_back();
    completed_cholesterol.pop_back();
    completed_sodium.pop_back();
    completed_carbs.pop_back();
    completed_fiber.pop_back();
    completed_sugar.pop_back();
    completed_protein.pop_back();
}

void statsPlotModel::pushDataToCompletedVectors()
{
    // this version of the function is called when we want to take the last entries from the data vectors
    // and push them to the completed vectors.
    int i = dates.size()-1;
    completed_dates.push_back(dates[i]);
    pushDataToCompletedVectors(i);
}

void statsPlotModel::pushDataToCompletedVectors(int date_ptr)
{
    completed_food_calories.push_back(food_calories[date_ptr]);
    completed_exercise_calories.push_back(exercise_calories[date_ptr]);
    completed_net_calories.push_back(net_calories[date_ptr]);
    completed_tot_fat.push_back(tot_fat[date_ptr]);
    completed_sat_fat.push_back(sat_fat[date_ptr]);
    completed_cholesterol.push_back(cholesterol[date_ptr]);
    completed_sodium.push_back(sodium[date_ptr]);
    completed_carbs.push_back(carbs[date_ptr]);
    completed_fiber.push_back(fiber[date_ptr]);
    completed_sugar.push_back(sugar[date_ptr]);
    completed_protein.push_back(protein[date_ptr]);
}

// resets all nutrient vectors to length zero
// this is called before we gra
void statsPlotModel::setVectorsToZero()
{
    for (auto i : nutrientStats)
    {
        i.second->resize(0);
    }
    dates.resize(0);
}

void statsPlotModel::refreshPlot()
{
    // first, remove all plots
    qDebug() << "refreshPlot: " << Qt::endl;
    QList<QAbstractButton*> checkboxes = checkBoxGroup->buttons();
    for (qsizetype i = 0; i < checkboxes.size(); ++i) {
        // grab index of each button
        int id = checkBoxGroup->id(checkboxes[i]);
        // remove old graph, if it exists
        removeNutritionPlot(static_cast<plotNutrient>(id));


        if (checkboxes[i]->isChecked())
            addNutritionPlot(static_cast<plotNutrient>(id));
    }
}

void statsPlotModel::setCheckBoxColors()
{
    if (checkBoxGroup != NULL) // check that the group has been initialized
    {
        QList<QAbstractButton*> checkboxes = checkBoxGroup->buttons();
        for (qsizetype i = 0; i < checkboxes.size(); ++i) {
            // grab index of each button
            int id = checkBoxGroup->id(checkboxes[i]);
            QPalette pal;
            pal.setColor(QPalette::Active, QPalette::WindowText,plotColors[static_cast<plotNutrient>(id)]);
            checkboxes[i]->setPalette(pal);
        }
    }
}

void statsPlotModel::addNutritionPlot(plotNutrient nutrient)
{
    // use the enum to grab the pointer to the data from unordered map "nutrientStats"
    QVector<double>* statsPtr;
    QVector<double>* datesPtr;
    if (plotCompleteDays)
    {
        statsPtr = completeNutrientStats[nutrient];
        datesPtr = &completed_dates;
    }
    else
    {
        statsPtr = nutrientStats[nutrient];
        datesPtr = &dates;
    }
    // check if this nutrient has already been plotted
    if (activePlots.find(nutrient) == activePlots.end())
    {
        QCPGraph* graph;
        if ((nutrient == plotFoodCalories) || (nutrient == plotExerciseCalories) || (nutrient == plotNetCalories) || (nutrient == plotSodium))
            graph = plot->addGraph();
        else
            graph = plot->addGraph(plot->xAxis,plot->yAxis2);

        graph->setPen(QPen(plotColors[nutrient]));
        graph->setData(*datesPtr,*statsPtr);
        graph->setName(legendNames[nutrient]);
        graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,5));
        // store index number of this graph in "activePlots"
        activePlots[nutrient] = graph;
    }
    else // plot already exists, so we just need to update the data
    {
        // grab plot index from activePlots
        QCPGraph* graph = activePlots[nutrient];
        graph->setData(*datesPtr,*statsPtr);

    }
    autoResizeAxes();
    plot->replot();
}

void statsPlotModel::removeNutritionPlot(plotNutrient nutrient)
{
    // check if this nutrient has already been plotted
    if (activePlots.find(nutrient) != activePlots.end())
    {
        // grab plot index from activePlots
        QCPGraph* graph = activePlots[nutrient];

        plot->removeGraph(graph);
        // remove plot index from activePlots
        activePlots.erase(nutrient);
        autoResizeAxes();
        plot->replot();

    }
}

void statsPlotModel::resizeXAxis()
{
    plotStartDay = dates[0];
    plotEndDay = dates[dates.size()-1] + SEC_IN_DAY;
    plot->xAxis->setRange(plotStartDay,plotEndDay);
    plot->replot();
}

void statsPlotModel::resizeXAxis(QDate startDate, QDate endDate)
{
    QString startDateString = dateToString(startDate);
    QString endDateString = dateToString(endDate);
    plotStartDay = QDateTime::fromString(startDateString,"yyyy-MM-dd").toSecsSinceEpoch();
    plotEndDay = QDateTime::fromString(endDateString,"yyyy-MM-dd").toSecsSinceEpoch() + SEC_IN_DAY;
    plot->xAxis->setRange(plotStartDay,plotEndDay);
    plot->replot();
}

void statsPlotModel::autoResizeAxes()
{
    // iterate through all data currently plotted and find max y value
    double maxLeftYAxis = 0;
    double maxRightYAxis = 0;
    for (auto i = activePlots.begin(); i != activePlots.end(); ++i)
    {
        plotNutrient nutrient =  i->first;
        QCPGraph* graph = i->second;
        QSharedPointer<QCPGraphDataContainer> dataPtr = graph->data();
        for (auto j = dataPtr->constBegin(); j != dataPtr->constEnd(); ++j)
        {
            double date = j->mainKey();
            double value = j->mainValue();
            if ((date >= plotStartDay) && (date <= plotEndDay))
            {
                // check whether this signal is plotted on left or right Y axis.
                if (leftYAxisSignals.indexOf(nutrient) != -1)
                {
                    if (value > maxLeftYAxis) maxLeftYAxis = value;
                } else {
                    if (value > maxRightYAxis) maxRightYAxis = value;
                }
            }
        }
    }

    // Multiply by 1.05 to give extra 5% of headroom
    plot->yAxis->setRange(0,1.05*maxLeftYAxis);
    plot->yAxis2->setRange(0,1.05*maxRightYAxis);
    plot->replot();
}

QVector<QString> statsPlotModel::getCompletedDateStrings() const
{
    return completed_date_strings;
}

const QVector<double> * statsPlotModel::getCompletedNetCaloriesPtr() const
{
    return &completed_net_calories;
}

const QVector<double> * statsPlotModel::getCompletedDatesPtr() const
{
    return &completed_dates;
}
