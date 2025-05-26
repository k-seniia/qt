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
#include "doubleitemdelegate.h"

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
    QStringList headers;
    for (int i = 0; i < numCriteria; ++i)
        headers << QString("Параметр %1").arg(i + 1);

    inputTable = new QTableWidget(numAlternatives, numCriteria, this);
    inputTable->setHorizontalHeaderLabels(headers);

    normalizedTable = new QTableWidget(numAlternatives, numCriteria, this);
    normalizedTable->setHorizontalHeaderLabels(headers);

    minimizedTable = new QTableWidget(numAlternatives, numCriteria, this);
    minimizedTable->setHorizontalHeaderLabels(headers);

    paretoTable = new QTableWidget(numAlternatives, numCriteria + 1, this);
    headers << "Статус";
    paretoTable->setHorizontalHeaderLabels(headers);

    headers.clear();

    valueFunctionTable = new QTableWidget(numAlternatives, 2, this);
    headers << "Розрахунок";
    headers << "Ф. цінності";
    valueFunctionTable->setHorizontalHeaderLabels(headers);
    //

    auto *doubleDelegate = new DoubleItemDelegate(this);
    inputTable->setItemDelegate(doubleDelegate);

    //
    tabWidget = new QTabWidget(this);

    tabWidget->addTab(inputTable, "Введення даних");
    tabWidget->addTab(normalizedTable, "Нормалізована матриця");
    tabWidget->addTab(minimizedTable, "Зведення до мінімізації");
    tabWidget->addTab(paretoTable, "Парето оптимізація");
    tabWidget->addTab(valueFunctionTable, "Вибір єдиного варіанта");

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

    fillNormButton = new QPushButton("Нормалізація значень", this);
    connect(fillNormButton, &QPushButton::clicked, this, [this]() {
        if (!validateTableData())
            return;
        fillNormalizedTable();
    });
    mainLayout->addWidget(fillNormButton);
    //fillNormButton->setEnabled(false);

    fillMinButton = new QPushButton("Мінімізація значень", this);
    connect(fillMinButton, &QPushButton::clicked, this, [this]() {
        if (!validateTableData())
            return;
        fillMinimizedTable();
    });
    mainLayout->addWidget(fillMinButton);
    fillMinButton->setEnabled(false);

    analyzeDominanceButton = new QPushButton("Аналіз домінування", this);
    mainLayout->addWidget(analyzeDominanceButton);
    connect(analyzeDominanceButton, &QPushButton::clicked, this, &MainWindow::analyzeDominance);
    analyzeDominanceButton->setEnabled(false);

    singleOptionButton = new QPushButton("Вибір єдиного варіанта", this);
    connect(singleOptionButton, &QPushButton::clicked, this, &MainWindow::selectSingleOption);
    mainLayout->addWidget(singleOptionButton);
    singleOptionButton->setEnabled(false);

    /*QPushButton *plotButton = new QPushButton("Побудувати графік", this);
    connect(plotButton, &QPushButton::clicked, this, &MainWindow::plotGraph);
    mainLayout->addWidget(plotButton);*/
}

void MainWindow::updateButtonsState()
{
    // 2. Для мінімізації — normalizedTable має бути заповнена
    bool normReady = true;
    if (normalizedTable) {
        for (int i = 0; i < normalizedTable->rowCount(); ++i) {
            for (int j = 0; j < normalizedTable->columnCount(); ++j) {
                QTableWidgetItem *item = normalizedTable->item(i, j);
                if (!item || item->text().isEmpty()) {
                    normReady = false;
                    break;
                }
            }
        }
    }
    if (fillMinButton)
        fillMinButton->setEnabled(normReady);

    // 3. Для аналізу домінування — minimizedTable має бути заповнена
    bool minReady = true;
    if (minimizedTable) {
        for (int i = 0; i < minimizedTable->rowCount(); ++i) {
            for (int j = 0; j < minimizedTable->columnCount(); ++j) {
                QTableWidgetItem *item = minimizedTable->item(i, j);
                if (!item || item->text().isEmpty()) {
                    minReady = false;
                    break;
                }
            }
        }
    }
    if (analyzeDominanceButton)
        analyzeDominanceButton->setEnabled(minReady);

    // 4. Для вибору єдиного варіанта — вага + minimizedTable + paretoTable
    bool weightsValid = true;
    double weightSum = 0.0;
    for (QLineEdit *edit : weightEdits) {
        bool ok;
        double val = edit->text().replace(",", ".").toDouble(&ok);
        if (!ok || edit->text().isEmpty()) {
            weightsValid = false;
            break;
        }
        weightSum += val;
    }

    const double epsilon = 0.0001;
    if (std::abs(weightSum - 1.0) > epsilon)
        weightsValid = false;

    bool paretoReady = paretoTable && paretoTable->rowCount() > 0;

    if (singleOptionButton)
        singleOptionButton->setEnabled(minReady && paretoReady && weightsValid);
}

