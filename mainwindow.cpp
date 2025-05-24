#include "mainwindow.h"
#include <QCheckBox>
#include <QComboBox>
#include <QFile>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
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

    //
    //
    QHBoxLayout *sizeLayout = new QHBoxLayout();
    sizeLayout->addWidget(new QLabel("Альтернативи:", this));
    altSpin = new QSpinBox(this);
    altSpin->setRange(1, 100);
    altSpin->setValue(5);
    sizeLayout->addWidget(altSpin);

    sizeLayout->addWidget(new QLabel("Критерії:", this));
    critSpin = new QSpinBox(this);
    critSpin->setRange(1, 10);
    critSpin->setValue(5);
    sizeLayout->addWidget(critSpin);

    QPushButton *resizeButton = new QPushButton("Оновити таблицю", this);
    connect(resizeButton, &QPushButton::clicked, this, &MainWindow::updateTableSize);
    sizeLayout->addWidget(resizeButton);

    mainLayout->addLayout(sizeLayout);

    numAlternatives = altSpin->value();
    numCriteria = critSpin->value();
    //

    //
    QLabel *optLabel = new QLabel("Оптимізація критеріїв:");
    optimizationLayout = new QHBoxLayout();
    mainLayout->addWidget(optLabel);
    for (int i = 0; i < 5; ++i) {
        QComboBox *combo = new QComboBox();
        combo->addItem("min");
        combo->addItem("max");
        combo->setCurrentText("min");
        optimizationCombos.append(combo);
        optimizationLayout->addWidget(combo);
    }
    mainLayout->addLayout(optimizationLayout);
    //

    //
    checksLayout = new QHBoxLayout();
    mainLayout->addLayout(checksLayout);
    updateParameterCheckboxes();
    //

    //
    QVector<QString> targets = getOptimizationTargets();

    QStringList headers;
    for (int i = 0; i < numCriteria; ++i)
        headers << QString("Параметр %1").arg(i + 1);

    inputTable = new QTableWidget(numAlternatives, numCriteria, this);
    inputTable->setHorizontalHeaderLabels(headers);

    normalizedTable = new QTableWidget(numAlternatives, numCriteria, this);
    normalizedTable->setHorizontalHeaderLabels(headers);

    minimizedTable = new QTableWidget(numAlternatives, numCriteria, this);
    minimizedTable->setHorizontalHeaderLabels(headers);
    //

    //
    tabWidget = new QTabWidget(this);

    tabWidget->addTab(inputTable, "Введення даних");
    tabWidget->addTab(normalizedTable, "Нормалізована матриця");
    tabWidget->addTab(minimizedTable, "Зведення до мінімізації");

    mainLayout->addWidget(tabWidget);
    //
    //

    //
    QLabel *weightLabel = new QLabel("Введіть ваги для кожного параметра:", this);
    mainLayout->addWidget(weightLabel);

    weightsLayout = new QHBoxLayout();
    mainLayout->addLayout(weightsLayout);
    updateWeightInputs();
    //

    QHBoxLayout *fileLayout = new QHBoxLayout();
    filePathEdit = new QLineEdit(this);

    QPushButton *loadButton = new QPushButton("Завантажити матрицю", this);
    connect(loadButton, &QPushButton::clicked, this, &MainWindow::loadMatrixFromFile);
    fileLayout->addWidget(filePathEdit);
    fileLayout->addWidget(loadButton);
    mainLayout->addLayout(fileLayout);

    QPushButton *fillNormButton = new QPushButton("Нормалізація значень", this);
    connect(fillNormButton, &QPushButton::clicked, this, [this]() {
        if (!validateTableData())
            return;
        fillNormalizedTable();
    });
    mainLayout->addWidget(fillNormButton);

    QPushButton *fillMinButton = new QPushButton("Мінімізація значень", this);
    connect(fillMinButton, &QPushButton::clicked, this, [this]() {
        if (!validateTableData())
            return;
        fillMinimizedTable();
    });
    mainLayout->addWidget(fillMinButton);

    QPushButton *analyzeDominanceButton = new QPushButton("Аналіз домінування", this);
    mainLayout->addWidget(analyzeDominanceButton);
    connect(analyzeDominanceButton, &QPushButton::clicked, this, &MainWindow::analyzeDominance);

    /*QPushButton *plotButton = new QPushButton("Побудувати графік", this);
    connect(plotButton, &QPushButton::clicked, this, &MainWindow::plotGraph);
    mainLayout->addWidget(plotButton);*/

    /*QPushButton *paretoButton = new QPushButton("Парето-оптимізація", this);
    connect(paretoButton, &QPushButton::clicked, this, &MainWindow::runParetoOptimization);
    mainLayout->addWidget(paretoButton);*/

    /*QPushButton *singleOptionButton = new QPushButton("Вибір єдиного варіанта", this);
    connect(singleOptionButton, &QPushButton::clicked, this, &MainWindow::selectSingleOption);
    mainLayout->addWidget(singleOptionButton);*/
}

