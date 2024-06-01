#ifndef EDITDIALOG_H
#define EDITDIALOG_H

#include <QDialog>
#include "transaction.h"

namespace Ui {
class editDialog;
}

class editDialog : public QDialog
{
    Q_OBJECT

public:
    explicit editDialog(QWidget *parent = nullptr);
    ~editDialog();
    void setTransaction(Transaction* transaction);

private slots:
    void on_cancel_clicked();

    void on_confirm_clicked();

private:
    Ui::editDialog *ui;
    void updateCategory(int index);
    Transaction* currentTransaction;
};

#endif // EDITDIALOG_H