QVector<QVector<double>> MainWindow::getMatrixFromTable(
    QTableWidget *table) const
{
    QVector<QVector<double>> matrix;
    for (int i = 0; i < table->rowCount(); ++i) {
        QVector<double> row;
        for (int j = 0; j < table->columnCount(); ++j) {
            QTableWidgetItem *item = table->item(i, j);
            row.append(item->text().toDouble());
        }
        matrix.append(row);
    }
    return matrix;
}

bool dominates(
    const QVector<double> &a, const QVector<double> &b)
{
    for (int i = 0; i < a.size(); i++)
        if (a[i] > b[i])
            return false;
    return true;
}

void MainWindow::analyzeDominance()
{
    const QVector<QVector<double>> matrix = getMatrixFromTable(minimizedTable);

    for (int row = 0; row < minimizedTable->rowCount(); ++row) {
        for (int col = 0; col < minimizedTable->columnCount(); ++col) {
            QTableWidgetItem *sourceItem = minimizedTable->item(row, col);
            if (sourceItem) {
                QTableWidgetItem *newItem = new QTableWidgetItem(*sourceItem);
                newItem->setFlags(newItem->flags() & ~Qt::ItemIsEditable); // Заборонити редагування
                paretoTable->setItem(row, col, newItem);
            }
        }
    }

    QVector<QPair<int, int>> worsePairs;
    QVector<int> excluded;
    for (int i = 0; i < matrix.size(); i++) {
        if (excluded.contains(i))
            continue;
        for (int j = 0; j < matrix.size(); j++) {
            if (i != j && !excluded.contains(j) && dominates(matrix[i], matrix[j])) {
                worsePairs.emplace_back(j, i);
                excluded.push_back(j);
            }
        }
    }

    for (int i = 0; i < matrix.size(); ++i) {
        QTableWidgetItem *item = new QTableWidgetItem;

        const QColor rowColor = QColor(255, 200, 200);

        if (excluded.contains(i)) {
            int dominator = -1;
            for (const auto &pair : worsePairs) {
                if (pair.first == i) {
                    dominator = pair.second;
                    break;
                }
            }
            item->setText("БГ до " + QString::number(dominator + 1));
        } else {
            item->setText("ПО");
        }

        item->setFlags(item->flags() & ~Qt::ItemIsEditable); // Заборонити редагування
        paretoTable->setItem(i, matrix[0].size(), item);

        int colCount = paretoTable->columnCount();
        for (int j = 0; j < colCount; ++j) {
            if (item->text() != "ПО") {
                QTableWidgetItem *cellItem = paretoTable->item(i, j);
                cellItem->setBackground(QBrush(rowColor));
            }
        }
    }

    int index = tabWidget->indexOf(paretoTable);
    tabWidget->setCurrentIndex(index);

    updateButtonsState();
}

QVector<QString> MainWindow::getOptimizationTargets() const
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

    int index = tabWidget->indexOf(normalizedTable);
    tabWidget->setCurrentIndex(index);

    updateButtonsState();
}

void MainWindow::fillMinimizedTable()
{
    QVector<QVector<double>> norm = getMatrixFromTable(normalizedTable);
    QVector<QString> targets = getOptimizationTargets();

    //minimizedTable->setColumnCount(norm[0].size() + 1);
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

    int index = tabWidget->indexOf(minimizedTable);
    tabWidget->setCurrentIndex(index);

    updateButtonsState();
}

