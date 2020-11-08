#include "mainwindow.h"
#include "ui_mainwindow.h"

/*QString promeni(int i)
{
    QString str = ui->textEdit->toPlainText();
    int x = str.toInt() + i;
    str = QString(x);
    return str;
}*/

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    /*connect(ui->pushButton_2,
            SIGNAL(clicked()),
            ui->textEdit,
            SLOT(setPlainText(QString(ui->textEdit->toPlainText().toInt() + 1)))
            );
    connect(ui->pushButton_3,
            SIGNAL(clicked()),
            ui->textEdit,
            SLOT(setPlainText(QString(ui->textEdit->toPlainText().toInt() - 1)))
            );*/
}

MainWindow::~MainWindow()
{
    delete ui;
}

