
#ifndef RECIPETABLEMODEL_H
#define RECIPETABLEMODEL_H

#include <QStandardItemModel>



class recipeTableModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit recipeTableModel(QObject *parent = nullptr);

    // QAbstractItemModel interface
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QVector<double> getMacroTotals() {return macroTotals;}
    void getFoodIDs(QVector<int> &ids, QVector<double> &serving_size);
signals:
    void macrosUpdated();

private:
    QVector<double> macroTotals;
private slots:
    void sumMacroTotals();
};

#endif // RECIPETABLEMODEL_H