void MainWindow::validateWeightSum()
{
    double sum = 0.0;
    bool allValid = true;

    for (QLineEdit *edit : weightEdits) {
        QString text = edit->text().trimmed();
        if (text.isEmpty()) {
            allValid = false;
            break;
        }

        bool ok;
        double val = text.toDouble(&ok);
        if (!ok) {
            allValid = false;
            break;
        }
        sum += val;
    }

    const double epsilon = 0.0001;

    bool showError = !allValid || std::abs(sum - 1.0) > epsilon;

    for (QLineEdit *edit : weightEdits) {
        if (showError) {
            edit->setStyleSheet("background-color: #ffcccc");
        } else {
            edit->setStyleSheet("");
        }
    }

    if (weightErrorLabel) {
        if (showError) {
            if (!allValid)
                weightErrorLabel->setText("Усі поля мають бути заповнені коректними числами.");
            else
                weightErrorLabel->setText("Сума ваг повинна дорівнювати 1.");
        } else {
            weightErrorLabel->setText("");
        }
    }

    if (singleOptionButton) {
        singleOptionButton->setEnabled(!showError);
    }
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

    QRegularExpression regex(R"(^([0]|1|0[.,]\d{0,8}|1[.,]0{0,8})?$)");

    int cols = inputTable->columnCount();
    for (int i = 0; i < cols; ++i) {
        QLineEdit *edit = new QLineEdit(this);
        edit->setPlaceholderText(QString("П%1").arg(i + 1));

        auto *validator = new QRegularExpressionValidator(regex, edit);
        edit->setValidator(validator);

        connect(edit, &QLineEdit::textChanged, this, [this]() {
            validateWeightSum(); // новий метод
        });

        // Автоматична заміна коми на крапку
        connect(edit, &QLineEdit::textChanged, edit, [edit]() {
            QString text = edit->text();
            if (text.contains(',')) {
                int pos = edit->cursorPosition();
                text.replace(',', '.');
                edit->setText(text);
                edit->setCursorPosition(pos);
            }
        });

        weightEdits.append(edit);
        weightsLayout->addWidget(edit);
    }

    weightErrorLabel = new QLabel(this);
    weightErrorLabel->setStyleSheet("color: red;");
    weightErrorLabel->setText(""); // спочатку порожній
    weightsLayout->addWidget(weightErrorLabel);

    //updateButtonsState();
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

    paretoTable->clear();
    paretoTable->setRowCount(numAlternatives);
    paretoTable->setColumnCount(numCriteria + 1);
    headers << "Статус";
    paretoTable->setHorizontalHeaderLabels(headers);

    valueFunctionTable->clear();
    valueFunctionTable->setRowCount(numAlternatives);

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

QVector<double> MainWindow::getWeights() const
{
    QVector<double> weights;
    for (QLineEdit *edit : weightEdits) {
        bool ok = false;
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
    QVector<QVector<double>> matrix = getMatrixFromTable(minimizedTable);
    QVector<double> weights = getWeights();

    if (weights.empty()) {
        QMessageBox::warning(nullptr, "Помилка", "Пусте значення ваги.");
        return;
    } else
        for (int row = 0; row < matrix.size(); ++row) {
            double value = 0.0;
            QString status = paretoTable->item(row, paretoTable->columnCount() - 1)->text();
            if (status == "ПО") {
                for (int col = 0; col < matrix[0].size(); ++col) {
                    double criterion = matrix[row][col];
                    double weight = weights[col];
                    value += criterion * weight;
                }

                QTableWidgetItem *item = new QTableWidgetItem(QString::number(value, 'f', 4));
                item->setFlags(item->flags() & ~Qt::ItemIsEditable); // Заборонити редагування
                valueFunctionTable->setItem(row, 1, item);
            } else {
                const QColor rowColor = QColor(255, 200, 200);
                //valueFunctionTable->setItem(row, 1, new QTableWidgetItem(""));
                int colCount = valueFunctionTable->columnCount();
                for (int j = 0; j < colCount; ++j) {
                    QTableWidgetItem *cellItem = valueFunctionTable->item(row, j);
                    if (!cellItem) {
                        cellItem = new QTableWidgetItem();
                        valueFunctionTable->setItem(row, j, cellItem);
                    }
                    cellItem->setBackground(QBrush(rowColor));
                }
            }
        }
    highlightRowWithMinValue();

    int index = tabWidget->indexOf(valueFunctionTable);
    tabWidget->setCurrentIndex(index);
}

void MainWindow::highlightRowWithMinValue()
{
    if (!valueFunctionTable || valueFunctionTable->rowCount() == 0
        || valueFunctionTable->columnCount() == 0)
        return;

    int rowCount = valueFunctionTable->rowCount();
    int lastCol = valueFunctionTable->columnCount() - 1;

    double minValue = std::numeric_limits<double>::max();
    int minRow = -1;

    for (int i = 0; i < rowCount; ++i) {
        QTableWidgetItem *item = valueFunctionTable->item(i, lastCol);
        if (!item)
            continue;

        QString text = item->text().trimmed();
        bool ok;
        double val = text.toDouble(&ok);

        if (ok && val < minValue) {
            minValue = val;
            minRow = i;
        }
    }

    if (minRow != -1) {
        QColor greenColor = QColor(200, 255, 200); // світло-зелений
        for (int j = 0; j < valueFunctionTable->columnCount(); ++j) {
            QTableWidgetItem *cellItem = valueFunctionTable->item(minRow, j);
            if (cellItem)
                cellItem->setBackground(QBrush(greenColor));
        }
    }
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

    //updateButtonsState();
}
