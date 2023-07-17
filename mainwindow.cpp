#include <QFileDialog>

#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("127.0.0.1");
    db.setUserName("postgres");
    db.setPassword("password");
    db.setDatabaseName("wellness");

    // set date
    currentDate = QDate::currentDate();
    ui->labelCurrentDate->setText(dateToString(currentDate,displayFormat));

    if (!db.open()) {
        QMessageBox::information(this, "Not Connected", "Database is not connected!");
        return;
    }

    if (!db.driver()->hasFeature(QSqlDriver::QuerySize))
    {
        qDebug() << "Database does NOT report query size" << Qt::endl;
    }

    dialog = new weightLossPlanDialog(this);
    //recipeDialog = new createRecipeDialog(this);
    queryModel = new QSqlQueryModel();
    querySearchModel = new QSqlQueryModel();
    foodSearchProxy = new foodSearchProxyModel(querySearchModel);
    listRecipesModel = new QSqlQueryModel();
    recipeEditTableModel = new recipeEditModel();
    fetchRecipes();

    weightModel = new WeightTable(ui->weightPlot,ui->bodyFatPlot);
    weightModel->setTable("weight");
    weightModel->select();
    weightModel->loadDataFromModel();
    weightModel->setHeaderData(weightModel->fieldIndex("date"), Qt::Horizontal, tr("Date"));
    weightModel->setHeaderData(weightModel->fieldIndex("weight"), Qt::Horizontal, tr("Weight (lbs)"));
    weightModel->setHeaderData(weightModel->fieldIndex("bodyfat"), Qt::Horizontal, tr("Body Fat %"));
    weightModel->setHeaderData(4, Qt::Horizontal, tr("Fat (lbs)"));
    ui->tableWeight->setModel(weightModel);
    ui->tableWeight->verticalHeader()->setVisible(false);
    ui->tableWeight->setSortingEnabled(true);
    ui->tableWeight->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    runningModel = new runningDataModel(ui->runningPlotDistance,ui->runningPlotPace,ui->runningPlotHeartRate);
    runningModel->setTable("running_log");
    runningModel->select();
    runningModel->loadDataFromModel();

    ui->tableRunning->setModel(runningModel);
    ui->tableRunning->verticalHeader()->setVisible(false);
    ui->tableRunning->setSortingEnabled(true);
    ui->tableRunning->hideColumn(0);
    ui->tableRunning->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // hide ID column
    int idIdx = weightModel->fieldIndex("id");
    ui->tableWeight->setColumnHidden(idIdx, true);

    //ui->groupBoxWeightLossPlanSummary->labelWeightLossPlanStartDate->setText("test");

    foodLibraryModel = new QSqlTableModel();
    foodLibraryModel->setTable("food_library");
    foodLibraryModel->select();
    foodLibraryModel->setHeaderData(foodLibraryModel->fieldIndex("name"), Qt::Horizontal, tr("Name"));
    foodLibraryModel->setHeaderData(foodLibraryModel->fieldIndex("description"), Qt::Horizontal, tr("Description"));
    foodLibraryModel->setHeaderData(foodLibraryModel->fieldIndex("units"), Qt::Horizontal, tr("Units"));
    foodLibraryModel->setHeaderData(foodLibraryModel->fieldIndex("serving_size"), Qt::Horizontal, tr("Serving"));
    foodLibraryModel->setHeaderData(foodLibraryModel->fieldIndex("calories"), Qt::Horizontal, tr("Calories"));
    foodLibraryModel->setHeaderData(foodLibraryModel->fieldIndex("tot_fat"), Qt::Horizontal, tr("Total Fat"));
    foodLibraryModel->setHeaderData(foodLibraryModel->fieldIndex("sat_fat"), Qt::Horizontal, tr("Sat. Fat"));
    foodLibraryModel->setHeaderData(foodLibraryModel->fieldIndex("cholesterol"), Qt::Horizontal, tr("Cholesterol"));
    foodLibraryModel->setHeaderData(foodLibraryModel->fieldIndex("sodium"), Qt::Horizontal, tr("Sodium"));
    foodLibraryModel->setHeaderData(foodLibraryModel->fieldIndex("carbs"), Qt::Horizontal, tr("Carbs"));
    foodLibraryModel->setHeaderData(foodLibraryModel->fieldIndex("fiber"), Qt::Horizontal, tr("Fiber"));
    foodLibraryModel->setHeaderData(foodLibraryModel->fieldIndex("sugar"), Qt::Horizontal, tr("Sugar"));
    foodLibraryModel->setHeaderData(foodLibraryModel->fieldIndex("protein"), Qt::Horizontal, tr("Protein"));
    ui->tableFoodLibrary->setModel(foodLibraryModel);
    ui->tableFoodLibrary->verticalHeader()->setVisible(false);
    ui->tableFoodLibrary->setSortingEnabled(true);
    ui->tableFoodLibrary->setColumnHidden(0, true);
    ui->tableFoodLibrary->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableFoodLibrary->horizontalHeader()->setSectionResizeMode(1,QHeaderView::ResizeToContents);
    ui->tableFoodLibrary->horizontalHeader()->setSectionResizeMode(2,QHeaderView::ResizeToContents);
    initializeFoodLogTables();

    QFont font;
    font.setBold(true);
    ui->tableFoodSearchResults->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableFoodSearchResults->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableFoodSearchResults->verticalHeader()->setVisible(false);
    ui->tableFoodSearchResults->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableFoodSearchResults->horizontalHeader()->setFont(font);

    ui->tableRecipeEdit->horizontalHeader()->setFont(font);
    ui->tableRecipeEdit->verticalHeader()->setVisible(false);

    ui->tableFoodLibrary->setModel(foodLibraryModel);
    ui->tableFoodLibrary->verticalHeader()->setVisible(false);
    ui->tableFoodLibrary->setSortingEnabled(true);

    btnGroup = new QButtonGroup();
    btnGroup->addButton(ui->radioBtnFoodSearch,foodSearch);
    btnGroup->addButton(ui->radioBtnRecipeSearch,recipeSearch);
    btnGroup->addButton(ui->radioBtnMealSearch,mealSearch);
    btnGroup->addButton(ui->radioBtnExerciseSearch,exerciseSearch);
    ui->radioBtnFoodSearch->setChecked(true);

    ui->comboBoxPlotTimeUnits->insertItem(CB_Days,"Days");
    ui->comboBoxPlotTimeUnits->insertItem(CB_Weeks,"Weeks");
    ui->comboBoxPlotTimeUnits->insertItem(CB_Months,"Months");
    ui->comboBoxPlotTimeUnits->insertItem(CB_Years,"Years");

    ui->comboBoxWeightPlotTimeUnits->insertItem(CB_Days,"Days");
    ui->comboBoxWeightPlotTimeUnits->insertItem(CB_Weeks,"Weeks");
    ui->comboBoxWeightPlotTimeUnits->insertItem(CB_Months,"Months");
    ui->comboBoxWeightPlotTimeUnits->insertItem(CB_Years,"Years");

    // set initial values for spin box
    ui->spinBoxPlotDuration->setValue(7);
    ui->spinBoxWeightPlotDuration->setValue(7);

    plotModel = new statsPlotModel(ui->customPlot);
    plotModel->checkBoxGroup->addButton(ui->checkBoxFoodCalories,statsPlotModel::plotFoodCalories);
    plotModel->checkBoxGroup->addButton(ui->checkBoxExerciseCalories,statsPlotModel::plotExerciseCalories);
    plotModel->checkBoxGroup->addButton(ui->checkBoxNetCalories,statsPlotModel::plotNetCalories);
    plotModel->checkBoxGroup->addButton(ui->checkBoxTotFat,statsPlotModel::plotTotFat);
    plotModel->checkBoxGroup->addButton(ui->checkBoxSatFat,statsPlotModel::plotSatFat);
    plotModel->checkBoxGroup->addButton(ui->checkBoxCholesterol,statsPlotModel::plotCholesterol);
    plotModel->checkBoxGroup->addButton(ui->checkBoxSodium,statsPlotModel::plotSodium);
    plotModel->checkBoxGroup->addButton(ui->checkBoxCarbs,statsPlotModel::plotCarbs);
    plotModel->checkBoxGroup->addButton(ui->checkBoxFiber,statsPlotModel::plotFiber);
    plotModel->checkBoxGroup->addButton(ui->checkBoxSugar,statsPlotModel::plotSugar);
    plotModel->checkBoxGroup->addButton(ui->checkBoxProtein,statsPlotModel::plotProtein);
    plotModel->setCheckBoxColors();
    plotModel->getDataFromDB();
    plotModel->addNutritionPlot(statsPlotModel::plotNetCalories);
    ui->checkBoxNetCalories->setChecked(true);

    loggingCompletedToday = updateLoggingCompletedCheckMark();

    weightLossModel = new weightLossPlanModel(plotModel,ui->plotWeightLossPlan,ui->plotWeeklyCalories,loggingCompletedToday);
    weightLossModel->setWeightVectorPointers(weightModel->getWeightVectorPtr(),weightModel->getDateVectorPtr());
    makeConnections();
    weightLossModel->loadWeightLossPlan();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::plotExample()
{
    // get sum of calories for each day
    // hard-code this query for now for testing

    QString queryString = "SELECT date,ROUND(SUM(food_log.serving_size/food_library.serving_size*food_library.calories),0) "
                  "AS calories from food_log "
                  "INNER JOIN food_library ON food_log.food_id = food_library.food_id "
                  "GROUP BY date "
                  "ORDER BY date DESC";

    QSqlQueryModel model;
    model.setQuery(queryString);
    QSqlError err = model.lastError();
    if (err.type() != QSqlError::NoError) qDebug() << err.text() << Qt::endl;

    int nRows = model.rowCount();
    QVector<double> dates(nRows);
    QVector<double> calories(nRows);
    for (int i{0}; i < nRows; i++)
    {
        calories[i] = model.record(i).value("calories").toDouble();
        QString dateString = model.record(i).value("date").toString();
        dates[i] = QDateTime::fromString(dateString,"yyyy-MM-dd").toSecsSinceEpoch();
    }
    ui->customPlot->addGraph();
    ui->customPlot->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    dateTicker->setDateTimeFormat("dd-MM-yyyy");
    ui->customPlot->xAxis->setTicker(dateTicker);

    ui->customPlot->graph(0)->setData(dates,calories);
    ui->customPlot->xAxis->setLabel("Date");
    ui->customPlot->yAxis->setLabel("Calories");
    ui->customPlot->xAxis->setRange(dates[0],dates[dates.size()-1]);
    ui->customPlot->yAxis->setRange(0,2500);
    ui->customPlot->replot();
}

