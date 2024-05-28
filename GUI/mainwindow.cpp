#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "transaction.h"
#include "linkedlist.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    balance = 0.0;
    updateBalance();

    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::onPushButtonClicked);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::onPushButton_2Clicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onPushButtonClicked()
{
    QDate date = ui->dateEdit->date();
    QString category = ui->comboBox->currentText();
    double amount = ui->textEdit->toPlainText().toDouble();
    QString description = ui->plainTextEdit->toPlainText();
    QString type = "expense";

    Transaction newTransaction(date, category, amount, description, type);
    transactions.append(newTransaction);

    balance -= amount;
    updateBalance();
    updateExpense(); // Update total expenses
    ui->textEdit->clear();
    ui->plainTextEdit->clear();
}

void MainWindow::onPushButton_2Clicked()
{
    QDate date = ui->dateEdit_2->date();
    QString category = ui->comboBox_2->currentText();
    double amount = ui->textEdit_2->toPlainText().toDouble();
    QString description = ui->plainTextEdit_2->toPlainText();
    QString type = "income";

    Transaction newTransaction(date, category, amount, description, type);
    transactions.append(newTransaction);

    balance += amount;
    updateBalance();
    updateIncome(); // Update total income
    ui->textEdit_2->clear();
    ui->plainTextEdit_2->clear();
}

void MainWindow::updateBalance()
{
    ui->label->setText("Balance: ₱" + QString::number(balance, 'f', 2));
    ui->label_3->setText("Balance: ₱" + QString::number(balance, 'f', 2));
    ui->label_10->setText("₱" + QString::number(balance, 'f', 2));
}

void MainWindow::updateIncome()
{
    double totalIncome = 0.0;

    // Iterate through all transactions and sum up the income
    Node<Transaction>* current = transactions.head;
    while (current) {
        if (current->data.getType() == "income") {
            totalIncome += current->data.getAmount();
        }
        current = current->next;
    }

    // Update the label (or any other UI element) to display total income
    ui->labelTotalIncome->setText("Total Income: ₱" + QString::number(totalIncome, 'f', 2));
}

void MainWindow::updateExpense()
{
    double totalExpense = 0.0;

    // Iterate through all transactions and sum up the expenses
    Node<Transaction>* current = transactions.head;
    while (current) {
        if (current->data.getType() == "expense") {
            totalExpense += current->data.getAmount();
        }
        current = current->next;
    }

    // Update the label (or any other UI element) to display total expenses
    ui->labelTotalExpense->setText("Total Expenses: ₱" + QString::number(totalExpense, 'f', 2));
}

