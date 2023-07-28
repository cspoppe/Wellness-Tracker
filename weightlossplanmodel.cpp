
#include "weightlossplanmodel.h"
#include "utilityfcns.h"
#include <QtSql>
#include <QDateTime>
#include <QTime>

weightLossPlanModel::weightLossPlanModel(WeightTable *weightTableModel, statsPlotModel *stats, QCustomPlot *weightPlot, QCustomPlot *weeklyPlot, bool loggingCompleted) : statsModel(stats), weightLossPlot(weightPlot),
    weeklyWeightPlot(weeklyPlot), loggingCompletedTodayFlag(loggingCompleted)
{
    weightVector = weightTableModel->getWeightVectorPtr();
    weightDatesVector = weightTableModel->getDateVectorPtr();

    tickToday = {0.0};
    todayCalories = {0.0};

    completedDatesVector = statsModel->getCompletedDatesPtr();
    completedNetCaloriesVector = statsModel->getCompletedNetCaloriesPtr();
    weeklyCaloriesTable = new weeklyCaloriesModel(loggingCompletedTodayFlag);

    tickLabelFont.setPointSize(12);
    labelFont.setPointSize(14);
    labelFont.setStyleStrategy(QFont::PreferAntialias);

    weightLossPlot->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    dateTicker->setDateTimeFormat("MMM d");
    weightLossPlot->xAxis->setTicker(dateTicker);
    //weightLossPlot->xAxis->setLabel("Date");
    weightLossPlot->yAxis->setLabel("Weight (lbs)");
    weightLossPlot->xAxis->setTickLabelFont(tickLabelFont);
    weightLossPlot->xAxis->setLabelFont(labelFont);
    weightLossPlot->yAxis->setTickLabelFont(tickLabelFont);
    weightLossPlot->yAxis->setLabelFont(labelFont);
    weightLossPlot->legend->setVisible(true);
    weightGraph = weightLossPlot->addGraph();
    weightGraph->setPen(QPen(Qt::blue));
    weightGraph->setName("Weight");
    weightGraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,5));

    weightFitGraph = weightLossPlot->addGraph();
    weightFitGraph->setPen(QPen(Qt::red));
    weightFitGraph->setName("Best Fit");

    TDEEGraph = weeklyWeightPlot->addGraph();
    TDEEGraph->setName("TDEE");
    weeklyNetCaloriesBar = new QCPBars(weeklyWeightPlot->xAxis,weeklyWeightPlot->yAxis);
    weeklyExcessCaloriesBar = new QCPBars(weeklyWeightPlot->xAxis,weeklyWeightPlot->yAxis);
    weeklyDeficitCaloriesBar = new QCPBars(weeklyWeightPlot->xAxis,weeklyWeightPlot->yAxis);
    //todayCaloriesBar = new QCPBars(weeklyWeightPlot->xAxis,weeklyWeightPlot->yAxis);

    QPen tdeePen;
    tdeePen.setColor(Qt::red);
    tdeePen.setStyle(Qt::DashLine);
    tdeePen.setWidthF(2);
    TDEEGraph->setPen(tdeePen);

    weeklyNetCaloriesBar->setName("Net");
    weeklyDeficitCaloriesBar->setName("Deficit");
    weeklyExcessCaloriesBar->setName("Excess");

    //todayCaloriesBar->setAntialiased(false);
    weeklyNetCaloriesBar->setAntialiased(false);
    weeklyExcessCaloriesBar->setAntialiased(false);
    weeklyDeficitCaloriesBar->setAntialiased(false);


    weeklyNetCaloriesBar->setStackingGap(0);
    weeklyExcessCaloriesBar->setStackingGap(0);
    weeklyDeficitCaloriesBar->setStackingGap(0);

    //todayCaloriesBar->setPen(QPen(QColor(255,108,34)));
    //todayCaloriesBar->setBrush(QColor(255,108,34).lighter(170));
    weeklyNetCaloriesBar->setPen(QPen(QColor(0,0,255)));
    weeklyNetCaloriesBar->setBrush(QColor(0,0,255).lighter(170));
    weeklyExcessCaloriesBar->setPen(QPen(QColor(255,0,0)));
    weeklyExcessCaloriesBar->setBrush(QColor(255,0,0).lighter(170));
    weeklyDeficitCaloriesBar->setPen(QPen(QColor(0,255,0)));
    weeklyDeficitCaloriesBar->setBrush(QColor(0,255,0).lighter(170));

    weeklyDeficitCaloriesBar->moveAbove(weeklyNetCaloriesBar);
    weeklyExcessCaloriesBar->moveAbove(weeklyNetCaloriesBar);

    TDEEGraph->setAntialiased(false);

    //loadWeightLossPlan();
    //calculateProgress();
}

