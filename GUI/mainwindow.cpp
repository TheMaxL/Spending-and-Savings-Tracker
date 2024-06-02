#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "editdialog.h"
#include "transaction.h"
#include "linkedlist.h"
#include <QFile>
#include <QTextStream>
#include <QIODevice>
#include <QMessageBox>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QChart>
#include <QDate>
#include <QDebug>
#include <cmath>
#include <QtCharts/QValueAxis>
#include <algorithm>
#include <QBrush>
#include <QColor>
#include <QListWidget>
#include <QListWidgetItem>
#include <QString>
#include <QMenu>
#include <QCursor>
#include <QLabel>

QT_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);
    ui->tabWidget_2->setCurrentIndex(0);

    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::onPushButtonClicked);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::onPushButton_2Clicked);
    connect(ui->monthComboBox_3, QOverload<int>::of(&QComboBox::currentIndexChanged),
    this, &MainWindow::updateDaySpinBoxRange);
    connect(ui->pushButton_7, &QPushButton::clicked, this, &MainWindow::on_pushButton_7_clicked);
    connect(ui->Confirm_Goal, &QPushButton::clicked, this, &MainWindow::on_ConfirmGoal_Clicked);

    ui->resetButton->setStyleSheet("color: red;");
    ui->clearExpenses->setStyleSheet("color: red;");
    ui->clearIncome->setStyleSheet("color: red;");

    year = 2024;
    month = 1;
    monthPie = 1;
    monthDaily = 1, day = 1;

    updateDaySpinBoxRange(0);
    printDaily();


    loadTransactionsFromFile("transactions.txt");
}

MainWindow::~MainWindow()
{
    saveTransactionsToFile("transactions.txt");
    delete ui;
}

class CustomListWidgetItem : public QWidget {
public:
    CustomListWidgetItem(const QString &htmlText, QWidget *parent = nullptr)
        : QWidget(parent) {
        QLabel *label = new QLabel(this);
        label->setTextFormat(Qt::RichText);
        label->setText(htmlText);
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->addWidget(label);
        setLayout(layout);
    }
};

MainWindow::AbnormalityLevel MainWindow::getAbnormalityLevel(double amount, double mean, double stdDev) {
    if (stdDev == 0.0) {
        return NoData; // No statistical data available
    }

    double deviation = std::abs(amount - mean);

    if (deviation <= stdDev) {
        return Normal; // Within 1 standard deviation
    } else if (deviation <= 2 * stdDev) {
        return Irregular; // Beyond 1 standard deviation but less than 2
    } else if (deviation <= 3 * stdDev) {
        return Abnormal; // Beyond 2 standard deviations but less than 3
    } else {
        return ExtremelyAbnormal; // Beyond 3 standard deviations
    }
}


void MainWindow::onPushButtonClicked()
{
    QDate date = ui->dateEdit->date();
    QString category = ui->comboBox->currentText();
    QString description = ui->plainTextEdit->toPlainText();
    QString type = "expense";

    bool isNumber;
    double amount = ui->textEdit->toPlainText().toDouble(&isNumber);
    if (!isNumber || amount < 0) {
        QMessageBox::information(this, "Invalid Input", "Please enter a valid positive number.");
        return;
    }

    Transaction newTransaction(date, category, amount, description, type);
    transactions.append(newTransaction);

    balance -= amount;
    totalExpense += amount;
    updateBalance();
    updateExpense();
    ui->textEdit->clear();
    ui->plainTextEdit->clear();

    double mean = CategoryStats[category].mean;
    double stdDev = CategoryStats[category].standardDeviation;

    AbnormalityLevel level = getAbnormalityLevel(amount, mean, stdDev);

    switch (level) {
    case NoData:
        ui->abnormality->setText("No statistical data available");
        break;
    case Normal:
        ui->abnormality->setText("Normal");
        break;
    case Irregular:
        ui->abnormality->setText("Irregular");
        break;
    case Abnormal:
        ui->abnormality->setText("Abnormal");
        break;
    case ExtremelyAbnormal:
        ui->abnormality->setText("Extremely abnormal");
        break;
    }
}