void MainWindow::buttonDateChangeClicked(int n)
{
    // decrement current date by one.
    currentDate = currentDate.addDays(n);
    // update date label
    ui->labelCurrentDate->setText(dateToString(currentDate,displayFormat));
    updateLoggingCompletedCheckMark();
    // update date on mealsStruct
    mealsStruct->updateDate(dateToString(currentDate,dBFormat));
    exerciseMod->setDate(dateToString(currentDate,dBFormat));
    // update food tables
    mealsStruct->refresh();
    exerciseMod->refresh();
    nutrients->updateData();
}

bool MainWindow::updateLoggingCompletedCheckMark()
{
    // check whether there is an entry in the database for today, and if so, if it's true or false
    // "True" indicates that the logging has been completed for the day.
    QString date = dateToString(currentDate,dBFormat);
    QString queryString = "SELECT is_completed FROM day_completed WHERE date='"+date+"'";
    QSqlQueryModel model;
    model.setQuery(queryString);
    QSqlError err = model.lastError();
    if (err.type() != QSqlError::NoError) qDebug() << err.text() << Qt::endl;

    int nRows = model.rowCount();
    bool dayCompleted = false;
    if (nRows > 0) dayCompleted = model.record(0).value("is_completed").toBool();
    ui->checkBoxDayCompleted->setChecked(dayCompleted);

    return dayCompleted;
}

