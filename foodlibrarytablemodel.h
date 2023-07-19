
#ifndef FOODLIBRARYTABLEMODEL_H
#define FOODLIBRARYTABLEMODEL_H

#include <QSqlTableModel>



class foodLibraryTableModel : public QSqlTableModel
{
public:
    explicit foodLibraryTableModel(QObject *parent = nullptr);


    // QAbstractItemModel interface
public:
    virtual QVariant data(const QModelIndex &index, int role) const override;
};

#endif // FOODLIBRARYTABLEMODEL_H
