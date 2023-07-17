#ifndef CREATERECIPEDIALOG_H
#define CREATERECIPEDIALOG_H

#include <QDialog>
#include <QStandardItemModel>
#include <QSqlQueryModel>


#include "foodsearchproxymodel.h"
#include "recipetablemodel.h"

namespace Ui {
class createRecipeDialog;
}

class createRecipeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit createRecipeDialog(QWidget *parent = nullptr);
    ~createRecipeDialog();

    QVector<int> getRecipeIds() const {return recipe_ids;}
    QVector<double> getRecipeServingSizes() const {return recipe_serving_sizes;}
    QString getRecipeName() const {return recipeName;}

private:
    Ui::createRecipeDialog *ui;
    recipeTableModel* recipeModel;
    QSqlQueryModel *queryModel;
    foodSearchProxyModel *foodSearchProxy;
    foodSearchProxyModel *recipeProxy;
    void updateRecipeMacros();
    QVector<int> recipe_ids;
    QVector<double> recipe_serving_sizes;
    QString recipeName;
private slots:
    void buttonSearchClicked();
    void buttonAddClicked();
    void buttonCancelClicked();
    void buttonDeleteClicked();
    void buttonCreateRecipeClicked();
};

#endif // CREATERECIPEDIALOG_H