void MainWindow::onPushButton_2Clicked()
{
    QDate date = ui->dateEdit_2->date();
    QString category = ui->comboBox_2->currentText();
    QString description = ui->plainTextEdit_2->toPlainText();
    QString type = "income";

    bool isNumber;
    double amount = ui->textEdit_2->toPlainText().toDouble(&isNumber);
    if (!isNumber || amount < 0) {
        QMessageBox::information(this, "Invalid Input", "Please enter a valid positive number.");
        return;
    }

    Transaction newTransaction(date, category, amount, description, type);
    transactions.append(newTransaction);

    balance += amount;
    totalIncome += amount;
    updateBalance();
    updateIncome();
    ui->textEdit_2->clear();
    ui->plainTextEdit_2->clear();
}

void MainWindow::updateBalance()
{
    ui->balance->setText("Balance: ₱" + QString::number(balance, 'f', 2));
    ui->balance_2->setText("Balance: ₱" + QString::number(balance, 'f', 2));
}

void MainWindow::updateIncome()
{
    if (totalIncome < 0) {
        totalIncome = 0;
    }
    ui->labelTotalIncome->setText("Total Income: ₱" + QString::number(totalIncome, 'f', 2));
}

void MainWindow::updateExpense()
{
    if (totalExpense < 0) {
        totalExpense = 0;
    }
    ui->labelTotalExpense->setText("Total Expenses: ₱" + QString::number(totalExpense, 'f', 2));
}

void MainWindow::updateBudget()
{
    budget = 0.0;
    ui->budget->setText("Budget: ₱" + QString::number(budget, 'f', 2));
    ui->budget_2->setText("Budget: ₱" + QString::number(budget, 'f', 2));
}

void MainWindow::calculateYearlyIncomeAndExpense(int year)
{
    double yearlyIncome = 0.0, yearlyExpense = 0.0, yearlyBalance = 0.0;
    Node<Transaction>* current = transactions.head;
    while (current) {
        if (current->data.getDate().year() == year) {
            if (current->data.getType() == "income") {
                yearlyIncome += current->data.getAmount();
                yearlyBalance += current->data.getAmount();
            } else if (current->data.getType() == "expense") {
                yearlyExpense += current->data.getAmount();
                yearlyBalance -= current->data.getAmount();
            }
        }
        current = current->next;
    }

    ui->yearlyIncomeLabel->setText("₱" + QString::number(yearlyIncome, 'f', 2));
    ui->yearlyExpenseLabel->setText("₱" + QString::number(yearlyExpense, 'f', 2));
    ui->yearlyBalanceLabel->setText("₱" + QString::number(yearlyBalance, 'f', 2));
    ui->yearlyExpenseLabel->setStyleSheet("color: red;");
    ui->yearlyIncomeLabel->setStyleSheet("color: green;");
    ui->yearlyBalanceLabel->setStyleSheet("color: blue;");
}


void MainWindow::on_spinBoxYear_valueChanged(int arg1)
{
    year = arg1;
    calculateYearlyIncomeAndExpense(arg1);
}

void MainWindow::saveTransactionsToFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Could not create file.");
        return;
    }

    QTextStream out(&file);

    // Save the balance, total income, and total expense first
    out << "Balance," << balance << "\n";
    out << "TotalIncome," << totalIncome << "\n";
    out << "TotalExpense," << totalExpense << "\n";

    // Save the transactions
    Node<Transaction>* current = transactions.head;
    while (current) {
        out << current->data.getDate().toString(Qt::ISODate) << ","
            << current->data.getCategory() << ","
            << current->data.getAmount() << ","
            << current->data.getDescription() << ","
            << current->data.getType() << "\n";
        current = current->next;
    }

    file.close();
}

void MainWindow::loadTransactionsFromFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::information(this, "Create", "File does not exist yet. Creating file.");
        return;
    }

    // Clear existing transactions before loading from the file
    Node<Transaction>* current = transactions.head;
    while (current) {
        Node<Transaction>* next = current->next;
        delete current;
        current = next;
    }
    transactions.head = nullptr;

    QTextStream in(&file);
    balance = 0.0;

    // Load balance, total income, and total expense first
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(",");
        if (fields.size() == 2) {
            if (fields[0] == "Balance") {
                balance = fields[1].toDouble();
            } else if (fields[0] == "TotalIncome") {
                totalIncome = fields[1].toDouble();
            } else if (fields[0] == "TotalExpense") {
                totalExpense = fields[1].toDouble();
            }
        } else {
            // Process transaction data
            QDate date = QDate::fromString(fields[0], Qt::ISODate);
            QString category = fields[1];
            double amount = fields[2].toDouble();
            QString description = fields[3];
            QString type = fields[4];
            Transaction newTransaction(date, category, amount, description, type);
            transactions.append(newTransaction);
        }
    }

    file.close();

    // Update UI elements after loading transactions
    updateBalance();
    updateIncome();
    updateExpense();
}


