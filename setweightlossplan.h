#ifndef SETWEIGHTLOSSPLAN_H
#define SETWEIGHTLOSSPLAN_H

#include <QDialog>

namespace Ui {
class setWeightLossPlan;
}

class setWeightLossPlan : public QDialog
{
    Q_OBJECT

public:
    explicit setWeightLossPlan(QWidget *parent = nullptr);
    ~setWeightLossPlan();

private:
    Ui::setWeightLossPlan *ui;
};

#endif // SETWEIGHTLOSSPLAN_H
