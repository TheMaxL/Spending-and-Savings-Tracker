#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <QString>
#include <QDate>

struct Transaction {
    QDate date;
    QString category;
    double amount;
    QString description;
    QString type;

    Transaction(QDate d, QString c, double a, QString ds, QString t) : date(d), category(c), amount(a), description(ds), type(t){}
};

#endif // TRANSACTION_H
