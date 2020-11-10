#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <curl/curl.h>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    Ui::MainWindow *ui;

public slots:/*
     std::string loginWolk(CURL *c);
     void  getDevice(CURL*, std::string);
     static size_t WriteCallback(void*, size_t, size_t, void *);
     int locateValue(std::string, std::string, int);
     std::string parseValue(std::string, std::string, int);*/

};
#endif // MAINWINDOW_H