QVector<QVector<double>> MainWindow::getMatrixFromTable(
    QTableWidget *table)
{
    QVector<QVector<double>> matrix;
    for (int i = 0; i < table->rowCount(); ++i) {
        QVector<double> row;
        for (int j = 0; j < table->columnCount(); ++j) {
            auto *item = table->item(i, j);
            bool ok;
            double val = item ? item->text().toDouble(&ok) : 0.0;
            row.append(ok ? val : 0.0);
        }
        matrix.append(row);
    }
    return matrix;
}

bool dominates(
    const QVector<double> &a, const QVector<double> &b)
{
    bool strictlyBetter = false;
    for (int i = 0; i < a.size(); i++) {
        if (a[i] > b[i])
            return false;
        if (a[i] < b[i])
            strictlyBetter = true;
    }
    return strictlyBetter;
}

void MainWindow::analyzeDominance()
{
    QVector<QVector<double>> matrix = getMatrixFromTable(minimizedTable);

    QVector<QPair<int, int>> worsePairs;
    QSet<int> excluded;
    for (int i = 0; i < matrix.size(); i++) {
        if (excluded.contains(i))
            continue;
        for (int j = 0; j < matrix.size() - 1; j++) {
            if (i != j && !excluded.contains(j) && dominates(matrix[i], matrix[j])) {
                worsePairs.emplace_back(j, i);
                excluded.insert(j);
            }
        }
    }

    QVector<bool> isDominated(matrix.size(), false);
    for (int i = 0; i < matrix.size(); i++) {
        if (excluded.contains(i))
            continue;
        for (int j = 0; j < matrix.size(); j++) {
            if (i != j && !excluded.contains(j) && dominates(matrix[j], matrix[i])) {
                isDominated[i] = true;
                break;
            }
        }
    }
    QVector<int> paretoIndices;
    for (int i = 0; i < matrix.size(); i++) {
        if (!excluded.contains(i) && !isDominated[i])
            paretoIndices.push_back(i);
    }

    /*int n = matrix.size();
    if (n == 0)
        return;
    int m = matrix[0].size() - 1;

    for (int i = 0; i < n; ++i) {
        bool isDominated = false;
        for (int j = 0; j < n; ++j) {
            if (i == j)
                continue;

            bool allLessOrEqual = true;
            bool atLeastOneLess = false;

            for (int k = 0; k < m; ++k) {
                if (matrix[j][k] < matrix[i][k])
                    atLeastOneLess = true;
                if (matrix[j][k] > matrix[i][k])
                    allLessOrEqual = false;
            }

            if (allLessOrEqual && atLeastOneLess) {
                isDominated = true;
                break;
            }
        }

        QString status = isDominated ? "БГ до *" : "ПО";
        QTableWidgetItem *item = new QTableWidgetItem(status);
        minimizedTable->setItem(i, matrix[0].size() - 1, item);
    }*/
}

QVector<QString> MainWindow::getOptimizationTargets()
{
    QVector<QString> targets;
    for (QComboBox *combo : optimizationCombos) {
        targets.append(combo->currentText());
    }
    return targets;
}

QVector<QVector<double>> normalizeMatrix(
    const QVector<QVector<double>> &matrix)
{
    int numAlternatives = matrix.size();
    int numCriteria = matrix[0].size();
    QVector<double> maxValues(numCriteria, 0.0);

    for (int j = 0; j < numCriteria; j++) {
        for (int i = 0; i < numAlternatives; i++) {
            if (matrix[i][j] > maxValues[j]) {
                maxValues[j] = matrix[i][j];
            }
        }
    }

    QVector<QVector<double>> normalized(numAlternatives, QVector<double>(numCriteria));
    for (int i = 0; i < numAlternatives; i++) {
        for (int j = 0; j < numCriteria; j++) {
            normalized[i][j] = matrix[i][j] / maxValues[j];
        }
    }

    return normalized;
}