void MainWindow::on_clearExpenses_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirmation", "Are you sure you want to clear your expenses data?",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        // User confirmed to clear expenses data
        totalExpense = 0.0;
        // Update UI after clearing expenses
        updateExpense();
        QMessageBox::information(this, "Data Cleared", "Expense data cleared successfully.");
    } else {
        // User chose not to clear expenses data
        QMessageBox::information(this, "Operation Cancelled", "Expenses data was not cleared.");
    }
}


void MainWindow::on_clearIncome_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirmation", "Are you sure you want to clear your income data?",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        // User confirmed to clear expenses data
        totalIncome = 0.0;
        updateIncome();
        QMessageBox::information(this, "Data Cleared", "Income data cleared successfully.");
    } else {
        QMessageBox::information(this, "Operation Cancelled", "Income data was not cleared.");
    }
}


void MainWindow::on_resetButton_clicked()
{
    QMessageBox::StandardButton clear;
    clear = QMessageBox::question(this, "Confirmation", "This action resets ALL your data. Continue?",
                                  QMessageBox::Yes | QMessageBox::No);
    if (clear == QMessageBox::Yes) {
        resetData("transactions.txt");
        dailyBalances.clear();
        updateAverageBalance();
        ui->label_14->setText("Daily Balance: 0.00");
    }
    ui->label_19->setText("Mean: 0.00");
    ui->label_20->setText("Variance: 0.00");
    ui->label_21->setText("Standard Deviation: 0.00");

    ui->graphicsView->setScene(nullptr);
}

void MainWindow::resetData(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        QMessageBox::critical(this, "Error", "Could not open file for writing!");
        return;
    }
    file.resize(0); // Truncate the file
    file.close();

    // Reset the in-memory data
    balance = 0.0;
    totalIncome = 0.0;
    totalExpense = 0.0;
    transactions.clear(); // Clear the in-memory transactions list

    // Update the UI to reflect the reset state
    updateBalance();
    updateIncome();
    updateExpense();
    //calculateYearlyIncomeAndExpense(ui->spinBoxYear->value()); // Update yearly calculations

    // Save the reset values to the file
    updateAverageBalance();
    ui->label_14->setText("Daily Balance: 0.00");
    saveTransactionsToFile(filename);
}

void MainWindow::on_dateEdit_dateChanged(const QDate &date)
{
    double dailyIncome = 0.0;
    double dailyExpense = 0.0;

    // Iterate through your transactions to calculate the daily income and expense
    Node<Transaction>* current = transactions.head;
    while (current) {
        if (current->data.getDate() == date) {
            if (current->data.getType() == "income") {
                dailyIncome += current->data.getAmount();
            } else if (current->data.getType() == "expense") {
                dailyExpense += current->data.getAmount();
            }
        }
        current = current->next;
    }

    double dailyBalance = dailyIncome - dailyExpense;

    // Store the daily balance in the QMap
    dailyBalances[date] = dailyBalance;

    // Update the label with the daily balance
    ui->label_14->setText("Daily Balance: " + QString::number(dailyBalance, 'f', 2));

    updateAverageBalance();
}

void MainWindow::updateAverageBalance()
{
    double totalBalance = 0.0;
    int days = 0;

    // Iterate through the daily balances
    for (auto it = dailyBalances.begin(); it != dailyBalances.end(); ++it) {
        totalBalance += it.value(); // Access the value using it.value()
        days++;
    }

    // Calculate the average balance
    double averageBalance = (days > 0) ? totalBalance / days : 0.0;

    // Update the label with the average balance
    ui->label_15->setText("Average Balance: " + QString::number(averageBalance, 'f', 2));
}


void MainWindow::on_monthComboBox_currentIndexChanged(int index)
{
    monthPie = index + 1;
    createPieChartIncome();
    createPieChartExpense();
}