void MainWindow::makeConnections()
{
    connect(ui->buttonFoodSearch,SIGNAL(clicked()),this,SLOT(buttonFoodSearchClicked()));
    connect(ui->buttonAddFood,SIGNAL(clicked()),this,SLOT(buttonAddFoodClicked()));
    connect(ui->buttonLogWeight,SIGNAL(clicked()),this,SLOT(buttonLogWeightClicked()));
    connect(ui->buttonLogRunning,SIGNAL(clicked()),this,SLOT(buttonLogRunningClicked()));


    QObject::connect(ui->foodSearch,SIGNAL(returnPressed()),this,SLOT(buttonFoodSearchClicked()));

    connect(ui->buttonAddBreakfast,&QPushButton::clicked,this,[this] {buttonAddToMeal(0);});
    connect(ui->buttonAddLunch,&QPushButton::clicked,this,[this] {buttonAddToMeal(1);});
    connect(ui->buttonAddDinner,&QPushButton::clicked,this,[this] {buttonAddToMeal(2);});
    connect(ui->buttonAddSnacks,&QPushButton::clicked,this,[this] {buttonAddToMeal(3);});
    connect(ui->buttonAddExercise,&QPushButton::clicked,this,[this] {buttonAddExercise();});

    connect(ui->buttonDeleteBreakfast,&QPushButton::clicked,this,[this] {buttonDeleteFromMeal(0);});
    connect(ui->buttonDeleteLunch,&QPushButton::clicked,this,[this] {buttonDeleteFromMeal(1);});
    connect(ui->buttonDeleteDinner,&QPushButton::clicked,this,[this] {buttonDeleteFromMeal(2);});
    connect(ui->buttonDeleteSnacks,&QPushButton::clicked,this,[this] {buttonDeleteFromMeal(3);});
    connect(ui->buttonDeleteExercise,&QPushButton::clicked,this,[this] {buttonDeleteExercise();});

    connect(ui->buttonDatePrev,&QPushButton::clicked,this,[this]{buttonDateChangeClicked(-1);});
    connect(ui->buttonDateNext,&QPushButton::clicked,this,[this]{buttonDateChangeClicked(1);});

    connect(ui->buttonReplotNutrientData,&QPushButton::clicked,this,[this]{buttonReplotNutrientDataClicked();});
    connect(ui->buttonPlotAllNutrientData,&QPushButton::clicked,this,[this]{buttonPlotAllNutrientDataClicked();});
    connect(ui->buttonWeightReplot,&QPushButton::clicked,this,[this]{buttonWeightReplotClicked();});
    connect(ui->buttonPlotAllWeightData,&QPushButton::clicked,this,[this]{buttonPlotAllWeightDataClicked();});
    connect(ui->checkBoxDayCompleted,&QCheckBox::stateChanged,this,&MainWindow::checkBoxDayLoggingClicked);
    connect(ui->checkBoxCompleteDaysOnly,&QCheckBox::stateChanged,this,&MainWindow::checkBoxCompleteDaysOnlyClicked);

    connect(ui->buttonImportCsv,SIGNAL(clicked()),this,SLOT(buttonImportCsvFile()));
    connect(plotModel->checkBoxGroup,&QButtonGroup::idToggled,this,&MainWindow::checkBoxToggled);
    connect(btnGroup,&QButtonGroup::buttonClicked,this,&MainWindow::radioButtonClicked);
    connect(ui->checkBoxShowLegend,&QCheckBox::stateChanged,this,&MainWindow::toggleNutritionLegend);
    connect(weightLossModel,&weightLossPlanModel::setPlanSummary,this,&MainWindow::setPlanSummaryFields);
    connect(weightLossModel,&weightLossPlanModel::setProgressSummary,this,&MainWindow::setProgressSummaryFields);

    connect(ui->buttonUpdateWeightLossPlan,&QPushButton::clicked,this,&MainWindow::updateWeightLossPlanClicked);
    connect(dialog,&QDialog::finished,this,&MainWindow::weightLossDialogFinished);

    connect(ui->buttonCreateRecipe,&QPushButton::clicked,this,&MainWindow::createRecipeClicked);
    connect(ui->tableRecipes,&QAbstractItemView::clicked,this,&MainWindow::tableRecipesClicked);

    connect(recipeEditTableModel,&recipeTableModel::macrosUpdated,this,&MainWindow::updateRecipeMacros);
    connect(nutrients,&nutrientsModel::nutrientsUpdated,this,&MainWindow::updatePlotData);
    //connect(recipeDialog,&QDialog::finished,this,&MainWindow::createRecipeDialogFinished);
}

void MainWindow::updatePlotData(QVector<double> stats)
{
    plotModel->updateDaysData(currentDate, stats);
    weightLossModel->updateTodaysCaloriesBar();
}

void MainWindow::updateRecipeMacros()
{
    QVector<double> stats = recipeEditTableModel->getMacroTotals();

    double calories = std::floor(stats[0]);
    double fatGrams = roundDec(stats[1],1);
    double carbsGrams = roundDec(stats[2],1);
    double proteinGrams = roundDec(stats[3],1);
    ui->labelCalories->setText(QString::number(calories));
    ui->labelFatGrams->setText(QString::number(fatGrams)+"g");
    ui->labelCarbsGrams->setText(QString::number(carbsGrams)+"g");
    ui->labelProteinGrams->setText(QString::number(proteinGrams)+"g");

    double totalCalories = 9*stats[1]+4*(stats[2]+stats[3]);
    if (totalCalories > 0)
    {
        double fatPercent = 9*stats[1]/totalCalories*100.0;
        double carbsPercent = 4*stats[2]/totalCalories*100.0;
        double proteinPercent = 4*stats[3]/totalCalories*100.0;
        // round to a single decimal place
        fatPercent = roundDec(fatPercent,1);
        carbsPercent = roundDec(carbsPercent,1);
        proteinPercent = roundDec(proteinPercent,1);
        ui->labelFatPercent->setText(QString::number(fatPercent)+"%");
        ui->labelCarbsPercent->setText(QString::number(carbsPercent)+"%");
        ui->labelProteinPercent->setText(QString::number(proteinPercent)+"%");
    }
    else
    {
        ui->labelFatPercent->setText("--");
        ui->labelCarbsPercent->setText("--");
        ui->labelProteinPercent->setText("--");
    }
}

