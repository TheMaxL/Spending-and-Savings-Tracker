#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

private:
    Ui::MainWindow *ui;
    QMap<QDate, QMap<QString, double>> dataStorage;
};

#endif // MAINWINDOW_H
