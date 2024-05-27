#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QDate>
#include <QString>
#include <QLabel>
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
    void on_pushButton_2_clicked();


private:
    Ui::MainWindow *ui;
    QList<Transaction> transactions;
    double balance;

    void updateBalance();

};

#endif // MAINWINDOW_H