void MainWindow::tableRecipesClicked(const QModelIndex &index)
{
    // retrieve the recipe id
    int recipe_id = listRecipesModel->index(index.row(),0).data().toInt();
    if (recipe_id != recipeEditTableModel->getCurrentRecipeID())
    {
        // Query the database and retrieve ingredients for recipe that was selected, then load them
        // into the table

        QString queryString = "select food_library.food_id,food_library.name,description,recipe_ingredients.serving_size,units,"
                              "round(recipe_ingredients.serving_size/food_library.serving_size*calories) AS calories,"
                              "round(recipe_ingredients.serving_size/food_library.serving_size*tot_fat) AS tot_fat,"
                              "round(recipe_ingredients.serving_size/food_library.serving_size*carbs) AS carbs,"
                              "round(recipe_ingredients.serving_size/food_library.serving_size*protein) AS protein,"
                              "food_library.serving_size "
                              "FROM recipe_ingredients INNER JOIN food_library ON recipe_ingredients.food_id = food_library.food_id "
                              "WHERE recipe_id = " + QString::number(recipe_id);
        queryModel->setQuery(queryString);
        QSqlError err = queryModel->lastError();
        if (err.type() != QSqlError::NoError) qDebug() << err.text() << Qt::endl;
        recipeEditTableModel->loadRecipe(recipe_id, queryModel);
        ui->tableRecipeEdit->setModel(recipeEditTableModel);
        ui->tableRecipeEdit->horizontalHeader()->hideSection(0);
        ui->tableRecipeEdit->horizontalHeader()->hideSection(9);
        ui->tableRecipeEdit->horizontalHeader()->hideSection(10);
        ui->tableRecipeEdit->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    }
    qDebug() << "recipe id selected: " << recipe_id << Qt::endl;
}

void MainWindow::updateWeightLossPlanClicked()
{
    dialog->setModal(true);
    dialog->open();
}

void MainWindow::createRecipeClicked()
{
    recipeDialog = new createRecipeDialog(this);
    connect(recipeDialog,&QDialog::finished,this,&MainWindow::createRecipeDialogFinished);
    recipeDialog->setModal(true);
    recipeDialog->open();
}

void MainWindow::createRecipeDialogFinished(int result)
{
    if (result == QDialog::Accepted)
    {
        QVector<int> recipeIds = recipeDialog->getRecipeIds();
        QVector<double> recipeServingSizes = recipeDialog->getRecipeServingSizes();
        QString recipeName = recipeDialog->getRecipeName();

        QString insertString = "INSERT INTO recipes(recipe_name) VALUES('"+recipeName+"') RETURNING recipe_id";

        queryModel->setQuery(insertString);
        QSqlError err = queryModel->lastError();
        if (err.type() != QSqlError::NoError) qDebug() << err.text() << Qt::endl;

        QString recipe_id = QString::number(queryModel->record(0).value("recipe_id").toInt());
        qDebug() << "Recipe added to database. Id: " << recipe_id << Qt::endl;

        insertString = "INSERT INTO recipe_ingredients VALUES";
        for (int i{0}; i < recipeIds.size(); ++i)
        {
            insertString += "(" + recipe_id + "," + QString::number(recipeIds[i]) + "," + QString::number(recipeServingSizes[i]) + "),";
        }
        insertString.removeLast(); // remove last comma

        queryModel->setQuery(insertString);
        err = queryModel->lastError();
        if (err.type() != QSqlError::NoError) qDebug() << err.text() << Qt::endl;
    }
    delete recipeDialog;
}

void MainWindow::weightLossDialogFinished(int result)
{
    if (result == QDialog::Accepted)
    {
        QDate startDate = dialog->getStartDate();
        double tdee = dialog->getTdee();
        double goalWeight = dialog->getGoalWeight();
        double lossRate = dialog->getLossRate();
        weightLossModel->setWeightLossPlan(startDate, tdee, goalWeight, lossRate);
    }
}

void MainWindow::setPlanSummaryFields(QDate start_date, QVector<double> plan_values)
{
    ui->labelWeightLossPlanStartDate->setText(dateToString(start_date,dBFormat));
    ui->labelWeightLossPlanTDEE->setText(QString::number(plan_values[0]) + " cal");
    ui->labelWeightLossPlanGoalWeight->setText(QString::number(plan_values[1]) + " lbs");
    ui->labelWeightLossPlanTargetLossPace->setText(QString::number(plan_values[2]) + " lbs/week");
    ui->labelWeightLossPlanDailyCaloricDeficit->setText(QString::number(plan_values[3]) + " cal");
}

void MainWindow::setProgressSummaryFields(QVector<double> progress_values)
{
    // round estimated and actual weight loss values (indices 2 and 3) to 2 decimal places
    double estWeightLoss = std::ceil(progress_values[4]*100.0)/100.0;
    double actualWeightLoss = std::ceil(progress_values[5]*100.0)/100.0;
    double actualLossRate = std::ceil(progress_values[6]*100.0)/100.0;

    ui->labelDaysSinceStart->setText(QString::number(progress_values[0]));
    ui->labelAverageDailyCalories->setText(QString::number(qRound(progress_values[1])) + " cal");
    ui->labelTotalCalorieDelta->setText(QString::number(qRound(progress_values[2])) + " cal");
    ui->labelMeanDailyDeficit->setText(QString::number(qRound(progress_values[3])) + " cal");
    ui->labelEstimatedWeightLoss->setText(QString::number(estWeightLoss)  + " lbs");
    ui->labelActualWeightLoss->setText(QString::number(actualWeightLoss) + " lbs");
    ui->labelActualWeightLossRate->setText(QString::number(actualLossRate) + " lbs/week");
    ui->labelCalculatedTDEE->setText(QString::number(qRound(progress_values[7])) + " cal");
    ui->labelTimeToGoalWeight->setText(QString::number(qRound(progress_values[8])) + " days");
}

