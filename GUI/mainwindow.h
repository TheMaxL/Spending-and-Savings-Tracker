#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QDate>
#include <QString>
#include "transaction.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onPushButtonClicked();
    void onDateChanged(const QDate &date);
    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    QMap<QDate, QMap<QString, double>> dataStorage;
    QList<Transaction> transactions;
};

#endif // MAINWINDOW_H
