
#include "foodlibrarytablemodel.h"
#include <QFont>
#include <QColor>

foodLibraryTableModel::foodLibraryTableModel(QObject *parent)
    : QSqlTableModel{parent}
{

}


QVariant foodLibraryTableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::FontRole && isDirty(index))
    {
        QFont font;
        font.setBold(true);
        return font;
    }
    if (role == Qt::ForegroundRole && isDirty(index))
    {
        return QColor(235,12,241);
    }
    return QSqlTableModel::data(index,role);
}
