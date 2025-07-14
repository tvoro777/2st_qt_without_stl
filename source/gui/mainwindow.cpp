#include "mainwindow.h"
#include "../opener_filter/opener_filter.h"
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>

#include "mainwindow.h"
#include <QVBoxLayout>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    openCsvButton(nullptr),
    filterButton(nullptr),
    filterTimeButton(nullptr),
    tableWidget(nullptr)
{
    QWidget* central = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(central);

    openCsvButton = new QPushButton("OpenCSV", central);
    filterButton = new QPushButton("Filter by Frequency", central);
    filterTimeButton = new QPushButton("Filter by Current Time", central);
    tableWidget = new QTableWidget(central);

    if (!openCsvButton || !filterButton || !filterTimeButton || !tableWidget) {
        qFatal("Ошибка создания виджетов!");
        return;
    }

    layout->addWidget(openCsvButton);
    layout->addWidget(filterButton);
    layout->addWidget(filterTimeButton);
    layout->addWidget(tableWidget);

    layout->setStretch(0, 0);
    layout->setStretch(1, 0);
    layout->setStretch(2, 0);
    layout->setStretch(3, 1);

    setCentralWidget(central);

    connect(openCsvButton, &QPushButton::clicked, this, &MainWindow::on_openCsvButton_clicked);
    connect(filterButton, &QPushButton::clicked, this, &MainWindow::on_filterButton_clicked);
    connect(filterTimeButton, &QPushButton::clicked, this, &MainWindow::on_filterByTimeButton_clicked);

    // 9. Настройка размеров
    resize(1000, 700);

    qDebug() << "Конструктор MainWindow завершил работу";
}

void MainWindow::showData(const QList<QStringList> &data) {
    tableWidget->clear();
    if (data.isEmpty()) return;

    // Устанавливаем количество столбцов по первой строке
    tableWidget->setColumnCount(data.first().size());
    tableWidget->setRowCount(data.size());

    // qDebug() << "Raw data after parsing:";
    // for (const QStringList &row : data) {
    //     qDebug() << "Row:" << row;
    // }

    // Заполняем таблицу
    for (int row = 0; row < data.size(); ++row) {
        const QStringList &rowData = data[row];
        for (int col = 0; col < rowData.size(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(rowData[col]);
            tableWidget->setItem(row, col, item);
        }
    }
}

void MainWindow::on_openCsvButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Открыть CSV", "", "CSV files (*.csv);;All files (*)");
    if (fileName.isEmpty())
        return;

    currentData = csvHandler.readCsv(fileName);
    qDebug() << "Прочитано строк:" << currentData.size();
    if (!currentData.isEmpty()) {
        qDebug() << "Заголовки:" << currentData.first();
        if (currentData.size() > 1) {
            qDebug() << "Первая строка данных:" << currentData[1];
        }
    }
    showData(currentData);
}

void MainWindow::on_filterButton_clicked() {
    if (currentData.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Нет данных для фильтрации!");
        return;
    }

    bool okMin, okMax;
    double minFreq = QInputDialog::getDouble(this, "Минимальная частота",
                                             "Введите минимальную частоту (кГц):", 15, 0, 100000, 2, &okMin);
    if (!okMin) return;

    double maxFreq = QInputDialog::getDouble(this, "Максимальная частота",
                                             "Введите максимальную частоту (кГц):", 25, 0, 100000, 2, &okMax);
    if (!okMax) return;

    if (minFreq > maxFreq) {
        QMessageBox::warning(this, "Ошибка", "Минимальная частота не может быть больше максимальной!");
        return;
    }

    QList<QStringList> filtered = csvHandler.filterByFrequency(currentData, minFreq, maxFreq);
    showData(filtered);
}

void MainWindow::on_filterByTimeButton_clicked()
{
    if (currentData.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Нет данных для фильтрации");
    }

    QDateTime currentDataTime = QDateTime::currentDateTime();
    QString currentDateTimeStr = currentDataTime.toString("yyyy-MM-dd HH:mm:ss");
    qDebug() << "Текущее время:" << currentDateTimeStr;

    QList<QStringList> filtered = csvHandler.filterByTime(currentData, currentDateTimeStr);
    showData(filtered);
}



