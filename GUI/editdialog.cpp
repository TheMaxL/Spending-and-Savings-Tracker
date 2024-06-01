#include "editdialog.h"
#include "ui_editdialog.h"
#include "transaction.h"
#include <QMessageBox>

editDialog::editDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::editDialog)
{
    ui->setupUi(this);

    connect(ui->type, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &editDialog::updateCategory);
}


editDialog::~editDialog()
{
    delete ui;
}


void editDialog::setTransaction(Transaction* transaction)
{
    currentTransaction = transaction;
}

void editDialog::updateCategory(int index)
{
    ui->category->clear();

    QString choice = ui->type->currentText().toLower();

    if (choice == "expense") {
        ui->category->addItem("Food");
        ui->category->addItem("Bills");
        ui->category->addItem("Comfort");
        ui->category->addItem("Hygience");
        ui->category->addItem("Medicine");
        ui->category->addItem("Transportation");
        ui->category->addItem("Other");
    } else if (choice == "income") {
        ui->category->addItem("Salary");
        ui->category->addItem("Allowance");
        ui->category->addItem("Bonus");
        ui->category->addItem("Other");
    }
}

void editDialog::on_cancel_clicked()
{
    close();
}


void editDialog::on_confirm_clicked()
{
    QDate date = ui->date->date();
    QString category = ui->category->currentText();
    QString description = ui->description->toPlainText();
    QString type = ui->type->currentText().toLower();

    bool isNumber;
    double amount = ui->amount->toPlainText().toDouble(&isNumber);
    if (!isNumber || amount < 0) {
        QMessageBox::information(this, "Invalid Input", "Please enter a valid positive number.");
        return;
    }

    // Update the values of currentTransaction
    currentTransaction->setDate(date);
    currentTransaction->setCategory(category);
    currentTransaction->setAmount(amount);
    currentTransaction->setDescription(description);
    currentTransaction->setType(type);

    // Close the dialog
    close();
}