void MainWindow::toggleNutritionLegend(int state)
{
    plotModel->toggleLegend(state);
}

void MainWindow::checkBoxCompleteDaysOnlyClicked(int state)
{
    if (state == Qt::Unchecked)
        plotModel->setCompleteDaysFlag(false);
    else
        plotModel->setCompleteDaysFlag(true);
    plotModel->refreshPlot();
}

void MainWindow::checkBoxDayLoggingClicked(int state)
{
    // if the state
    QString queryString;
    QString date = dateToString(currentDate,dBFormat);
    QSqlQueryModel model;
    if (state == Qt::Unchecked)
    {
        queryString = "UPDATE day_completed SET is_completed=false WHERE date='"+date+"'";
    }
    else
    {
        //queryString = "UPDATE day_completed SET is_completed=false WHERE date='"+date+"'";
        queryString = "do $$ "
                      "BEGIN "
                      "IF EXISTS(SELECT * FROM day_completed WHERE date = '"+date+"') "
                      "THEN "
                      "UPDATE day_completed SET is_completed=true WHERE date = '"+date+"'; "
                      "ELSE "
                      "INSERT INTO day_completed VALUES('"+date+"',true); "
                      "END IF; end; $$";
    }

    model.setQuery(queryString);
    QSqlError err = model.lastError();
    if (err.type() != QSqlError::NoError) qDebug() << err.text() << Qt::endl;

    // if "currentDate" is today, then we need to update loggingCompletedToday flag
    if (currentDate == QDate::currentDate())
    {
        if (state == Qt::Unchecked)
            loggingCompletedToday = false;
        else
            loggingCompletedToday = true;
        weightLossModel->setLoggingCompletedFlag(loggingCompletedToday);
    }

}

void MainWindow::checkBoxToggled(int id, bool checked)
{
    // check status of the check box. If it has been checked, then we add the appropriate plot to the graph.
    if (checked)
        plotModel->addNutritionPlot(static_cast<statsPlotModel::plotNutrient>(id));
    else
        plotModel->removeNutritionPlot(static_cast<statsPlotModel::plotNutrient>(id));
}

void MainWindow::radioButtonClicked()
{
    // check if the search bar is empty
    QString searchText = ui->foodSearch->text();
    if (!searchText.isEmpty())
    {
        buttonFoodSearchClicked();
    }

}

void MainWindow::initializeFoodLogTables()
{
    /*
    breakfast = new mealStruct(0);
    lunch = new mealStruct(1);
    dinner = new mealStruct(2);
    snacks = new mealStruct(3);
*/
    mealsStruct = new mealsStructure(dateToString(currentDate));
    exerciseMod = new exerciseModel(dateToString(currentDate));

    std::vector<QTableView*> tables = {ui->tableBreakfast, ui->tableLunch, ui->tableDinner, ui->tableSnacks,ui->tableMacrosBreakfast,ui->tableMacrosLunch,ui->tableMacrosDinner,ui->tableMacrosSnacks,ui->tableNutrients,ui->tableExercise};

    int i = 0;
    QFont font;
    font.setBold(true);
    for (auto & table: tables) {
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table->horizontalHeader()->setFont(font);
        if (((i >= 4) && (i <= 7)) || (i == 8))
            table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table->verticalHeader()->setVisible(false);
        if (i > 3) table->horizontalHeader()->setVisible(false);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        table->setSelectionMode(QAbstractItemView::SingleSelection);
        i++;
    }



    ui->tableBreakfast->setModel(mealsStruct->meals[0]->proxyModel);
    ui->tableLunch->setModel(mealsStruct->meals[1]->proxyModel);
    ui->tableDinner->setModel(mealsStruct->meals[2]->proxyModel);
    ui->tableSnacks->setModel(mealsStruct->meals[3]->proxyModel);
    ui->tableExercise->setModel(exerciseMod);

    ui->tableMacrosBreakfast->setModel(mealsStruct->meals[0]->mealMacros);
    ui->tableMacrosLunch->setModel(mealsStruct->meals[1]->mealMacros);
    ui->tableMacrosDinner->setModel(mealsStruct->meals[2]->mealMacros);
    ui->tableMacrosSnacks->setModel(mealsStruct->meals[3]->mealMacros);



    //breakfast->mealModel->refresh();
    //lunch->mealModel->refresh();
    //dinner->mealModel->refresh();
    //snacks->mealModel->refresh();
    //breakfast->mealMacros->updateData();
    //lunch->mealMacros->updateData();
    //dinner->mealMacros->updateData();
    //snacks->mealMacros->updateData();

    nutrients = new nutrientsModel(mealsStruct,exerciseMod);
    ui->tableNutrients->setModel(nutrients);

    for (int i = 0; i < 4; i++) {
        mealsStruct->meals[i]->refresh();
        QObject::connect(mealsStruct->meals[i]->mealModel,SIGNAL(dataChanged(QModelIndex,QModelIndex)),nutrients,SLOT(updateData()));
        QObject::connect(mealsStruct->meals[i]->mealModel,SIGNAL(dataChanged(QModelIndex,QModelIndex)),mealsStruct->meals[i]->mealMacros,SLOT(updateData()));
    }
    exerciseMod->refresh();
    QObject::connect(exerciseMod,SIGNAL(dataChanged(QModelIndex,QModelIndex)),nutrients,SLOT(updateData()));
    nutrients->updateData();
}

void MainWindow::buttonLogWeightClicked()
{
    // grab text from search field
    QString weight = ui->inputWeight->text();
    QString bodyFat = ui->inputBF->text();

    QString insertString = "INSERT INTO weight(date,weight,bodyfat) VALUES(CURRENT_DATE,";
    insertString += weight + "," + bodyFat + ");";
    //queryModel = new QSqlQueryModel();
    queryModel->setQuery(insertString);
    QSqlError err = queryModel->lastError();
    if (err.type() != QSqlError::NoError) qDebug() << err.text() << Qt::endl;
    weightModel->select();
    ui->tableWeight->setModel(weightModel);
    weightModel->loadDataFromModel();
    ui->tableWeight->verticalHeader()->setVisible(false);
    ui->tableWeight->setSortingEnabled(true);

    // update weight loss plan
    weightLossModel->loadWeightLossPlan();
}

