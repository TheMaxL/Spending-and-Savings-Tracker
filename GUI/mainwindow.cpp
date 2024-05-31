#include "mainwindow.h"
#include "./ui_mainwindow.h"
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

QT_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);


    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::onPushButtonClicked);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::onPushButton_2Clicked);
    ui->resetButton->setStyleSheet("color: red;");
    ui->yearlyExpenseLabel->setStyleSheet("color: red;");
    ui->yearlyIncomeLabel->setStyleSheet("color: green;");
    ui->yearlyBalanceLabel->setStyleSheet("color: blue;");
    ui->clearExpenses->setStyleSheet("color: red;");
    ui->clearIncome->setStyleSheet("color: red;");
    year = 2024;

    loadTransactionsFromFile("transactions.txt");
}

MainWindow::~MainWindow()
{
    saveTransactionsToFile("transactions.txt");
    delete ui;
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
    ui->labelTotalIncome->setText("Total Income: ₱" + QString::number(totalIncome, 'f', 2));
}

void MainWindow::updateExpense()
{
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
}


void MainWindow::on_tabWidget_tabBarClicked(int index)
{
    calculateYearlyIncomeAndExpense(year);
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
        // You may want to clear transactions as well if needed: transactions.clear();
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

    // Debug information
    qDebug() << "updateAverageBalance called. Total Balance:" << totalBalance << "Days:" << days << "Average Balance:" << averageBalance;
}



void MainWindow::on_monthComboBox_currentIndexChanged(int index)
{
    createPieChart();
}


void MainWindow::createPieChart()
{
    // will add graph of expenses and income
}