void MainWindow::createPieChartIncome()
{
    Node<Transaction>* current = transactions.head;
    double salary = 0.0, allowance = 0.0, bonus = 0.0, other = 0.0, total = 0.0;

    while (current)
    {
        QDate transactionDate = current->data.getDate();
        if (transactionDate.year() == year && transactionDate.month() == monthPie && current->data.getType() == "income")
        {
            total += current->data.getAmount();
            if (current->data.getCategory() == "Salary")
            {
                salary += current->data.getAmount();
            }
            else if (current->data.getCategory() == "Allowance")
            {
                allowance += current->data.getAmount();
            }
            else if (current->data.getCategory() == "Bonus")
            {
                bonus += current->data.getAmount();
            }
            else if (current->data.getCategory() == "Other")
            {
                other += current->data.getAmount();
            }
        }
        current = current->next; // Move to the next node
    }

    QPieSeries *series = new QPieSeries();

    // Add dummy data points if total is 0
    if (total <= 0.0)
    {
        series->append("Salary", 1);
        series->append("Allowance", 1);
        series->append("Bonus", 1);
        series->append("Other", 1);
    }
    else
    {
        // Add actual data points
        series->append("Salary", salary / total);
        series->append("Allowance", allowance / total);
        series->append("Bonus", bonus / total);
        series->append("Other", other / total);
    }

    // Custom colors for the slices (different shades of green)
    QStringList colors = {"#00FF00", "#00CC00", "#009900", "#006600"};
    int colorIndex = 0;

    // Set colors for each slice
    for (QPieSlice *slice : series->slices())
    {
        slice->setColor(QColor(colors[colorIndex]));
        colorIndex = (colorIndex + 1) % colors.size();
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Income");
    chart->legend()->hide();

    // Set label format to show percentages
    chart->setTheme(QChart::ChartThemeLight);

    // Set labels visible and format
    // Iterate through the slices in the series
    for (QPieSlice *slice : series->slices()) {
        // Always set the label visible for each slice
        slice->setLabelVisible(true);

        // Check if the total is greater than 0 to avoid division by zero
        if (total > 0.0) {
            if (slice->percentage() > 0.0) {
                slice->setLabelVisible(true);
                QString percentage = QString::number(slice->percentage() * 100, 'f', 2);
                slice->setLabel(QString("%1 (%2%)").arg(slice->label()).arg(percentage));
            } else {
                slice->setLabelVisible(false);
            }
        } else {
            // Handle the case when total is 0 (to avoid division by zero)
            slice->setLabel(QString("%1 (%2%)").arg(slice->label()).arg(0.0, 0, 'f', 2));
        }
    }

    QChartView *chartview = new QChartView(chart);
    chartview->setRenderHint(QPainter::Antialiasing);

    // Clear any existing layout or widgets in PieChartIncome
    if (ui->PieChartIncome->layout())
    {
        QLayoutItem *item;
        while ((item = ui->PieChartIncome->layout()->takeAt(0)) != nullptr)
        {
            delete item->widget(); // delete the widget
            delete item; // delete the layout item
        }
        delete ui->PieChartIncome->layout(); // delete the layout
    }

    // Set the new layout and add the chart view
    QVBoxLayout *layout = new QVBoxLayout(ui->PieChartIncome);
    layout->addWidget(chartview);
    ui->PieChartIncome->setLayout(layout);
}

void MainWindow::createPieChartExpense()
{
    Node<Transaction>* current = transactions.head;
    double food = 0.0, bills = 0.0, comfort = 0.0, hygiene = 0.0, medicine = 0.0, transportation = 0.0, other = 0.0, total = 0.0;

    while (current)
    {
        QDate transactionDate = current->data.getDate();
        if (transactionDate.year() == year && transactionDate.month() == monthPie && current->data.getType() == "expense")
        {
            total += current->data.getAmount();
            if (current->data.getCategory() == "Food")
            {
                food += current->data.getAmount();
            }
            else if (current->data.getCategory() == "Bills")
            {
                bills += current->data.getAmount();
            }
            else if (current->data.getCategory() == "Comfort")
            {
                comfort += current->data.getAmount();
            }
            else if (current->data.getCategory() == "Hygiene")
            {
                hygiene += current->data.getAmount();
            }
            else if (current->data.getCategory() == "Medicine")
            {
                medicine += current->data.getAmount();
            }
            else if (current->data.getCategory() == "Transportation")
            {
                transportation += current->data.getAmount();
            }
            else if (current->data.getCategory() == "Other")
            {
                other += current->data.getAmount();
            }
        }
        current = current->next;
    }

    QPieSeries *series = new QPieSeries();

    // Add dummy data points if total is 0
    if (total <= 0.0)
    {
        series->append("Food", 1);
        series->append("Bills", 1);
        series->append("Comfort", 1);
        series->append("Hygiene", 1);
        series->append("Medicine", 1);
        series->append("Transportation", 1);
        series->append("Other", 1);
    }
    else
    {
        series->append("Food", food / total);
        series->append("Bills", bills / total);
        series->append("Comfort", comfort / total);
        series->append("Hygiene", hygiene / total);
        series->append("Medicine", medicine / total);
        series->append("Transportation", transportation / total);
        series->append("Other", other / total);
    }

    // Custom colors for the slices (different shades of red)
    QStringList colors = {"#8B0000", "#DC143C", "#B22222", "#CD5C5C", "#F08080", "#FF0000", "#660000"};
    int colorIndex = 0;

    // Set colors for each slice
    for (QPieSlice *slice : series->slices())
    {
        slice->setColor(QColor(colors[colorIndex]));
        colorIndex = (colorIndex + 1) % colors.size();
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Expense");
    chart->legend()->hide();

    // Set label format to show percentages
    chart->setTheme(QChart::ChartThemeLight);

    // Set labels visible and format
    // Iterate through the slices in the series
    for (QPieSlice *slice : series->slices()) {
        // Always set the label visible for each slice
        slice->setLabelVisible(true);

        // Check if the total is greater than 0 to avoid division by zero
        if (total > 0.0) {
            if (slice->percentage() > 0.0) {
                slice->setLabelVisible(true);
                QString percentage = QString::number(slice->percentage() * 100, 'f', 2);
                slice->setLabel(QString("%1 (%2%)").arg(slice->label()).arg(percentage));
            } else {
                slice->setLabelVisible(false);
            }
        } else {
            // Handle the case when total is 0 (to avoid division by zero)
            slice->setLabel(QString("%1 (%2%)").arg(slice->label()).arg(0.0, 0, 'f', 2));
        }
    }

    QChartView *chartview = new QChartView(chart);
    chartview->setRenderHint(QPainter::Antialiasing);

    // Clear any existing layout or widgets in PieChartExpense
    if (ui->PieChartExpense->layout())
    {
        QLayoutItem *item;
        while ((item = ui->PieChartExpense->layout()->takeAt(0)) != nullptr)
        {
            delete item->widget(); // delete the widget
            delete item; // delete the layout item
        }
        delete ui->PieChartExpense->layout(); // delete the layout
    }

    // Set the new layout and add the chart view
    QVBoxLayout *layout = new QVBoxLayout(ui->PieChartExpense);
    layout->addWidget(chartview);
    ui->PieChartExpense->setLayout(layout);
}

void MainWindow::on_monthComboBox_2_currentIndexChanged(int index)
{
    month = index + 1;
    printMonthly();
    updateMonthly();
}

void MainWindow::printMonthly()
{
    ui->monthlyList->clear();  // Clear the list widget before adding new items

    Node<Transaction>* current = transactions.head;
    while (current) {
        QDate transactionDate = current->data.getDate();
        if (transactionDate.month() == month && transactionDate.year() == year) {
            QString amountText = QString("₱%1").arg(current->data.getAmount());
            QString color = (current->data.getType() == "expense") ? "red" : (current->data.getType() == "income") ? "green" : "black";
            QString transactionInfo = QString("%1 - %2: <span style='color:%3;'>%4</span>")
                                          .arg(transactionDate.toString("yyyy-MM-dd"))
                                          .arg(current->data.getCategory())
                                          .arg(color)
                                          .arg(amountText);

            if (!current->data.getDescription().isEmpty()) {
                transactionInfo.append("    (" + current->data.getDescription() + ")");
            }

            QLabel *label = new QLabel();
            label->setTextFormat(Qt::RichText);
            label->setText(transactionInfo);

            QListWidgetItem *item = new QListWidgetItem();
            ui->monthlyList->addItem(item);
            ui->monthlyList->setItemWidget(item, label);
        }
        current = current->next;
    }

    // Apply the stylesheet for the QListWidget
    ui->dailyList->setStyleSheet("QListWidget::item:selected { background: transparent; }");
}

void MainWindow::updateMonthly()
{
    double monthlyExpense = 0.0, monthlyIncome = 0.0, monthlyBalance = 0.0;
    Node<Transaction>* current = transactions.head;
    while (current) {
        QDate currentDate = current->data.getDate();
        if (currentDate.year() == year && currentDate.month() == month) {
            if (current->data.getType() == "income") {
                monthlyIncome += current->data.getAmount();
                monthlyBalance += current->data.getAmount();
            } else if (current->data.getType() == "expense") {
                monthlyExpense += current->data.getAmount();
                monthlyBalance -= current->data.getAmount();
            }
        }
        current = current->next;
    }
    ui->monthlyIncomeLabel->setText("₱" + QString::number(monthlyIncome, 'f', 2));
    ui->monthlyExpenseLabel->setText("₱" + QString::number(monthlyExpense, 'f', 2));
    ui->monthlyBalanceLabel->setText("₱" + QString::number(monthlyBalance, 'f', 2));
    ui->monthlyExpenseLabel->setStyleSheet("color: red;");
    ui->monthlyIncomeLabel->setStyleSheet("color: green;");
    ui->monthlyBalanceLabel->setStyleSheet("color: blue;");
}

void MainWindow::updateDaySpinBoxRange(int index)
{
    // Get the selected month from the index and calculate the number of days
    int selectedMonth = index + 1;  // Assuming monthComboBox_3 index starts from 0
    int daysInMonth = QDate(year, selectedMonth, 1).daysInMonth();

    // Set the range of the day spin box based on the number of days
    ui->spinBoxDay->setRange(1, daysInMonth);
}



void MainWindow::printDaily()
{
    ui->dailyList->clear();  // Clear the list widget before adding new items

    Node<Transaction>* current = transactions.head;
    while (current) {
        QDate transactionDate = current->data.getDate();
        if (transactionDate.month() == monthDaily && transactionDate.day() == day && transactionDate.year() == year) {
            QString amountText = QString("₱%1").arg(current->data.getAmount());
            QString color = (current->data.getType() == "expense") ? "red" : (current->data.getType() == "income") ? "green" : "black";
            QString transactionInfo = QString("%1 - %2: <span style='color:%3;'>%4</span>")
                                          .arg(transactionDate.toString("yyyy-MM-dd"))
                                          .arg(current->data.getCategory())
                                          .arg(color)
                                          .arg(amountText);

            if (!current->data.getDescription().isEmpty()) {
                transactionInfo.append("    (" + current->data.getDescription() + ")");
            }

            QLabel *label = new QLabel();
            label->setTextFormat(Qt::RichText);
            label->setText(transactionInfo);

            QListWidgetItem *item = new QListWidgetItem();
            ui->dailyList->addItem(item);
            ui->dailyList->setItemWidget(item, label);
        }
        current = current->next;
    }

    // Apply the stylesheet for the QListWidget
    ui->dailyList->setStyleSheet("QListWidget::item:selected { background: transparent; }");
}


// Function to calculate mean
double calculateMean(const QList<double>& data) {
    double sum = 0.0;
    for(double value : data) {
        sum += value;
    }
    return (data.size() > 0) ? sum / data.size() : 0.0;
}

// Function to calculate variance
double calculateVariance(const QList<double>& data, double mean) {
    double sum = 0.0;
    for(double value : data) {
        sum += (value - mean) * (value - mean);
    }
    return (data.size() > 0) ? sum / data.size() : 0.0;
}

// Function to calculate standard deviation
double calculateStandardDeviation(double variance) {
    return std::sqrt(variance);
}
double normalDistribution(double x, double mean, double stdDev) {
    double exponent = -pow((x - mean) / stdDev, 2) / 2;
    double coefficient = 1 / (stdDev * sqrt(2 * M_PI));
    return coefficient * exp(exponent);
}

void MainWindow::on_pushButton_7_clicked() {
    QDate startDate = ui->dateEdit_7->date();
    QDate endDate = ui->dateEdit_8->date();
    QString category = ui->comboBox_7->currentText();

    if (!startDate.isValid() || !endDate.isValid() || startDate > endDate) {
        QMessageBox::warning(this, "Invalid Date Range", "Please select a valid date range.");
        return;
    }

    QList<double> balances;
    Node<Transaction>* current = transactions.head;
    while (current) {
        QDate transactionDate = current->data.getDate();
        if (transactionDate >= startDate && transactionDate <= endDate && current->data.getCategory() == category) {
            balances.append(current->data.getAmount());
        }
        current = current->next;
    }

    if (balances.isEmpty()) {
        QMessageBox::information(this, "No Data", "No transactions found in the selected date range for the chosen category.");
        return;
    }

    double mean = calculateMean(balances);
    double variance = calculateVariance(balances, mean);
    double stdDev = calculateStandardDeviation(variance);

    struct CategoryStats stats;
    stats.mean = mean;
    stats.variance = variance;
    stats.standardDeviation = stdDev;

    CategoryStats[category] = stats;

    ui->label_19->setText("Mean Expense: ₱" + QString::number(mean, 'f', 2));
    ui->label_20->setText("Variance: ₱" + QString::number(variance, 'f', 2));
    ui->label_21->setText("Standard Deviation: ₱" + QString::number(stdDev, 'f', 2));

    // Create a normal distribution graph
    QChart *chart = new QChart();
    chart->setTitle("Normal Distribution Graph");

    // Create a line series for the normal distribution curve
    QLineSeries *series = new QLineSeries();
    series->setName("Normal Distribution");

    // Generate x-values for the normal distribution curve
    int numPoints = 1000; // Increase the number of points for a smoother curve
    double xMin = mean - 4 * stdDev; // Extend the range to -4 and +4 standard deviations
    double xMax = mean + 4 * stdDev;
    double xStep = (xMax - xMin) / (numPoints - 1);

    for (int i = 0; i < numPoints; ++i) {
        double x = xMin + i * xStep;
        double y = normalDistribution(x, mean, stdDev);
        series->append(x, y);
    }

    // Add the series to the chart
    chart->addSeries(series);

    // Create axes
    QValueAxis *axisX = new QValueAxis();
    axisX->setRange(xMin, xMax);
    axisX->setTitleText("Amount");

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, normalDistribution(mean, mean, stdDev)); // Use the peak of the bell curve
    axisY->setTitleText("Probability Density");

    // Add axes to the chart
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisX);
    series->attachAxis(axisY);

    // Create a QChartView
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // Create a scene and add the chart view to it
    QGraphicsScene *scene = new QGraphicsScene();
    QGraphicsProxyWidget *proxy = scene->addWidget(chartView);

    // Set the scene to the graphics view
    ui->graphicsView->setScene(scene);

    // Resize the chartView to match the size of graphicsView
    chartView->resize(ui->graphicsView->size());
    ui->graphicsView->fitInView(proxy->boundingRect(), Qt::KeepAspectRatio);
}

