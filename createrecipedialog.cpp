#include "createrecipedialog.h"
#include "ui_createrecipedialog.h"

#include "utilityFcns.h"
#include <QMessageBox>

createRecipeDialog::createRecipeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::createRecipeDialog)
{
    ui->setupUi(this);

    QFont font;
    font.setBold(true);

    connect(ui->buttonSearch,&QPushButton::clicked,this,&createRecipeDialog::buttonSearchClicked);
    connect(ui->buttonAdd,&QPushButton::clicked,this,&createRecipeDialog::buttonAddClicked);
    connect(ui->buttonCancel,&QPushButton::clicked,this,&createRecipeDialog::buttonCancelClicked);
    connect(ui->buttonDelete,&QPushButton::clicked,this,&createRecipeDialog::buttonDeleteClicked);
    connect(ui->buttonCreateRecipe,&QPushButton::clicked,this,&createRecipeDialog::buttonCreateRecipeClicked);

    QObject::connect(ui->inputSearch,SIGNAL(returnPressed()),this,SLOT(buttonSearchClicked()));

    ui->tableIngredientSearchResults->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableIngredientSearchResults->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableIngredientSearchResults->verticalHeader()->setVisible(false);
    ui->tableIngredientSearchResults->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableIngredientSearchResults->horizontalHeader()->setFont(font);

    ui->tableIngredients->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableIngredients->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableIngredients->verticalHeader()->setVisible(false);
    ui->tableIngredients->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableIngredients->horizontalHeader()->setFont(font);

    queryModel = new QSqlQueryModel();
    foodSearchProxy = new foodSearchProxyModel(queryModel);
    foodSearchProxy->addFilteredColumn(6);
    foodSearchProxy->addFilteredColumn(7);
    foodSearchProxy->addFilteredColumn(8);

    recipeModel = new recipeTableModel();
    recipeProxy = new foodSearchProxyModel(recipeModel);
    recipeProxy->addFilteredColumn(9);
    recipeProxy->addFilteredColumn(10);
    recipeModel->setColumnCount(11);
    recipeModel->setHeaderData(0,Qt::Horizontal,tr("ID"));
    recipeModel->setHeaderData(1,Qt::Horizontal,tr("Name"));
    recipeModel->setHeaderData(2,Qt::Horizontal,tr("Desc."));
    recipeModel->setHeaderData(3,Qt::Horizontal,tr("Serving"));
    recipeModel->setHeaderData(4,Qt::Horizontal,tr("Units"));
    recipeModel->setHeaderData(5,Qt::Horizontal,tr("Calories"));
    recipeModel->setHeaderData(6,Qt::Horizontal,tr("Fat (g)"));
    recipeModel->setHeaderData(7,Qt::Horizontal,tr("Carbs (g)"));
    recipeModel->setHeaderData(8,Qt::Horizontal,tr("Protein (g)"));
    recipeModel->setHeaderData(9,Qt::Horizontal,tr("Serving size base"));
    recipeModel->setHeaderData(10,Qt::Horizontal,tr("Old serving size"));
    ui->tableIngredients->setModel(recipeProxy);

    connect(recipeModel,&recipeTableModel::macrosUpdated,this,&createRecipeDialog::updateRecipeMacros);

    // initialize recipe values
    ui->labelCalories->setText("0");
    ui->labelFatGrams->setText("0g");
    ui->labelCarbsGrams->setText("0g");
    ui->labelProteinGrams->setText("0g");

    ui->labelFatPercent->setText("--");
    ui->labelCarbsPercent->setText("--");
    ui->labelProteinPercent->setText("--");
}

createRecipeDialog::~createRecipeDialog()
{
    delete ui;
}

void createRecipeDialog::buttonCancelClicked()
{
    reject();
}

void createRecipeDialog::buttonAddClicked()
{
    QItemSelectionModel *select = ui->tableIngredientSearchResults->selectionModel();
    if (select != NULL && select->hasSelection()) {
        QModelIndexList selection = select->selectedRows();
        QModelIndex index = selection.at(0);
        QAbstractProxyModel *proxyModel = qobject_cast<QAbstractProxyModel*>(ui->tableIngredientSearchResults->model());
        QList<QStandardItem*> newRow;
        for (int i{0}; i < proxyModel->sourceModel()->columnCount(); ++i)
        {
            QStandardItem *item = new QStandardItem(proxyModel->sourceModel()->index(index.row(),i).data().toString());
            newRow.push_back(item);
        }
        //add on two more items:
        // The first is never edited and just saves the base serving size
        // This will not be displayed in the table, only used to calculate and update other values when serving size is edited
        QStandardItem *base_serving_size = new QStandardItem(proxyModel->sourceModel()->index(index.row(),3).data().toString());
        newRow.push_back(base_serving_size);

        // The second item stores what the serving size was most recently set at
        // This is used to calculate the old "serving multiplier" (i.e., serving_size/base_serving_size)
        // This is so we can revert the macro values back to their base value, so they can be correctly updated for
        // the new serving size.
        QStandardItem *old_serving_size = new QStandardItem(proxyModel->sourceModel()->index(index.row(),3).data().toString());
        newRow.push_back(old_serving_size);
        recipeModel->appendRow(newRow);
    }


    // add calories and macros from this new item to the totals for this recipe
    //updateRecipeMacros();
}

void createRecipeDialog::updateRecipeMacros()
{
    QVector<double> stats = recipeModel->getMacroTotals();

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

void createRecipeDialog::buttonSearchClicked()
{
    QString searchText = ui->inputSearch->text();

    QString queryString = "SELECT id,name,description,serving_size,units,calories,tot_fat,carbs,protein FROM food_library WHERE name ILIKE '%" + searchText + "%' or description ILIKE '%" + searchText + "%'";

    queryModel->setQuery(queryString);
    queryModel->setHeaderData(0,Qt::Horizontal,tr("ID"));
    queryModel->setHeaderData(1,Qt::Horizontal,tr("Name"));
    queryModel->setHeaderData(2,Qt::Horizontal,tr("Desc."));
    queryModel->setHeaderData(3,Qt::Horizontal,tr("Serving"));
    queryModel->setHeaderData(4,Qt::Horizontal,tr("Units"));
    queryModel->setHeaderData(5,Qt::Horizontal,tr("Calories"));
    ui->tableIngredientSearchResults->setModel(foodSearchProxy);
    ui->tableIngredientSearchResults->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void createRecipeDialog::buttonDeleteClicked()
{
    QItemSelectionModel *select = ui->tableIngredients->selectionModel();
    if (select != NULL && select->hasSelection()) {
        QModelIndex index = select->currentIndex();
        recipeModel->removeRow(index.row());
    }
}

void createRecipeDialog::buttonCreateRecipeClicked()
{
    // collect all of the food ids along with serving size
    recipeModel->getFoodIDs(recipe_ids,recipe_serving_sizes);
    recipeName = ui->inputRecipeName->text();
    qDebug() << "Recipe: " << recipeName << Qt::endl;
    qDebug() << "IDs: " << recipe_ids << Qt::endl;
    qDebug() << "Serving size: " << recipe_serving_sizes << Qt::endl;
    accept();

}
