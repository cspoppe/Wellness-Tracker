
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include <QSqlDatabase>
#include <QMessageBox>
#include "qtableview.h"
#include "weighttable.h"
//#include "editablesqlmodel.h"
#include "nutrientsmodel.h"
//#include "mealproxymodel.h"
//#include "macrosmodel.h"
#include "mealstruct.h"
#include "mealsstructure.h"
#include "qcustomplot.h"
#include "mealsummarymodel.h"
#include "exercisemodel.h"
#include "statsplotmodel.h"
#include "foodsearchproxymodel.h"
#include "weightlossplanmodel.h"
#include "utilityfcns.h"
#include "weightlossplandialog.h"
#include "runningdatamodel.h"
#include "createrecipedialog.h"
#include "recipeeditmodel.h"
#include "foodlibrarytablemodel.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow

{
    Q_OBJECT

public:
    QSqlDatabase db;
    QSqlQueryModel *queryModel;
    QSqlQueryModel *querySearchModel;
    WeightTable *weightModel;
    runningDataModel *runningModel;
    foodLibraryTableModel *foodLibraryModel;
    nutrientsModel *nutrients;
    foodSearchProxyModel *foodSearchProxy;
    weightLossPlanModel *weightLossModel;
    weightLossPlanDialog *dialog;
    createRecipeDialog *recipeDialog;
    mealsStructure *mealsStruct;
    exerciseModel *exerciseMod;
    mealSummaryModel *mealSummary;
    QButtonGroup* btnGroup;
    statsPlotModel* plotModel;
    QSqlQueryModel* listRecipesModel;
    recipeEditModel *recipeEditTableModel;

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    // buttons
    void buttonLogWeightClicked();
    void buttonLogRunningClicked();
    void buttonAddFoodClicked();
    void buttonFoodSearchClicked();
    void buttonAddToMeal(int mealNum);
    void buttonAddExercise();
    void buttonDeleteExercise();
    void buttonDeleteFromMeal(int mealNum);
    void buttonImportCsvFile();
    void buttonDateChangeClicked(int n);
    void buttonReplotNutrientDataClicked();
    void buttonPlotAllNutrientDataClicked();
    void buttonWeightReplotClicked();
    void buttonPlotAllWeightDataClicked();
    void checkBoxDayLoggingClicked(int state);
    void checkBoxCompleteDaysOnlyClicked(int state);
    void toggleNutritionLegend(int state);
    void setPlanSummaryFields(QDate start_date, QVector<double> plan_values);
    void setProgressSummaryFields(QVector<double> progress_values);
    void updateWeightLossPlanClicked();
    void createRecipeClicked();
    void weightLossDialogFinished(int result);
    void createRecipeDialogFinished(int result);
    void tableRecipesClicked(const QModelIndex &index);
    void updateRecipeMacros();
    void updatePlotData(QVector<double> stats);
    void submitChanges();
    void setEnableSubmitRevertButtons(bool status);

    void cleanUpString(QString& str);
    void addFoodToLog(int meal, int food_id, double serving_size);
    void addMealToLog(int destMealNum, QString date, int sourceMealNum);
    void addExerciseToLog(int exercise_id);
    void deleteExerciseFromLog(int exercise_id);
    void deleteFoodFromLog(int meal, int food_id, int row_index);
    void initializeFoodLogTables();
    void addFoodToMeal(QItemSelectionModel *select,  int mealNo);
    void addMealToMeal(QItemSelectionModel *select, int mealNo);
    void addExercise(QItemSelectionModel *select);
    void deleteExercise(QItemSelectionModel *select);
    void deleteFoodFromMeal(QItemSelectionModel *select, QTableView* table, int mealNo);
    void checkBoxToggled(int id, bool checked);
    void radioButtonClicked();
    void fetchRecipes();
private:
    bool loggingCompletedToday;
    enum radioButton {foodSearch, recipeSearch, mealSearch, exerciseSearch};
    enum comboBoxValue {CB_Days,CB_Weeks, CB_Months, CB_Years};
    QDate currentDate;
    void makeConnections();
    bool updateLoggingCompletedCheckMark();
    void plotExample();
    //std::vector<mealStruct*> mealsVector;
    Ui::MainWindow *ui;
    std::unordered_map<QString, int> mealLabelMap =
        {
        {"Breakfast",0},
        {"Lunch",1},
        {"Dinner",2},
        {"Snacks",3}
    };

};

#endif // MAINWINDOW_H