void MainWindow::updateDaily()
{
    double dailyExpense = 0.0, dailyIncome = 0.0, dailyBalance = 0.0;
    Node<Transaction>* current = transactions.head;
    while (current) {
        QDate currentDate = current->data.getDate();
        if (currentDate.year() == year && currentDate.month() == monthDaily && currentDate.day() == day) {
            if (current->data.getType() == "income") {
                dailyIncome += current->data.getAmount();
                dailyBalance += current->data.getAmount();
            } else if (current->data.getType() == "expense") {
                dailyExpense += current->data.getAmount();
                dailyBalance -= current->data.getAmount();
            }
        }
        current = current->next;
    }
    ui->dailyIncomeLabel->setText("₱" + QString::number(dailyIncome, 'f', 2));
    ui->dailyExpenseLabel->setText("₱" + QString::number(dailyExpense, 'f', 2));
    ui->dailyBalanceLabel->setText("₱" + QString::number(dailyBalance, 'f', 2));
    ui->dailyExpenseLabel->setStyleSheet("color: red;");
    ui->dailyIncomeLabel->setStyleSheet("color: green;");
    ui->dailyBalanceLabel->setStyleSheet("color: blue;");
}
void MainWindow::on_monthComboBox_3_currentIndexChanged(int index)
{
    monthDaily = index + 1;
    updateDaily();
    printDaily();
}
void MainWindow::on_spinBoxDay_valueChanged(int arg1)
{
    day = arg1;
    updateDaily();
    printDaily();
}
void MainWindow::on_tabWidget_currentChanged(int index)
{
    updateBalance();
    updateExpense();
    updateIncome();
    calculateYearlyIncomeAndExpense(year);
    updateMonthly();
    printMonthly();
    updateDaily();
    printDaily();
    createPieChartExpense();
    createPieChartIncome();
}
void MainWindow::on_tabWidget_2_currentChanged(int index)
{
    updateDaySpinBoxRange(0);
    printDaily();
    updateDaily();
    printMonthly();
    updateMonthly();
    createPieChartExpense();
    createPieChartIncome();
}

