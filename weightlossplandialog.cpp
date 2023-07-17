#include "weightlossplandialog.h"
#include "ui_weightlossplandialog.h"

#include <QMessageBox>

weightLossPlanDialog::weightLossPlanDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::weightLossPlanDialog)
{
    ui->setupUi(this);
    ui->dateEdit->setDate(QDate::currentDate());
    connect(ui->buttonSet,&QPushButton::clicked,this,&weightLossPlanDialog::buttonSetClicked);
    connect(ui->buttonClose,&QPushButton::clicked,this,&weightLossPlanDialog::buttonCloseClicked);
}

weightLossPlanDialog::~weightLossPlanDialog()
{
    delete ui;
}

QDate weightLossPlanDialog::getStartDate() const
{
    return startDate;
}

double weightLossPlanDialog::getTdee() const
{
    return tdee;
}

double weightLossPlanDialog::getGoalWeight() const
{
    return goalWeight;
}

double weightLossPlanDialog::getLossRate() const
{
    return lossRate;
}

void weightLossPlanDialog::buttonSetClicked()
{
    // check if any fields are empty

    QString tdeeStr = ui->inputTDEE->text();
    QString goalWeightStr = ui->inputGoalWeight->text();
    QString lossRateStr = ui->inputLossRate->text();
    if (tdeeStr.isEmpty() || goalWeightStr.isEmpty() || lossRateStr.isEmpty())
    {
        QMessageBox::critical(
            this,
            tr("Error!"),
            tr("All fields must be filled to set a new weight loss plan.") );
    } else {
        // grab info from fields
        startDate = ui->dateEdit->date();
        tdee = tdeeStr.toDouble();
        goalWeight = goalWeightStr.toDouble();
        lossRate = lossRateStr.toDouble();
        accept();
    }
}

void weightLossPlanDialog::buttonCloseClicked()
{
    reject();
}
