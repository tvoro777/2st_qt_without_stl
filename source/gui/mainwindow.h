#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include "../opener_filter/opener_filter.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void on_openCsvButton_clicked();
    void on_filterButton_clicked();
    void on_filterByTimeButton_clicked();

private:
    QPushButton *openCsvButton;
    QPushButton *filterButton;
    QPushButton *filterTimeButton;
    QTableWidget *tableWidget;
    opener_filter csvHandler;
    QList<QStringList> currentData;
    void showData(const QList<QStringList> &data);
};

#endif