void MainWindow::buttonLogRunningClicked()
{
    // grab text from search field
    QString miles = ui->inputMiles->text();
    QString minutes = ui->inputMinutes->text();
    QString calories = ui->inputRunningCalories->text();
    QString heartRate = ui->inputHeartRate->text();

    QString insertString = "INSERT INTO running_log(date,distance_mi,calories,avg_heart_rate,duration_min) VALUES(CURRENT_DATE,";
    insertString += miles + "," + calories + "," + heartRate + "," + minutes + ");";
    //queryModel = new QSqlQueryModel();
    queryModel->setQuery(insertString);
    ui->tableRunning->setModel(runningModel);
    ui->tableRunning->verticalHeader()->setVisible(false);
    ui->tableRunning->setSortingEnabled(true);

    runningModel->loadDataFromModel();
}


void MainWindow::buttonAddFoodClicked()
{
    QString name = ui->foodName->text();
    QString description = ui->foodDescription->text();
    cleanUpString(name);
    cleanUpString(description);
    QString servingSize = ui->foodServingSize->text();
    QString units = ui->foodUnits->text();

    QString calories = ui->foodCalories->text();
    QString totalFat = ui->foodTotalFat->text();
    QString satFat = ui->foodSatFat->text();
    QString cholesterol = ui->foodCholesterol->text();
    QString sodium = ui->foodSodium->text();
    QString carbs = ui->foodCarbs->text();
    QString fiber = ui->foodFiber->text();
    QString sugar = ui->foodSugar->text();
    QString protein = ui->foodProtein->text();

    QString insertString = "INSERT INTO food_library(name,description,units,serving_size,calories,tot_fat,sat_fat, cholesterol,sodium,carbs,fiber,sugar,protein) VALUES('";
    insertString += name + "','";
    insertString += description + "','";
    insertString += units + "',";
    insertString += servingSize + ",";
    insertString += calories + "," + totalFat + "," + satFat + "," + cholesterol + "," + sodium + "," + carbs + "," + fiber + "," + sugar + "," + protein + ");";
    //queryModel = new QSqlQueryModel();
    queryModel->setQuery(insertString);
    QSqlError err = queryModel->lastError();
    if (err.type() != QSqlError::NoError) qDebug() << err.text() << Qt::endl;
    foodLibraryModel->select();
}

void MainWindow::cleanUpString(QString& str) {
    // check string for any single apostrophes, replace with double apostrophes
    int i{0};
    while (i < str.length()) {
        if ( (str[i] == QChar('\'')) && ( (i == str.length()-1) || (str[i+1] != QChar('\'')) )) {
            qDebug() << "Apostrophe found at index: " << i << Qt::endl;
            str.insert(i+1, QString("'"));
            i+=2;
        } else {
            i++;
        }
    }
}

void MainWindow::fetchRecipes()
{
    QString queryString = "SELECT * from recipes";
    listRecipesModel->setQuery(queryString);
    ui->tableRecipes->setModel(listRecipesModel);
    ui->tableRecipes->horizontalHeader()->setVisible(false);
    ui->tableRecipes->verticalHeader()->setVisible(false);
    ui->tableRecipes->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableRecipes->setColumnHidden(0, true);
    ui->tableRecipes->setShowGrid(false);
}

void MainWindow::buttonFoodSearchClicked()
{
    QString searchText = ui->foodSearch->text();
    //QAbstractButton *btn = btnGroup->checkedButton();
    int id = btnGroup->checkedId();
    if (id == foodSearch)
    {
        QString queryString = "SELECT food_id,name,description,serving_size,units FROM food_library WHERE name ILIKE '%" + searchText + "%' or description ILIKE '%" + searchText + "%'";
        //queryModel = new QSqlQueryModel();
        querySearchModel->setQuery(queryString);
        querySearchModel->setHeaderData(0,Qt::Horizontal,tr("ID"));
        querySearchModel->setHeaderData(1,Qt::Horizontal,tr("Name"));
        querySearchModel->setHeaderData(2,Qt::Horizontal,tr("Desc."));
        querySearchModel->setHeaderData(3,Qt::Horizontal,tr("Serving"));
        querySearchModel->setHeaderData(4,Qt::Horizontal,tr("Units"));
        ui->tableFoodSearchResults->setModel(foodSearchProxy);
        ui->tableFoodSearchResults->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    }
    else if (id == recipeSearch)
    {

    }
    else if (id == mealSearch)
    {
        QString queryString = "DROP TABLE IF EXISTS meal_table; SELECT distinct food_log.date,food_log.meal into temp table meal_table FROM food_log "
                              "INNER JOIN food_library ON food_log.food_id = food_library.food_id "
                                "WHERE food_library.name ILIKE '%" + searchText + "%';";
        QString queryString2 = "select food_log.date,food_log.meal,food_library.name,food_log.food_id,food_log.serving_size from food_log "
                               "INNER JOIN meal_table ON food_log.date = meal_table.date AND food_log.meal = meal_table.meal "
                               "INNER JOIN food_library ON food_log.food_id = food_library.food_id "
                               "ORDER BY food_log.date DESC,food_log.meal DESC";
        //queryModel = new QSqlQueryModel();
        queryModel->setQuery(queryString);
        QSqlError err = queryModel->lastError();
        if (err.type() != QSqlError::NoError) qDebug() << err.text() << Qt::endl;
        queryModel->setQuery(queryString2);
        err = queryModel->lastError();
        if (err.type() != QSqlError::NoError) qDebug() << err.text() << Qt::endl;
        mealSummary = new mealSummaryModel(queryModel);
        mealSummary->setHeaderData(0,Qt::Horizontal,tr("Date"));
        mealSummary->setHeaderData(1,Qt::Horizontal,tr("Meal"));
        mealSummary->setHeaderData(2,Qt::Horizontal,tr("Foods"));
        ui->tableFoodSearchResults->setModel(mealSummary);
        ui->tableFoodSearchResults->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
        int col_width = ui->tableFoodSearchResults->horizontalHeader()->sectionSize(0);
        col_width *= 1.2;
        ui->tableFoodSearchResults->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Fixed);
        ui->tableFoodSearchResults->horizontalHeader()->resizeSection(0,col_width);

        ui->tableFoodSearchResults->horizontalHeader()->setSectionResizeMode(1,QHeaderView::ResizeToContents);
        col_width = ui->tableFoodSearchResults->horizontalHeader()->sectionSize(1);
        col_width *= 1.2;

        ui->tableFoodSearchResults->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Fixed);
        ui->tableFoodSearchResults->horizontalHeader()->resizeSection(1,col_width);

        ui->tableFoodSearchResults->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Stretch);

        ui->tableFoodSearchResults->resizeRowsToContents();
    }
    else
    {
        QString queryString = "SELECT * FROM exercise_library WHERE exercise_name ILIKE '%" + searchText + "%'";
        //queryModel = new QSqlQueryModel();
        queryModel->setQuery(queryString);
        ui->tableFoodSearchResults->setModel(queryModel);
        ui->tableFoodSearchResults->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    }
}


