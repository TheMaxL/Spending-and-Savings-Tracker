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

    void setDate(const QDate& newDate) {
        date = newDate;
    }

    void setCategory(const QString& newCategory) {
        category = newCategory;
    }

    void setAmount(double newAmount) {
        amount = newAmount;
    }

    void setDescription(const QString& newDescription) {
        description = newDescription;
    }

    void setType(const QString& newType) {
        type = newType;
    }

    bool operator==(const Transaction& other) const {
        return date == other.date && category == other.category &&
               amount == other.amount && description == other.description &&
               type == other.type;
    }
};

#endif // TRANSACTION_H

