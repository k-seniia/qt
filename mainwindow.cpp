#include "mainwindow.h"
#include <QCheckBox>
#include <QFile>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTextStream>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(
    QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    QLabel *label = new QLabel("Матриця показників якості:", this);
    mainLayout->addWidget(label);

    table = new QTableWidget(5, 5, this);
    table->setHorizontalHeaderLabels(
        {"Параметр 1", "Параметр 2", "Параметр 3", "Параметр 4", "Параметр 5"});
    mainLayout->addWidget(table);

    QLabel *paramLabel = new QLabel("Вибір двох параметрів для побудови графіка:", this);
    mainLayout->addWidget(paramLabel);

    QHBoxLayout *checksLayout = new QHBoxLayout();
    for (int i = 0; i < 10; ++i) {
        QCheckBox *check = new QCheckBox(QString::number(i + 1), this);
        parameterChecks.append(check);
        checksLayout->addWidget(check);
    }
    mainLayout->addLayout(checksLayout);

    QHBoxLayout *fileLayout = new QHBoxLayout();
    filePathEdit = new QLineEdit(this);
    QPushButton *loadButton = new QPushButton("Завантажити матрицю", this);
    connect(loadButton, &QPushButton::clicked, this, &MainWindow::loadMatrixFromFile);
    fileLayout->addWidget(filePathEdit);
    fileLayout->addWidget(loadButton);
    mainLayout->addLayout(fileLayout);

    QPushButton *plotButton = new QPushButton("Побудувати графік", this);
    connect(plotButton, &QPushButton::clicked, this, &MainWindow::plotGraph);
    mainLayout->addWidget(plotButton);

    QPushButton *paretoButton = new QPushButton("Парето-оптимізація", this);
    connect(paretoButton, &QPushButton::clicked, this, &MainWindow::runParetoOptimization);
    mainLayout->addWidget(paretoButton);

    QPushButton *singleOptionButton = new QPushButton("Вибір єдиного варіанта", this);
    connect(singleOptionButton, &QPushButton::clicked, this, &MainWindow::selectSingleOption);
    mainLayout->addWidget(singleOptionButton);
}

void MainWindow::selectSingleOption()
{
    QMessageBox::information(this, "Вибір", "Вибір єдиного варіанта виконаний (заглушка).");
}

void MainWindow::runParetoOptimization()
{
    QMessageBox::information(this, "Парето", "Парето-оптимізація виконана (заглушка).");
}

void MainWindow::plotGraph()
{
    int checkedCount = 0;
    QString selectedParams;
    for (int i = 0; i < parameterChecks.size(); ++i) {
        if (parameterChecks[i]->isChecked()) {
            ++checkedCount;
            selectedParams += QString("Параметр %1 ").arg(i + 1);
        }
    }

    if (checkedCount != 2) {
        QMessageBox::warning(this, "Увага", "Потрібно обрати рівно два параметри!");
        return;
    }

    QMessageBox::information(this,
                             "Графік",
                             QString("Будується графік для: %1").arg(selectedParams));
}

void MainWindow::loadMatrixFromFile()
{
    QString filePath = QFileDialog::getOpenFileName(this,
                                                    "Оберіть файл",
                                                    "",
                                                    "Text Files (*.txt);;All Files (*)");
    if (filePath.isEmpty())
        return;

    filePathEdit->setText(filePath);
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Помилка", "Не вдалося відкрити файл.");
        return;
    }

    QTextStream in(&file);
    int row = 0;
    while (!in.atEnd() && row < table->rowCount()) {
        QString line = in.readLine();
        QStringList values = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        for (int col = 0; col < values.size() && col < table->columnCount(); ++col) {
            table->setItem(row, col, new QTableWidgetItem(values[col]));
        }
        ++row;
    }
}