void MainWindow::fillNormalizedTable()
{
    QVector<QVector<double>> matrix = getMatrixFromTable(inputTable);
    QVector<QVector<double>> norm = normalizeMatrix(matrix);

    for (int i = 0; i < norm.size(); ++i) {
        for (int j = 0; j < norm[i].size(); ++j) {
            QTableWidgetItem *item = new QTableWidgetItem(QString::number(norm[i][j], 'f', 3));
            item->setFlags(item->flags() & ~Qt::ItemIsEditable); // Заборонити редагування
            normalizedTable->setItem(i, j, item);
        }
    }
}

QVector<int> findParetoSet(
    const QVector<QVector<double>> &matrix)
{
    int n = matrix.size();
    int m = matrix[0].size();
    QVector<int> pareto;

    for (int i = 0; i < n; ++i) {
        bool dominated = false;
        for (int j = 0; j < n; ++j) {
            if (i == j)
                continue;
            bool allBetterOrEqual = true;
            bool strictlyBetter = false;
            for (int k = 0; k < m; ++k) {
                if (matrix[j][k] < matrix[i][k]) {
                    allBetterOrEqual = false;
                    break;
                }
                if (matrix[j][k] > matrix[i][k])
                    strictlyBetter = true;
            }
            if (allBetterOrEqual && strictlyBetter) {
                dominated = true;
                break;
            }
        }
        if (!dominated)
            pareto.append(i);
    }

    return pareto;
}

void MainWindow::fillMinimizedTable()
{
    QVector<QVector<double>> norm = getMatrixFromTable(normalizedTable);
    QVector<QString> targets = getOptimizationTargets();

    minimizedTable->setColumnCount(norm[0].size() + 1);
    QStringList headers;
    for (int j = 0; j < norm[0].size(); ++j)
        headers << QString("Параметр %1").arg(j + 1);
    headers << "Статус";
    minimizedTable->setHorizontalHeaderLabels(headers);

    minimizedTable->clear();

    for (int i = 0; i < numAlternatives; ++i) {
        for (int j = 0; j < numCriteria; ++j) {
            double value = norm[i][j];
            if (targets[j] == "max") {
                value = 1.0 - value;
            }

            QTableWidgetItem *item = new QTableWidgetItem(QString::number(value, 'f', 3));
            item->setFlags(item->flags() & ~Qt::ItemIsEditable); // Заборонити редагування
            minimizedTable->setItem(i, j, item);
        }
    }

    /*// Знайти Парето-оптимальні альтернативи
    QVector<int> pareto = findParetoSet(norm);

    for (int i = 0; i < norm.size(); ++i) {
        for (int j = 0; j < norm[i].size(); ++j) {
            QTableWidgetItem *item = new QTableWidgetItem(QString::number(norm[i][j], 'f', 4));
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            minimizedTable->setItem(i, j, item);
        }

        QString status = pareto.contains(i) ? "Парето-оптимальна" : "Безумовно гірша";
        QTableWidgetItem *statusItem = new QTableWidgetItem(status);
        statusItem->setFlags(statusItem->flags() & ~Qt::ItemIsEditable);
        minimizedTable->setItem(i, norm[i].size(), statusItem);
    }*/
}

