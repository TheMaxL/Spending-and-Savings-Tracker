#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDate>
#include <QString>
#include <QLabel>
#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <QtCharts>
#include "transaction.h"
#include "linkedlist.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void saveTransactionsToFile(const QString& filename);
    void loadTransactionsFromFile(const QString& filename);
    void resetData(const QString& filename);

private slots:
    void onPushButtonClicked();
    void onPushButton_2Clicked();
    void on_spinBoxYear_valueChanged(int arg1);
    void on_clearExpenses_clicked();
    void on_clearIncome_clicked();
    void on_resetButton_clicked();
    void on_monthComboBox_currentIndexChanged(int index);
    void on_dateEdit_dateChanged(const QDate &date);
    void updateAverageBalance();
    void updateDaySpinBoxRange(int index);
    void on_monthComboBox_2_currentIndexChanged(int index);
    void on_monthComboBox_3_currentIndexChanged(int index);
    void on_spinBoxDay_valueChanged(int arg1);
    void on_tabWidget_2_currentChanged(int index);
    void on_tabWidget_currentChanged(int index);
    void on_pushButton_7_clicked();

private:
    Ui::MainWindow *ui;
    LinkedList<Transaction> transactions; // Use linked list instead of QList
    double balance, totalIncome, totalExpense, amount, budget;
    int year, month, monthPie, monthDaily, day;
    double averageBalance;
    QMap<QDate, double> dailyBalances;

    void updateBalance();
    void updateIncome();
    void updateExpense();
    void updateBudget();
    void calculateYearlyIncomeAndExpense(int year);
    void createPieChartIncome();
    void createPieChartExpense();
    void updateMonthly();
    void printMonthly();
    void updateDaily();
    void printDaily();
};

#endif // MAINWINDOW_H


