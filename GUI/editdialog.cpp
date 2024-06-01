#include "editdialog.h"
#include "ui_editdialog.h"
#include "transaction.h"

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

}

void editDialog::updateCategory(int index)
{
    ui->category->clear();

    QString choice = ui->type->currentText();

    if (choice == "Option 1") {
        ui->category->addItem("Option A");
        ui->category->addItem("Option B");
        ui->category->addItem("Option C");
    } else if (choice == "Option 2") {
        ui->category->addItem("Option X");
        ui->category->addItem("Option Y");
        ui->category->addItem("Option Z");
    }
}
