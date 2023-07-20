
#ifndef WEEKLYCALORIESMODEL_H
#define WEEKLYCALORIESMODEL_H

#include <QStandardItemModel>



class weeklyCaloriesModel : public QStandardItemModel
{
public:
    explicit weeklyCaloriesModel(QObject *parent = nullptr);
    void loadData(double tdee, const QVector<double> *dates,const QVector<double> *calories);
    void setTDEE(double TDEE_input) {TDEE = TDEE_input;}

private:
    double TDEE;
    QColor colorUnder = QColor(0,255,0).lighter(170);
    QColor colorOver = QColor(255,0,0).lighter(170);
    // QAbstractItemModel interface
public:
    virtual QVariant data(const QModelIndex &index, int role) const override;
};

#endif // WEEKLYCALORIESMODEL_H