void MainWindow::updateWeightInputs()
{
    qDeleteAll(weightEdits);
    weightEdits.clear();

    QLayoutItem *child;
    while ((child = weightsLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    int cols = inputTable->columnCount();
    for (int i = 0; i < cols; ++i) {
        QLineEdit *edit = new QLineEdit(this);
        edit->setPlaceholderText(QString("П%1").arg(i + 1));
        edit->setValidator(new QDoubleValidator(0, 1, 4, edit));
        weightEdits.append(edit);
        weightsLayout->addWidget(edit);
    }
}

bool MainWindow::validateTableData()
{
    for (int row = 0; row < inputTable->rowCount(); ++row) {
        for (int col = 0; col < inputTable->columnCount(); ++col) {
            QTableWidgetItem *item = inputTable->item(row, col);
            if (!item) {
                QMessageBox::warning(this,
                                     "Помилка",
                                     QString("Порожня клітинка [%1, %2]").arg(row + 1).arg(col + 1));
                return false;
            }

            bool ok;
            item->text().toDouble(&ok);
            if (!ok) {
                QMessageBox::warning(this,
                                     "Помилка",
                                     QString("Некоректне значення [%1, %2]: '%3'")
                                         .arg(row + 1)
                                         .arg(col + 1)
                                         .arg(item->text()));
                return false;
            }
        }
    }
    return true;
}

void MainWindow::updateParameterCheckboxes()
{
    QLayoutItem *child;
    while ((child = checksLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }
    parameterChecks.clear();

    for (int i = 0; i < numCriteria; ++i) {
        QCheckBox *check = new QCheckBox(QString::number(i + 1), this);
        parameterChecks.append(check);
        checksLayout->addWidget(check);
    }
}

void MainWindow::updateTableSize()
{
    numAlternatives = altSpin->value();
    numCriteria = critSpin->value();

    inputTable->clear();
    inputTable->setRowCount(numAlternatives);
    inputTable->setColumnCount(numCriteria);

    QStringList headers;
    for (int i = 0; i < numCriteria; ++i)
        headers << QString("Параметр %1").arg(i + 1);
    inputTable->setHorizontalHeaderLabels(headers);

    normalizedTable->clear();
    normalizedTable->setRowCount(numAlternatives);
    normalizedTable->setColumnCount(numCriteria);
    normalizedTable->setHorizontalHeaderLabels(headers);

    minimizedTable->clear();
    minimizedTable->setRowCount(numAlternatives);
    minimizedTable->setColumnCount(numCriteria);
    minimizedTable->setHorizontalHeaderLabels(headers);

    updateParameterCheckboxes();
    updateWeightInputs();

    qDeleteAll(optimizationCombos);
    optimizationCombos.clear();

    QLayoutItem *item;
    while ((item = optimizationLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    for (int i = 0; i < numCriteria; ++i) {
        QComboBox *combo = new QComboBox();
        combo->addItem("min");
        combo->addItem("max");
        combo->setCurrentText("min");
        optimizationCombos.append(combo);
        optimizationLayout->addWidget(combo);
    }
}

int chooseBestByWeights(
    const QVector<QVector<double>> &matrix, const QVector<double> &weights)
{
    int n = matrix.size();
    double bestScore = -1.0;
    int bestIndex = -1;

    for (int i = 0; i < n; ++i) {
        double score = 0.0;
        for (int j = 0; j < weights.size(); ++j)
            score += matrix[i][j] * weights[j];
        if (score > bestScore) {
            bestScore = score;
            bestIndex = i;
        }
    }

    return bestIndex;
}

QVector<double> MainWindow::getWeights()
{
    QVector<double> weights;
    for (QLineEdit *edit : weightEdits) {
        bool ok;
        double val = edit->text().toDouble(&ok);
        if (!ok) {
            QMessageBox::warning(nullptr, "Помилка", "Введено некоректне значення ваги.");
            return {};
        }
        weights.append(val);
    }
    return weights;
}

void MainWindow::selectSingleOption()
{
    if (!validateTableData())
        return;

    QVector<QVector<double>> matrix = getMatrixFromTable(inputTable);
    QVector<QVector<double>> norm = normalizeMatrix(matrix);

    QVector<double> weights = getWeights();
    if (weights.isEmpty())
        return;

    int best = chooseBestByWeights(norm, weights);
    QMessageBox::information(this,
                             "Вибір варіанта",
                             QString("Найкраща альтернатива: %1").arg(best + 1));
}

void MainWindow::runParetoOptimization()
{
    if (!validateTableData())
        return;

    QVector<QVector<double>> matrix = getMatrixFromTable(inputTable);
    QVector<QVector<double>> norm = normalizeMatrix(matrix);
    QVector<int> pareto = findParetoSet(norm);

    QString result = "Парето-оптимальні альтернативи:\n";
    for (int i : pareto) {
        result += QString("Альтернатива %1\n").arg(i + 1);
    }
    QMessageBox::information(this, "Парето-оптимізація", result);
}

void MainWindow::plotGraph()
{
    if (!validateTableData())
        return;

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

    QMessageBox::
        information(this, "Графік", QString("Будується графік для: %1").arg(selectedParams));
}

void MainWindow::loadMatrixFromFile()
{
    QString filePath = QFileDialog::
        getOpenFileName(this, "Оберіть файл", "", "Text Files (*.txt);;All Files (*)");
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
    while (!in.atEnd() && row < inputTable->rowCount()) {
        QString line = in.readLine();
        QStringList values = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        for (int col = 0; col < values.size() && col < inputTable->columnCount(); ++col) {
            inputTable->setItem(row, col, new QTableWidgetItem(values[col]));
        }
        ++row;
    }
}
