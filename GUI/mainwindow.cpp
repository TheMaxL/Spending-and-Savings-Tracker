#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "transaction.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::onPushButtonClicked);
    connect(ui->dateEdit, &QDateEdit::dateChanged, this, &MainWindow::onDateChanged);
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

    amount = -amount;

    Transaction newTransaction(date, category, amount);
    transactions.append(newTransaction);

    qDebug() << "Added transaction:" << "Date:" << date << "Category:" << category << "Amount:" << amount;
}

void MainWindow::onDateChanged(const QDate &date)
{
    for (const Transaction &transaction : transactions) {
        qDebug() << "Transaction - Date:" << transaction.date << "Category:" << transaction.category << "Amount:" << transaction.amount;
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    QDate date = ui->dateEdit_2->date();
    QString category = ui->comboBox_2->currentText();
    double amount = ui->textEdit_2->toPlainText().toDouble();

    Transaction newTransaction(date, category, amount);
    transactions.append(newTransaction);

    qDebug() << "Added transaction:" << "Date:" << date << "Category:" << category << "Amount:" << amount;
}

