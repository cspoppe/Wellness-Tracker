#ifndef WEIGHTLOSSPLANDIALOG_H
#define WEIGHTLOSSPLANDIALOG_H

#include <QDialog>
#include <QDate>

namespace Ui {
class weightLossPlanDialog;
}

class weightLossPlanDialog : public QDialog
{
    Q_OBJECT

public:
    explicit weightLossPlanDialog(QWidget *parent = nullptr);
    ~weightLossPlanDialog();

    QDate getStartDate() const;
    double getTdee() const;
    double getGoalWeight() const;
    double getLossRate() const;

private:
    Ui::weightLossPlanDialog *ui;
    QDate startDate;
    double tdee;
    double goalWeight;
    double lossRate;
private slots:
    void buttonSetClicked();
    void buttonCloseClicked();
};

#endif // WEIGHTLOSSPLANDIALOG_H