void MainWindow::editItem(QListWidgetItem *item, int x)
{
    int row;
    if (x == 1)
    {
        row = ui->monthlyList->row(item);
    } else if (x == 2)
    {
        row = ui->dailyList->row(item);
    }
    Transaction* transaction = transactions.at(row);  // Retrieve the transaction pointer
    if (transaction) {
        if (transaction->getType() == "expense")
        {
            balance += transaction->getAmount();
            totalExpense -= transaction->getAmount();
        } else if (transaction->getType() == "income")
        {
            balance -= transaction->getAmount();
            totalIncome -= transaction->getAmount();
        }
        editDialog edit;
        edit.setModal(this);
        edit.setTransaction(transaction);
        edit.exec();

        if (transaction->getType() == "expense")
        {
            balance -= transaction->getAmount();
            totalExpense += transaction->getAmount();
        } else if (transaction->getType() == "income")
        {
            balance += transaction->getAmount();
            totalIncome += transaction->getAmount();
        }
    }
    else {
        QMessageBox::information(this, "Error", "Transaction not found in the list.");
    }
}

void MainWindow::deleteItem(QListWidgetItem *item, int x)
{
    int row;
    if (x == 1)
    {
        row = ui->monthlyList->row(item);
    } else if (x == 2)
    {
        row = ui->dailyList->row(item);
    }
    Transaction* transaction = transactions.at(row);  // Retrieve the transaction pointer
    if (transaction) {
        QString type = transaction->getType();
        double amount = transaction->getAmount();
        if (transactions.deleteNode(*transaction)) {
            if (type == "expense") {
                balance += amount; // Add back the expense amount
                totalExpense -= amount; // Update total expense
            } else if (type == "income") {
                balance -= amount; // Deduct the income amount
                totalIncome -= amount; // Update total income
            }
        } else {
            QMessageBox::information(this, "Error", "Transaction not found in the list.");
        }
    } else {
        QMessageBox::information(this, "Error", "Invalid transaction index.");
    }
}

