#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <QString>
#include <QDate>

class Transaction {
public:
    QDate date;
    QString category;
    double amount;
    QString description;
    QString type;

    Transaction(QDate date, QString category, double amount, QString description, QString type)
        : date(date), category(category), amount(amount), description(description), type(type) {}

    QDate getDate() const { return date; }
    QString getCategory() const { return category; }
    double getAmount() const { return amount; }
    QString getDescription() const { return description; }
    QString getType() const { return type; }

    bool operator==(const Transaction& other) const {
        return date == other.date && category == other.category &&
               amount == other.amount && description == other.description &&
               type == other.type;
    }
};

#endif // TRANSACTION_H

