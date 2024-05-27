#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <QString>
#include <QDate>

struct Transaction {
    QDate date;
    QString category;
    double amount;
    // QString description; (maybe we can let user add a short description of the transaction)

    Transaction(QDate d, QString c, double a) : date(d), category(c), amount(a) {}
};

#endif // TRANSACTION_H
