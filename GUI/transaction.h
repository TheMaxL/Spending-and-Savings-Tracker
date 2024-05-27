#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <QString>
#include <QDate>

struct Transaction {
    QDate date;
    QString category;
    double amount;
    QString description;

    Transaction(QDate d, QString c, double a, QString ds) : date(d), category(c), amount(a), description(ds) {}
};

#endif // TRANSACTION_H