void MainWindow::on_ConfirmGoal_Clicked()
{
    // Retrieve the savings goal from the textEdit_3
    bool ok;
    double goal = ui->textEdit_3->toPlainText().toDouble(&ok);
    if (!ok || goal <= 0) {
        QMessageBox::warning(this, "Invalid Input", "Please enter a valid positive number.");
        return;
    }

    // Retrieve the average daily balance from the label_15
    double averageDailyBalance = ui->label_15->text().split(" ").last().toDouble(&ok);
    if (!ok || averageDailyBalance <= 0) {
        QMessageBox::warning(this, "Invalid Average Daily Balance", "Average daily balance must be a positive number.");
        return;
    }

    // Calculate the number of days needed to reach the savings goal
    double daysNeeded = goal / averageDailyBalance;

    // Display the result in the Savingdays label
    ui->Savingdays->setText(QString::number(ceil(daysNeeded)));
}


void MainWindow::on_monthlyList_itemDoubleClicked(QListWidgetItem *item)
{
    editOrDelete(item, 1);
    printMonthly();
    updateMonthly();
    calculateYearlyIncomeAndExpense(year);
}

void MainWindow::on_dailyList_itemDoubleClicked(QListWidgetItem *item)
{
    editOrDelete(item, 2);
    printDaily();
    updateDaily();
    calculateYearlyIncomeAndExpense(year);
}