/*
void weightLossPlanModel::setWeightVectorPointers(const QVector<double> *weight, const QVector<double> *dates)
{
    weightVector = weight;
    weightDatesVector = dates;
}
*/

void weightLossPlanModel::createTodayCaloriesBar()
{
    todayCaloriesBar = new QCPBars(weeklyWeightPlot->xAxis,weeklyWeightPlot->yAxis);
    todayCaloriesBar->setAntialiased(false);
    todayCaloriesBar->setPen(QPen(QColor(255,108,34)));
    todayCaloriesBar->setBrush(QColor(255,108,34).lighter(170));
    todayCaloriesBar->setName("Today");
}

void weightLossPlanModel::loadWeightLossPlan()
{
    qDebug() << "loadWeightLossPlan..." << Qt::endl;
    QSqlQueryModel model;
    QString queryString = "SELECT * FROM weight_loss_plan";
    model.setQuery(queryString);

    QSqlError err = model.lastError();
    if (err.type() != QSqlError::NoError) qDebug() << "Query error: " << err.text() << Qt::endl;

    if (model.rowCount() == 0) return;

    QVector<double> plan_values;
    QString dateString = model.record(0).value("start_date").toString();
    qDebug() << "date: " << dateString << Qt::endl;
    planStartDate = QDate::fromString(dateString,"yyyy-MM-dd");
    TDEE_estimated = model.record(0).value("TDEE").toDouble();
    weeklyCaloriesTable->setTDEE(TDEE_estimated);
    goalWeight = model.record(0).value("goal_weight").toDouble();
    weight_loss_rate_weekly = model.record(0).value("loss_rate_target").toDouble();
    daily_caloric_deficit = weight_loss_rate_weekly*3500/7.0;

    plan_values.push_back(TDEE_estimated);
    plan_values.push_back(goalWeight);
    plan_values.push_back(weight_loss_rate_weekly);
    plan_values.push_back(daily_caloric_deficit);

    qDebug() << plan_values << Qt::endl;

    weeklyCaloriesTable->loadData(TDEE_estimated,completedDatesVector,completedNetCaloriesVector);

    plotWeeklyCalories();

    emit setPlanSummary(planStartDate, plan_values);
}

