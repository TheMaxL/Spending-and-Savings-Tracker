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

}
