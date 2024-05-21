#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
<<<<<<< HEAD
    delete ui; //New Changes
=======
    delete ui;
>>>>>>> 76aba7d364258bdb6f2ad75d63c180c7b9f8af47
}
