#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <QString>
#include <QDate>

class Transaction {
public:
    Transaction(QDate date, QString category, double amount, QString description, QString type)
        : date(date), category(category), amount(amount), description(description), type(type) {}

    QDate getDate() const { return date; }
    QString getCategory() const { return category; }
    double getAmount() const { return amount; }
    QString getDescription() const { return description; }
    QString getType() const { return type; }

private:
    QDate date;
    QString category;
    double amount;
    QString description;
    QString type;
};

#endif // TRANSACTION_H