void MainWindow::editOrDelete(QListWidgetItem *item, int x)
{
    QPoint pos;
    // Get the position of the mouse cursor relative to the list widget
    if (x == 1)
    {
        pos = ui->monthlyList->mapFromGlobal(QCursor::pos());
    } else if (x == 2)
    {
        pos = ui->dailyList->mapFromGlobal(QCursor::pos());
    }

    // Create a context menu
    QMenu contextMenu(tr("Context Menu"), this);

    // Apply stylesheet to the QMenu to add hover effect
    contextMenu.setStyleSheet("QMenu::item { color: black; background-color: transparent; }"
                              "QMenu::item:selected { background-color: lightgray; color: black; }");

    // Add actions to the context menu
    QAction editAction("Edit", this);
    QAction deleteAction("Delete", this);

    // Connect actions to slots
    connect(&editAction, &QAction::triggered, [=](){ editItem(item, x); });
    connect(&deleteAction, &QAction::triggered, [=](){ deleteItem(item, x); });

    // Add actions to the context menu
    contextMenu.addAction(&editAction);
    contextMenu.addAction(&deleteAction);

    // Show the context menu at the mouse position
    if (x == 1) {
        contextMenu.exec(ui->monthlyList->mapToGlobal(pos));
    } else if (x == 2)
    {
        contextMenu.exec(ui->dailyList->mapToGlobal(pos));
    }
}