void MainWindow::buttonAddToMeal(int mealNum)
{
    QItemSelectionModel *select = ui->tableFoodSearchResults->selectionModel();
    // check status of radio button to see if we are adding a single food or entire meal
    int id = btnGroup->checkedId();
    if (id == foodSearch)
        addFoodToMeal(select, mealNum);
    else
        addMealToMeal(select, mealNum);
}

void MainWindow::buttonAddExercise()
{
    QItemSelectionModel *select = ui->tableFoodSearchResults->selectionModel();
    // check status of radio button to see if we are adding a single food or entire meal
    int id = btnGroup->checkedId();
    if (id == exerciseSearch)
        addExercise(select);
}

void MainWindow::buttonDeleteExercise()
{
    QItemSelectionModel *select = ui->tableExercise->selectionModel();
    // check status of radio button to see if we are adding a single food or entire meal
    deleteExercise(select);
}

void MainWindow::addExercise(QItemSelectionModel *select)
{
    if (select != NULL && select->hasSelection()) {
        qDebug() << "addExercise called." << Qt::endl;
        QModelIndexList selection = select->selectedRows();
        QModelIndex index = selection.at(0);
        int exercise_id = ui->tableFoodSearchResults->model()->index(index.row(),0).data().toInt();
        // add this food to our database for the day's food log
        addExerciseToLog(exercise_id);
    } else {
        qDebug() << "selectionModel has NO selection" << Qt::endl;
    }
}

void MainWindow::deleteExercise(QItemSelectionModel *select)
{
    if (select != NULL && select->hasSelection()) {
        qDebug() << "deleteExercise called." << Qt::endl;
        QModelIndexList selection = select->selectedRows();
        QModelIndex index = selection.at(0);
        int exercise_id = ui->tableExercise->model()->index(index.row(),3).data().toInt();
        // add this food to our database for the day's food log
        deleteExerciseFromLog(exercise_id);
    } else {
        qDebug() << "selectionModel has NO selection" << Qt::endl;
    }
}

void MainWindow::addExerciseToLog(int exercise_id)
{
    QString insertString = "INSERT INTO exercise_log(date,exercise_id,duration_min,calories) "
                           "VALUES('"+dateToString(currentDate)+"',"+QString::number(exercise_id)+",10,100);";
    //queryModel = new QSqlQueryModel();
    queryModel->setQuery(insertString);
    QSqlError err = queryModel->lastError();
    if (err.type() != QSqlError::NoError) qDebug() << err.text() << Qt::endl;
    exerciseMod->refresh();
    nutrients->updateData();
}

void MainWindow::deleteExerciseFromLog(int exercise_id)
{
    QString insertString = "DELETE FROM exercise_log WHERE date='"+dateToString(currentDate)+"' AND exercise_id="+QString::number(exercise_id);

    //"DELETE FROM food_log where date='"+dateToString(currentDate)+"' AND meal=:meal AND food_id=:food_id"
    //queryModel = new QSqlQueryModel();
    queryModel->setQuery(insertString);
    QSqlError err = queryModel->lastError();
    if (err.type() != QSqlError::NoError) qDebug() << err.text() << Qt::endl;
    exerciseMod->refresh();
    nutrients->updateData();
}

void MainWindow::buttonDeleteFromMeal(int mealNum)
{
    std::vector<QTableView*> tables = {ui->tableBreakfast,ui->tableLunch,ui->tableDinner,ui->tableSnacks};
    //QTableView*
    QItemSelectionModel *select = tables[mealNum]->selectionModel();
    deleteFoodFromMeal(select, tables[mealNum], mealNum);
}

void MainWindow::addFoodToMeal(QItemSelectionModel *select, int mealNo)
{
    if (select != NULL && select->hasSelection()) {
        qDebug() << "selectionModel has selection" << Qt::endl;
        QModelIndexList selection = select->selectedRows();
        QModelIndex index = selection.at(0);
        qDebug() << index.row();
        QAbstractProxyModel *proxyModel = qobject_cast<QAbstractProxyModel*>(ui->tableFoodSearchResults->model());
        int food_id = proxyModel->sourceModel()->index(index.row(),0).data().toInt();
        double serving_size = proxyModel->sourceModel()->index(index.row(),3).data().toDouble();
        // add this food to our database for the day's food log
        addFoodToLog(mealNo,food_id,serving_size);
    } else {
        qDebug() << "selectionModel has NO selection" << Qt::endl;
    }
}

