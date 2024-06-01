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

private:
    Ui::editDialog *ui;
    void updateCategory(int index);
};

#endif // EDITDIALOG_H