void weightLossPlanModel::plotWeeklyCalories()
{
    // determine which week we are in, i.e., what's the most recent Monday.
    // Weeks start on Monday, end on Sunday.

    // this function return 1 for Monday, 2 for Tues.,
    // For our purposes, we want to begin with 0 for Monday.
    QDate today = QDate::currentDate();
    todayIndex = today.dayOfWeek()-1;
    double currentDate = QDateTime(today,QTime()).toSecsSinceEpoch();
    double startOfWeek = currentDate - todayIndex*SEC_IN_DAY+60; // adding 60 seconds makes it easier to compare to vector of logged dates by simple inequalities

    // grab data beginning with this day and after
    int i = completedDatesVector->size()-1;
    while ((*completedDatesVector)[i] > startOfWeek) --i;

    QVector<double> datesThisWeek = completedDatesVector->sliced(i);
    weeklyExcessCaloriesPlotVector = {};
    weeklyDeficitCaloriesPlotVector = {};
    weeklyNetCaloriesPlotVector = {};
    QVector<double> netCaloriesThisWeek = completedNetCaloriesVector->sliced(i);

    int nPoints = netCaloriesThisWeek.size();

    double dayOfWeek = startOfWeek - 60;
    int netCalIndex = 0;

    QVector<double> ticks;
    QVector<double> ticksNet;
    QVector<double> ticksDeficit;
    QVector<double> ticksExcess;
    for (int j{1}; j <= 7; ++j)
    {
        ticks.push_back(j);
        if ((netCalIndex < nPoints) && areSameDay(dayOfWeek,datesThisWeek[netCalIndex]))
        {
            ticksNet.push_back(j);
            if (netCaloriesThisWeek[netCalIndex] > TDEE_estimated)
            {
                weeklyNetCaloriesPlotVector.push_back(TDEE_estimated);
                //weeklyDeficitCaloriesPlotVector.push_back(0.0);
                ticksExcess.push_back(j);
                weeklyExcessCaloriesPlotVector.push_back(netCaloriesThisWeek[netCalIndex] - TDEE_estimated);
            } else if (netCaloriesThisWeek[netCalIndex] < TDEE_estimated)
            {
                weeklyNetCaloriesPlotVector.push_back(netCaloriesThisWeek[netCalIndex]);
                ticksDeficit.push_back(j);
                weeklyDeficitCaloriesPlotVector.push_back(TDEE_estimated - netCaloriesThisWeek[netCalIndex]);
                //weeklyExcessCaloriesPlotVector.push_back(0.0);
            } else {
                weeklyNetCaloriesPlotVector.push_back(netCaloriesThisWeek[netCalIndex]);
                //weeklyDeficitCaloriesPlotVector.push_back(0.0);
                //weeklyExcessCaloriesPlotVector.push_back(0.0);
            }
            ++netCalIndex;
        }
        else
        {
            //weeklyNetCaloriesPlotVector.push_back(0.0);
            //weeklyDeficitCaloriesPlotVector.push_back(0.0);
            //weeklyExcessCaloriesPlotVector.push_back(0.0);
        }
        dayOfWeek += SEC_IN_DAY;
    }

    // if logging today is not completed, we need to grab today's calories so far and store it separately so it can be plotted with its own color.
    if (!loggingCompletedTodayFlag)
    {
        createTodayCaloriesBar();
        tickToday[0] = (todayIndex+1);
        //todayCalories = statsModel->getTodaysCalories();
        todayCalories[0] = (statsModel->getTodaysNetCalories());
        weeklyCaloriesTable->updateTodaysCount(statsModel->getTodaysNetCalories());
        todayCaloriesBar->setData(tickToday,todayCalories);
    }
    else
    {
        // remove the bar for today's incomplete calories, if it is there.
        if (weeklyWeightPlot->hasPlottable(todayCaloriesBar)) weeklyWeightPlot->removePlottable(todayCaloriesBar);
    }

    // set up labels for x axis
    QDate date = today.addDays(-todayIndex); // set to the first day
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);

    QVector<QString> labels;
    for (int i{1}; i <= 7; ++i)
    {
        labels.push_back(date.toString("ddd MMM d"));
        date = date.addDays(1);
    }
    textTicker->addTicks(ticks, labels);
    weeklyWeightPlot->xAxis->setTicker(textTicker);
    weeklyWeightPlot->xAxis->setSubTicks(false);
    weeklyWeightPlot->xAxis->setTickLength(0,4);
    weeklyWeightPlot->xAxis->setRange(0,9);
    //weeklyWeightPlot->xAxis->setLabel("Day");
    weeklyWeightPlot->xAxis->setLabelFont(labelFont);
    weeklyWeightPlot->xAxis->setTickLabelFont(tickLabelFont);
    weeklyWeightPlot->yAxis->setLabelFont(labelFont);
    weeklyWeightPlot->yAxis->setTickLabelFont(tickLabelFont);
    weeklyWeightPlot->legend->setVisible(true);

    double yMax = vectorMax(netCaloriesThisWeek)+500;
    if (yMax < TDEE_estimated) yMax = TDEE_estimated + 500;
    weeklyWeightPlot->yAxis->setRange(0,yMax);
    weeklyWeightPlot->yAxis->setLabel("Calories");

    TDEEGraph->setData({0,8},{TDEE_estimated,TDEE_estimated});
    weeklyNetCaloriesBar->setData(ticksNet,weeklyNetCaloriesPlotVector);
    weeklyExcessCaloriesBar->setData(ticksExcess,weeklyExcessCaloriesPlotVector);
    weeklyDeficitCaloriesBar->setData(ticksDeficit,weeklyDeficitCaloriesPlotVector);
    weeklyWeightPlot->replot();
}