void MainWindow::addMealToMeal(QItemSelectionModel *select, int mealNo)
{
    if (select != NULL && select->hasSelection()) {
        qDebug() << "addMealToMeal: selectionModel has selection" << Qt::endl;
        QModelIndexList selection = select->selectedRows();
        QModelIndex index = selection.at(0);
        qDebug() << index.row();
        QString date = ui->tableFoodSearchResults->model()->index(index.row(),0).data().toString();
        QString sourceMeal = ui->tableFoodSearchResults->model()->index(index.row(),1).data().toString();
        int sourceMealNum = mealLabelMap[sourceMeal];
        // add this food to our database for the day's food log
        addMealToLog(mealNo,date,sourceMealNum);
    } else {
        qDebug() << "addMealToMeal:  selectionModel has NO selection" << Qt::endl;
    }
}

void MainWindow::deleteFoodFromMeal(QItemSelectionModel *select, QTableView* table, int mealNo)
{
    if (select != NULL && select->hasSelection()) {
        qDebug() << "selectionModel has selection" << Qt::endl;
        QModelIndexList selection = select->selectedRows();
        QModelIndex index = selection.at(0);
        qDebug() << index.row();
        QAbstractProxyModel *proxyModel = qobject_cast<QAbstractProxyModel*>(table->model());
        int food_id = proxyModel->sourceModel()->index(index.row(),15).data().toInt();
        // delete this food from our database for the day's food log
        deleteFoodFromLog(mealNo,food_id);
    } else {
        qDebug() << "selectionModel has NO selection" << Qt::endl;
    }
}

void MainWindow::addFoodToLog(int meal, int food_id, double serving_size) {
    QString insertString = "INSERT INTO food_log(date,meal,food_id,serving_size) VALUES('"+dateToString(currentDate)+"',";
    insertString += QString::number(meal) + ",";
    insertString += QString::number(food_id) + ",";
    insertString += QString::number(serving_size) + ");";
    //queryModel = new QSqlQueryModel();
    queryModel->setQuery(insertString);
    mealsStruct->meals[meal]->refresh();
    //breakfast->mealModel->refresh();
    nutrients->updateData();
    //mealsStruct->meals[meal]->mealMacros->updateData();
}

void MainWindow::addMealToLog(int destMealNum, QString date, int sourceMealNum) {
    QString insertString = "WITH myconst (var1,var2) as (values (date('"+dateToString(currentDate)+"'),"+QString::number(destMealNum)+")) "
        "INSERT INTO food_log SELECT var1,var2,food_id,serving_size from myconst,food_log "
                           "WHERE date='"+date+"' AND meal=" + QString::number(sourceMealNum);
    qDebug() << "addMealToLog:" << Qt::endl << "dest meal: " << QString::number(destMealNum) << ", date: " << date << ", source meal: " << QString::number(sourceMealNum) << Qt::endl;
    //queryModel = new QSqlQueryModel();
    queryModel->setQuery(insertString);
    QSqlError err = queryModel->lastError();
    if (err.type() != QSqlError::NoError) qDebug() << err.text() << Qt::endl;
    mealsStruct->meals[destMealNum]->refresh();
    nutrients->updateData();
    //mealsStruct->meals[meal]->mealMacros->updateData();
}

void MainWindow::deleteFoodFromLog(int meal, int food_id) {
    QSqlQuery query;
    query.prepare("DELETE FROM food_log where date='"+dateToString(currentDate)+"' AND meal=:meal AND food_id=:food_id");
    query.bindValue(":meal",meal);
    query.bindValue(":food_id",food_id);
    if (query.exec()) {
        mealsStruct->meals[meal]->refresh();
        //meals[meal]->mealModel->refresh();
        nutrients->updateData();
        //meals[meal]->mealMacros->updateData();
    }
}

void MainWindow::buttonImportCsvFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Directory"),
                                               "C:/Users/cspop/OneDrive/Documents/Personal",
                                               tr("CSV Files (*.csv)"));

    //qDebug() << "file name : " << fileName << Qt::endl;


    QString insertString = "COPY food_library(name,description,units,serving_size,calories,tot_fat,sat_fat,cholesterol,sodium,carbs,fiber,sugar,protein) FROM '" + fileName + "' DELIMITER ',' CSV HEADER";
    //queryModel = new QSqlQueryModel();
    queryModel->setQuery(insertString);
    QSqlError err = queryModel->lastError();
    if (err.type() != QSqlError::NoError) qDebug() << err.text() << Qt::endl;
    foodLibraryModel->select();
}

void MainWindow::buttonReplotNutrientDataClicked()
{
    // get timeframe from the spin box and combo box.
    int duration = -(ui->spinBoxPlotDuration->value());
    int unitsIndex = ui->comboBoxPlotTimeUnits->currentIndex();


    QDate startDay;
    QDate today = QDate::currentDate();
    switch(unitsIndex) {
    case CB_Days:
        startDay = today.addDays(duration);
        break;
    case CB_Weeks:
        startDay = today.addDays(7*duration);
        break;
    case CB_Months:
        startDay = today.addMonths(duration);
        break;
    case CB_Years:
        startDay = today.addYears(duration);
        break;
    default:
        qDebug() << "Error in buttonPlotDataClicked(): no case found in switch statement." << Qt::endl;
    }

    //plotModel->getDataFromDB(startDay,today);
    plotModel->resizeXAxis(startDay,today);
}

void MainWindow::buttonPlotAllNutrientDataClicked()
{
    plotModel->resizeXAxis();
}

void MainWindow::buttonWeightReplotClicked()
{
    // get timeframe from the spin box and combo box.
    int duration = -(ui->spinBoxWeightPlotDuration->value());
    int unitsIndex = ui->comboBoxWeightPlotTimeUnits->currentIndex();


    QDate startDay;
    QDate today = QDate::currentDate();
    switch(unitsIndex) {
    case CB_Days:
        startDay = today.addDays(duration);
        break;
    case CB_Weeks:
        startDay = today.addDays(7*duration);
        break;
    case CB_Months:
        startDay = today.addMonths(duration);
        break;
    case CB_Years:
        startDay = today.addYears(duration);
        break;
    default:
        qDebug() << "Error in buttonPlotDataClicked(): no case found in switch statement." << Qt::endl;
    }

    weightModel->resizeXAxis(startDay,today);
}

void MainWindow::buttonPlotAllWeightDataClicked()
{
    weightModel->resizeXAxis();
}
