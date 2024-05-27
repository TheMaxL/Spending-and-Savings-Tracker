#include "mainwindow.h"
#include "./ui_mainwindow.h"

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
    double number = ui->textEdit->toPlainText().toDouble();

    if (category == "Budget") {
        // Store the number as positive
        dataStorage[date][category] = number;
    } else {
        // Store the number as negative
        dataStorage[date][category] = -number;
    }
}

void MainWindow::onDateChanged(const QDate &date)
{
    // Publish data for the previous date
    QDate previousDate = ui->dateEdit->date().addDays(-1);
    QMap<QString, double> dataForPreviousDate = dataStorage[previousDate];
    // Publish the data (e.g., print to console, send to server, etc.)
    qDebug() << "Data for" << previousDate << ":";
    for (auto it = dataForPreviousDate.begin(); it != dataForPreviousDate.end(); ++it) {
        qDebug() << it.key() << ":" << it.value();
    }
}