void weightLossPlanModel::updateTodaysCaloriesBar()
{
    todayCalories[0] = statsModel->getTodaysNetCalories();
    todayCaloriesBar->setData(tickToday,todayCalories);
    weeklyCaloriesTable->updateTodaysCount(todayCalories[0]);
    weeklyWeightPlot->replot();
}

void weightLossPlanModel::setLoggingCompletedFlag(bool status)
{
    loggingCompletedTodayFlag = status;
    // if the status is true, then we convert today's bar to a completed version,
    // with green to indicate the deficit or red to indicate the excess.
    plotWeeklyCalories();
    weeklyCaloriesTable->setLoggingStatus(status);

}

void weightLossPlanModel::setWeightLossPlan(QDate startDate, double tdee, double weight, double lossRate)
{
    planStartDate = startDate;
    TDEE_estimated = tdee;
    goalWeight = weight;
    weight_loss_rate_weekly = lossRate;
    daily_caloric_deficit = weight_loss_rate_weekly*3500/7.0;

    QSqlQueryModel model;
    QString queryString = "UPDATE weight_loss_plan SET start_date='"+dateToString(planStartDate);
    queryString += "',tdee="+QString::number(TDEE_estimated);
    queryString += ",goal_weight="+QString::number(goalWeight);
    queryString += ",loss_rate_target="+QString::number(weight_loss_rate_weekly);
    model.setQuery(queryString);

    QSqlError err = model.lastError();
    if (err.type() != QSqlError::NoError) qDebug() << "Query error: " << err.text() << Qt::endl;

    QVector<double> plan_values;

    plan_values.push_back(TDEE_estimated);
    plan_values.push_back(goalWeight);
    plan_values.push_back(weight_loss_rate_weekly);
    plan_values.push_back(daily_caloric_deficit);

    emit setPlanSummary(planStartDate, plan_values);
}

void weightLossPlanModel::calculateProgress()
{
    // calculate sum of all completed days' net calories since beginning date of plan.
    // also count number of completed days
    QDate currentDate = QDate::currentDate();
    double startDateDouble = convertDateToDouble(planStartDate);
    double currentDateDouble = convertDateToDouble(currentDate);
    //QVector<double> dates = statsModel->getCompletedDates();
    //QVector<double> netCalories = statsModel->getCompletedNetCalories();

    // find the first date that's on or after the beginning of the plan.
    int start_i = 0;
    for (int i{0}; i < completedDatesVector->size(); ++i)
    {
        if (startDateDouble <= (*completedDatesVector)[i])
        {
            start_i = i;
            break;
        }
    }

    int daysSinceStart = qRound((currentDateDouble - startDateDouble)/SEC_IN_DAY);
    //qDebug() << "calculateProgress:" << Qt::endl;
    int nDays = completedDatesVector->size()-start_i;
    double totalCalories = 0.0;
    for (int i{start_i}; i < completedNetCaloriesVector->size();++i)
    {
        //qDebug() << " date: " << (*completedDatesVector)[i] << ", net calories: " << (*completedNetCaloriesVector)[i] << Qt::endl;
        totalCalories += (*completedNetCaloriesVector)[i];
    }
    // net calories consumed minus est. calories burned
    totalCalorieDelta = totalCalories - nDays*TDEE_estimated;
    //qDebug() << "total calorie delta: " << totalCalorieDelta << Qt::endl;
    averageDailyDelta = totalCalorieDelta/nDays;
    estimatedWeightChange = averageDailyDelta*daysSinceStart/3500.0;

    int nDaysUnlogged = daysSinceStart - nDays;

    double estUnloggedCalories = 2000.0*nDaysUnlogged;

    averageDailyCalories = (totalCalories+estUnloggedCalories)/daysSinceStart;
    calculateWeightLossFit();
    actualWeightChange = weight_change_lbs_per_day*daysSinceStart; // calculated by line of best fit for weight data since the start of the weight loss plan.
    calculatedTDEE = (totalCalories+estUnloggedCalories-3500.0*actualWeightChange)/daysSinceStart;
    double weightToday = (*weightVector)[(*weightVector).size()-1];
    daysToGoal = (goalWeight - weightToday)/weight_change_lbs_per_day;

    QVector<double> progress_values;
    progress_values.push_back(daysSinceStart);
    progress_values.push_back(averageDailyCalories);
    progress_values.push_back(totalCalorieDelta);
    progress_values.push_back(averageDailyDelta);
    progress_values.push_back(estimatedWeightChange);
    progress_values.push_back(actualWeightChange);
    progress_values.push_back(weight_change_lbs_per_day*7);
    progress_values.push_back(calculatedTDEE);
    progress_values.push_back(daysToGoal);

    emit setProgressSummary(progress_values);
}

void weightLossPlanModel::calculateWeightLossFit()
{
    double startDateDouble = convertDateToDouble(planStartDate);
    // find the first date that's on or after the beginning of the plan.
    int start_i = 0;
    for (int i{0}; i < (*weightDatesVector).size(); ++i)
    {
        if (startDateDouble <= (*weightDatesVector)[i])
        {
            start_i = i;
            break;
        }
    }

    QVector<double> x = weightDatesVector->sliced(start_i);
    QVector<double> y = weightVector->sliced(start_i);


    weightGraph->setData(x,y);
    minWeight = *std::min_element(y.begin(),y.end());
    maxWeight = *std::max_element(y.begin(),y.end());
    weightLossPlot->xAxis->setRange(x[0],x[x.size()-1]+SEC_IN_DAY);
    weightLossPlot->yAxis->setRange(minWeight-2,maxWeight+2);
    weightLossPlot->replot();

    //qDebug() << "x[0]: " << x[0] << Qt::endl;
    //qDebug() << "x[n]: " << x[x.size()-1] << Qt::endl;

    // First, since dates are stored in seconds since 1970 and I'm concerned about doing math with such large values, especially squares,
    // I want to remove offset from x vector so that first date is represented as 0.
    double offset = x[0];
    double x_sum = 0.0;
    double y_sum = 0.0;
    for (int i = 0; i < x.size(); ++i)
    {
        x[i] -= offset;
        //x[i] /= SEC_IN_DAY; // convert from seconds to days
        x_sum += x[i];
        y_sum += y[i];
    }

    // calculate mean of x,y values
    double x_mean = x_sum/x.size();
    double y_mean = y_sum/y.size();

    double numerator_sum = 0;
    double denominator_sum = 0;
    for (int i = 0; i < y.size(); ++i)
    {
        numerator_sum += (x[i]-x_mean)*(y[i]-y_mean);
        denominator_sum += (x[i]-x_mean)*(x[i]-x_mean);
    }
    weight_fit_m = numerator_sum/denominator_sum;
    weight_fit_b = y_mean - weight_fit_m*(x_mean+offset);

    weight_change_lbs_per_day = weight_fit_m*SEC_IN_DAY;

    //qDebug() << "weight_fit_m: " << weight_fit_m << Qt::endl;

    // calculate end points of line of best fit so we can plot the line
    double fit_x0 = offset;
    double fit_y0 = weight_fit_m*fit_x0 + weight_fit_b;
    double fit_xn = x[x.size()-1] + offset;
    double fit_yn = weight_fit_m*fit_xn + weight_fit_b;

    QVector<double> x_fit = {fit_x0, fit_xn};
    QVector<double> y_fit = {fit_y0, fit_yn};

    //qDebug() << "x_fit: " << x_fit << Qt::endl;
    //qDebug() << "y_fit: " << y_fit << Qt::endl;

    weightFitGraph->setData(x_fit,y_fit);
    weightLossPlot->replot();


}
