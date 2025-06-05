#include "mainwindow.h"
#include "doubleitemdelegate.h"

MainWindow::MainWindow(
    QWidget *parent)
    : QMainWindow(parent)
{
    central = new QWidget(this);
    setCentralWidget(central);
    mainLayout = new QVBoxLayout(central);

    //
    //
    sizeContainer = new QGroupBox(this);
    sizeLayout = new QHBoxLayout(sizeContainer);
    sizeLayout->setContentsMargins(3, 3, 3, 3);

    sizeLayout->addWidget(new QLabel("Альтернативи:", this), 1);
    altSpin = new QSpinBox(this);
    altSpin->setRange(2, 100);
    altSpin->setValue(5);
    altSpin->setToolTip("Кількість рядків (від 2 до 100)");
    sizeLayout->addWidget(altSpin, 1);

    sizeLayout->addWidget(new QLabel("Параметри:", this), 1);
    critSpin = new QSpinBox(this);
    critSpin->setRange(2, 10);
    critSpin->setValue(5);
    critSpin->setToolTip("Кількість стовбців (від 2 до 10)");
    sizeLayout->addWidget(critSpin, 1);

    resizeButton = new QPushButton("Оновити таблицю", this);
    resizeButton->setToolTip("Оновлюються розміри усіх таблиць");
    connect(resizeButton, &QPushButton::clicked, this, &MainWindow::updateTableSize);
    sizeLayout->addWidget(resizeButton, 2);

    fileContainer = new QGroupBox(this);
    fileLayout = new QHBoxLayout(fileContainer);
    fileLayout->setContentsMargins(3, 3, 3, 3);

    filePathEdit = new QLineEdit(this);
    filePathEdit->setToolTip(
        "Введіть або виберіть шлях до файлу з даними для таблиці\n" "Кома, як роздільник між " "зна" "чен" "ням" "и, " "зчи" "тає" "тьс" "я " "некоректно");
    loadButton = new QPushButton("Завантажити таблицю", this);
    loadButton->setToolTip("Заповнити початкову таблицю");
    connect(loadButton, &QPushButton::clicked, this, &MainWindow::loadMatrixFromFile);
    fileLayout->addWidget(filePathEdit, 3);
    fileLayout->addWidget(loadButton, 2);

    inputLayout = new QHBoxLayout();
    inputLayout->addWidget(sizeContainer, 5);
    inputLayout->addWidget(fileContainer, 4);
    mainLayout->addLayout(inputLayout);

    numAlternatives = altSpin->value();
    numCriteria = critSpin->value();
    //

    //
    minmaxContainer = new QGroupBox("Мінімізація/Максимізація параметрів:", this);
    minmaxLayout = new QHBoxLayout(minmaxContainer);
    minmaxLayout->setAlignment(Qt::AlignLeft);
    minmaxLayout->setContentsMargins(20, 0, 3, 3);
    minmaxContainer->setToolTip(
        "Після внесення змін рекомендовано перерахувати таблицю " "мінімізації " "та всі залежні " "ві" "д " "неї");
    mainLayout->addWidget(minmaxContainer);
    //

    //
    weightContainer = new QGroupBox("Коефіцієнти цінності:", this);
    weightsLayout = new QHBoxLayout(weightContainer);
    weightsLayout->setAlignment(Qt::AlignLeft);
    weightsLayout->setContentsMargins(20, 0, 3, 3);
    mainLayout->addWidget(weightContainer);
    updateWeightInputs();
    //

    //
    checksContainer = new QGroupBox(this);
    checksLayout = new QHBoxLayout(checksContainer);
    checksLayout->setAlignment(Qt::AlignLeft);
    checksLayout->setContentsMargins(55, 0, 0, 0);
    checksContainer->setToolTip("Після внесення змін рекомендовано перерахувати ПО та Ф. цінності");
    mainLayout->addWidget(checksContainer);
    //

    //
    inputTable = new QTableWidget(this);
    inputTable->setToolTip("Введіть початкові значення параметрів");

    normalizedTable = new QTableWidget(this);
    normalizedTable->setToolTip("Нормалізовані значення");

    minimizedTable = new QTableWidget(this);
    minimizedTable->setToolTip("Перетворені значення для мінімізації");

    paretoTable = new QTableWidget(this);
    paretoTable->setToolTip("Результати Парето-аналізу");

    valueFunctionTable = new QTableWidget(this);
    valueFunctionTable->setToolTip("Розрахунок скалярної функції цінності для єдиного вибору");

    graphSummaryTable = new QTableWidget(this);
    graphSummaryTable->setToolTip("Зведення мінімізованих параметрів для БДО");

    graphChecksLayout = new QVBoxLayout();
    updateParameterCheckboxes();

    tab = new QWidget(this);

    graphScene = new QGraphicsScene(tab);
    graphView = new QGraphicsView(graphScene, tab);
    graphView->setMinimumHeight(350); // Висота графіка

    QHBoxLayout *graphLayout = new QHBoxLayout(tab);
    graphLayout->addWidget(graphSummaryTable);
    graphLayout->addLayout(graphChecksLayout);
    graphLayout->addWidget(graphView);
    graphLayout->setContentsMargins(0, 0, 0, 0);
    drawGraphAxes(graphScene);
    updateGraphParameterCheckboxes();
    //

    auto *doubleDelegate = new DoubleItemDelegate(this);
    inputTable->setItemDelegate(doubleDelegate);

    //
    tabWidget = new QTabWidget(this);

    tabWidget->addTab(inputTable, "Введення даних");
    tabWidget->addTab(normalizedTable, "Нормалізовані значення");
    tabWidget->addTab(minimizedTable, "Зведення до мінімізації");
    tabWidget->addTab(paretoTable, "Парето-оптимізація");
    tabWidget->addTab(valueFunctionTable, "Вибір єдиного варіанта");
    tabWidget->addTab(tab, "БДО");

    mainLayout->addWidget(tabWidget);
    //
    //

    fillNormButton = new QPushButton("Нормалізація значень", this);
    connect(fillNormButton, &QPushButton::clicked, this, [this]() {
        if (!validateTableData())
            return;
        fillNormalizedTable();
    });
    fillNormButton->setToolTip(
        "Нормалізує значення матриці (від 0 до 1)\nПотребує заповненої вхідної таблиці");

    fillMinButton = new QPushButton("Мінімізація значень", this);
    connect(fillMinButton, &QPushButton::clicked, this, [this]() {
        if (!validateTableData())
            return;
        fillMinimizedTable();
    });
    fillMinButton->setEnabled(false);
    fillMinButton->setToolTip(
        "Перетворює завдання на задачу мінімізації\nПотребує таблицю " "нормалізації та " "обраних " "опцій " "мін/макс");

    analyzeDominanceButton = new QPushButton("Парето-оптимізація", this);
    connect(analyzeDominanceButton, &QPushButton::clicked, this, &MainWindow::analyzeDominance);
    analyzeDominanceButton->setEnabled(false);
    analyzeDominanceButton->setToolTip(
        "Аналіз Парето-оптимальних рішень\nПотребує таблицю мінімізації");

    singleOptionButton = new QPushButton("Вибір єдиного варіанта", this);
    connect(singleOptionButton, &QPushButton::clicked, this, &MainWindow::selectSingleOption);
    singleOptionButton->setEnabled(false);
    singleOptionButton->setToolTip(
        "Знаходить єдиний варіант серед Парето-рішень\nПотребує таблицю " "мінімізації та " "заповн" "ені " "коефіц" "ієнт" "и");

    plotButton = new QPushButton("Побудувати БДО", this);
    connect(plotButton, &QPushButton::clicked, this, &MainWindow::plotGraph);
    plotButton->setEnabled(false);
    plotButton->setToolTip(
        "Побудувати двовимірний графік для обраних параметрів\nПотребує таблицю " "мінімізації та " "ДВА обраних " "параметри на вкладці БДО");

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(fillNormButton);
    buttonLayout->addWidget(fillMinButton);
    buttonLayout->addWidget(analyzeDominanceButton);
    buttonLayout->addWidget(singleOptionButton);
    buttonLayout->addWidget(plotButton);
    mainLayout->addLayout(buttonLayout);

    initConnections();
    updateTableSize();
}

QVector<int> MainWindow::getSelectedParameterIndexes() const
{
    QVector<int> selected;
    for (int i = 0; i < numCriteria; ++i) {
        if (parameterChecks[i]->isChecked())
            selected.append(i);
    }
    return selected;
}

void MainWindow::plotGraph()
{
    graphScene->clear();
    graphSummaryTable->clearContents();
    graphSummaryTable->setRowCount(0);

    QVector<int> selectedColumns;
    for (int i = 0; i < numCriteria; ++i)
        if (graphParameterChecks[i]->isChecked())
            selectedColumns.append(i);

    if (selectedColumns.size() != 2) {
        QMessageBox::warning(this, "Помилка", "Оберіть рівно 2 параметри для побудови БДО.");
        return;
    }

    int xCol = selectedColumns[0];
    int yCol = selectedColumns[1];

    QVector<QPointF> points;
    QVector<int> rowIndices;
    int rows = numAlternatives;
    for (int i = 0; i < rows; ++i) {
        bool ok1, ok2;
        double x = minimizedTable->item(i, xCol)->text().toDouble(&ok1);
        double y = minimizedTable->item(i, yCol)->text().toDouble(&ok2);
        if (ok1 && ok2) {
            points.append(QPointF(x, y));
            rowIndices.append(i);
        }
    }

    if (points.isEmpty())
        return;

    // Аналіз домінування
    QVector<bool> isPareto(points.size(), true);
    QVector<QString> dominanceInfo(points.size(), "ПО");

    for (int i = 0; i < points.size(); ++i) {
        for (int j = 0; j < points.size(); ++j) {
            if (i == j)
                continue;
            if ((points[j].x() <= points[i].x() && points[j].y() <= points[i].y())
                && (points[j].x() < points[i].x() || points[j].y() < points[i].y())) {
                isPareto[i] = false;
                dominanceInfo[i] = "БГ до " + QString::number(rowIndices[j] + 1);
                break;
            }
        }
    }

    drawGraphAxes(graphScene);

    // Побудова точок і таблички
    graphSummaryTable->setRowCount(points.size());
    graphSummaryTable->setColumnCount(3);
    graphSummaryTable->setHorizontalHeaderLabels({"X", "Y", "Статус"});

    for (int i = 0; i < points.size(); ++i) {
        const QPointF &p = points[i];
        double sceneX = margin + p.x() * width;
        double sceneY = margin + (1.0 - p.y()) * height;

        QColor color = isPareto[i] ? Qt::blue : Qt::red;
        graphScene->addEllipse(sceneX - 3, sceneY - 3, 6, 6, QPen(color), QBrush(color));

        // Додати номер рядка біля точки
        QGraphicsTextItem *label = graphScene->addText(QString::number(rowIndices[i] + 1));
        label->setPos(sceneX + 5, sceneY - 10);

        // Додати до таблички
        graphSummaryTable->setItem(i, 0, new QTableWidgetItem(QString::number(p.x(), 'g', 3)));
        graphSummaryTable->setItem(i, 1, new QTableWidgetItem(QString::number(p.y(), 'g', 3)));
        graphSummaryTable->setItem(i, 2, new QTableWidgetItem(dominanceInfo[i]));

        if (!isPareto[i]) {
            for (int j = 0; j < 3; ++j)
                graphSummaryTable->item(i, j)->setBackground(QColor(255, 200, 200));
        }
    }

    int index = tabWidget->indexOf(tab);
    tabWidget->setCurrentIndex(index);
}

void MainWindow::drawGraphAxes(
    QGraphicsScene *scene)
{
    QRectF sceneRect(0, 0, width + margin * 2, height + margin * 2);
    scene->setSceneRect(sceneRect);

    // Сітка: бліда кожні 0.05, пунктирна кожні 0.1
    QPen lightPen(QColor(200, 200, 200), 1, Qt::SolidLine);
    QPen dashPen(QColor(150, 150, 150), 1, Qt::DashLine);

    for (double v = 0.05; v <= 1.0; v += 0.05) {
        int x = margin + v * width;
        int y = margin + (1.0 - v) * height;

        QPen pen = (fmod(v * 10, 2) == 0) ? dashPen : lightPen;

        scene->addLine(x, margin, x, height + margin, pen); // вертикальна
        scene->addLine(margin, y, width + margin, y, pen);  // горизонтальна
    }

    // Ося X і Y
    QPen axisPen(Qt::black, 2);
    scene->addLine(margin, height + margin, width + margin, height + margin,
                   axisPen);                                          // X
    scene->addLine(margin, margin, margin, height + margin, axisPen); // Y

    // Поділки і підписи
    for (int i = 0; i <= 5; ++i) {
        double value = i * 0.2;
        int x = margin + value * width;
        int y = margin + height - value * height;

        scene->addLine(x, height + margin - 5, x, height + margin + 5, axisPen);
        scene->addText(QString::number(value, 'g', 1))->setPos(x - 10, height + margin + 5);

        scene->addLine(margin - 5, y, margin + 5, y, axisPen);
        scene->addText(QString::number(value, 'g', 1))->setPos(margin - 30, y - 10);
    }

    scene->addText("X")->setPos(width + margin + 10, height + margin - 10);
    scene->addText("Y")->setPos(margin - 20, margin - 30);
}

bool MainWindow::isTableFilled(
    QTableWidget *table) const
{
    if (!table || table->rowCount() == 0 || table->columnCount() == 0)
        return false;

    for (int i = 0; i < table->rowCount(); ++i) {
        for (int j = 0; j < table->columnCount(); ++j) {
            QTableWidgetItem *item = table->item(i, j);
            if (!item || item->text().isEmpty())
                return false;
        }
    }
    return true;
}

bool MainWindow::isTableFilledOnlyForActiveColumns(
    QTableWidget *table) const
{
    if (!table)
        return false;

    QVector<int> activeCols = getSelectedParameterIndexes();

    for (int i = 0; i < table->rowCount(); ++i) {
        for (int col : activeCols) {
            QTableWidgetItem *item = table->item(i, col);
            if (!item || item->text().isEmpty()) {
                return false;
            }
        }
        // Перевіряємо останню колонку "Статус" (ПО/БГ), яка також важлива
        int statusCol = table->columnCount() - 1;
        QTableWidgetItem *statusItem = table->item(i, statusCol);
        if (!statusItem || statusItem->text().isEmpty()) {
            return false;
        }
    }
    return true;
}

void MainWindow::updateButtonsState()
{
    // --- 1. Перевірка normalizedTable для кнопки мінімізації ---
    bool normReady = isTableFilled(normalizedTable);
    if (fillMinButton)
        fillMinButton->setEnabled(normReady);

    // --- 2. Перевірка minimizedTable для аналізу домінування ---
    bool minReady = isTableFilled(minimizedTable);
    if (analyzeDominanceButton)
        analyzeDominanceButton->setEnabled(minReady);

    // --- 3. Перевірка ваг (weightsValid) ---
    bool weightsValid = validateWeightSum();

    // --- 4. Перевірка, що paretoTable готова ---
    bool paretoReady = isTableFilledOnlyForActiveColumns(paretoTable);

    bool singleReady = paretoReady && weightsValid;

    if (singleOptionButton)
        singleOptionButton->setEnabled(singleReady);

    // --- 5. Перевірка, чи можна активувати кнопку графіка ---
    int selectedParameters = 0;
    for (int i = 0; i < graphParameterChecks.size(); ++i) {
        if (graphParameterChecks[i]->isChecked())
            selectedParameters++;
    };

    bool plotReady = minReady && (selectedParameters == 2);

    if (plotButton)
        plotButton->setEnabled(plotReady);
}

QVector<QVector<double>> MainWindow::getMatrixFromTable(
    QTableWidget *table) const
{
    QVector<QVector<double>> matrix;
    for (int i = 0; i < table->rowCount(); ++i) {
        QVector<double> row;
        for (int j = 0; j < table->columnCount(); ++j) {
            QTableWidgetItem *item = table->item(i, j);
            if (!item || item->text().trimmed().isEmpty()) {
                row.append(0.0); // або NaN, або повідомлення про помилку
            } else {
                bool ok;
                double val = item->text().toDouble(&ok);
                row.append(ok ? val : 0.0);
            }
        }
        matrix.append(row);
    }
    return matrix;
}

void MainWindow::updateWeightsState()
{
    // Підрахунок обраних параметрів
    QVector<int> selectedIndexes = getSelectedParameterIndexes();

    // Якщо нічого не вибрано — активуємо всі поля
    bool enableAll = selectedIndexes.isEmpty();

    for (int i = 0; i < weightEdits.size(); ++i) {
        if (enableAll || selectedIndexes.contains(i)) {
            weightEdits[i]->setEnabled(true);
        } else {
            weightEdits[i]->setEnabled(false);
        }
    }
}

void MainWindow::initConnections()
{
    // 1. Чекбокси параметрів
    for (QCheckBox *check : parameterChecks) {
        if (check) {
            connect(check, &QCheckBox::stateChanged, this, [this]() {
                updateWeightsState();
                updateButtonsState(); // Якщо також потрібно оновити стан кнопок
            });
        }
    }

    for (QCheckBox *check : graphParameterChecks) {
        if (check) {
            connect(check, &QCheckBox::stateChanged, this, &MainWindow::updateButtonsState);
        }
    }

    // 2. Вагові коефіцієнти
    for (QLineEdit *edit : weightEdits) {
        if (edit) {
            connect(edit, &QLineEdit::textChanged, this, &MainWindow::updateButtonsState);
        }
    }

    // 3. Таблиці
    if (normalizedTable) {
        connect(normalizedTable, &QTableWidget::itemChanged, this, &MainWindow::updateButtonsState);
    }

    if (minimizedTable) {
        connect(minimizedTable, &QTableWidget::itemChanged, this, &MainWindow::updateButtonsState);
    }

    if (paretoTable) {
        connect(paretoTable, &QTableWidget::itemChanged, this, &MainWindow::updateButtonsState);
    }
}

void MainWindow::analyzeDominance()
{
    const QVector<QVector<double>> matrix = getMatrixFromTable(minimizedTable);

    // 1. Збір індексів обраних параметрів
    QVector<int> selectedCols = getSelectedParameterIndexes();

    // Якщо не вибрано — беремо всі
    if (selectedCols.isEmpty()) {
        for (int i = 0; i < parameterChecks.size(); ++i)
            selectedCols.append(i);
    }

    for (int row = 0; row < numAlternatives; ++row) {
        for (int col = 0; col < numCriteria; ++col) {
            QTableWidgetItem *item = new QTableWidgetItem;
            if (selectedCols.contains(col)) {
                double sourceItem = matrix[row][col];
                item->setText(QString::number(sourceItem, 'g', 3));
            } else {
                item->setText(""); // залишити порожнім
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                item->setBackground(QBrush(QColor(230, 230, 230))); // сірий для неактивних
            }
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            paretoTable->setItem(row, col, item);
        }
    }

    QVector<QPair<int, int>> worsePairs;
    QVector<int> excluded;
    for (int i = 0; i < matrix.size(); i++) {
        if (excluded.contains(i))
            continue;
        for (int j = 0; j < matrix.size(); j++) {
            if (i == j || excluded.contains(j))
                continue;

            bool dom = true;
            bool strictlyBetter = false;

            for (int col : selectedCols) {
                if (matrix[i][col] > matrix[j][col]) {
                    dom = false;
                    break;
                } else if (matrix[i][col] < matrix[j][col]) {
                    strictlyBetter = true;
                }
            }

            if (dom && strictlyBetter) {
                worsePairs.append({j, i});
                excluded.append(j);
            }
        }
    }

    const QColor rowColor = QColor(255, 200, 200);

    for (int i = 0; i < matrix.size(); ++i) {
        QTableWidgetItem *item = new QTableWidgetItem;

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

        if (excluded.contains(i)) {
            int colCount = paretoTable->columnCount();
            for (int j = 0; j < colCount; ++j) {
                if (item->text() != "ПО") {
                    QTableWidgetItem *cellItem = paretoTable->item(i, j);
                    cellItem->setBackground(QBrush(rowColor));
                }
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
    for (QComboBox *combo : minmaxCombos) {
        targets.append(combo->currentText());
    }
    return targets;
}

void MainWindow::fillNormalizedTable()
{
    // Зчитуємо матрицю зі значеннями з таблиці введення
    QVector<QVector<double>> matrix = getMatrixFromTable(inputTable);

    // Ініціалізуємо вектор для зберігання максимальних значень кожного параметра
    QVector<double> maxValues(numCriteria, 0.0);

    // Знаходимо максимальні значення для кожного параметра
    for (int j = 0; j < numCriteria; j++) {
        for (int i = 0; i < numAlternatives; i++) {
            if (matrix[i][j] > maxValues[j]) {
                maxValues[j] = matrix[i][j];
            }
        }
    }

    // Обчислюємо нормалізовані значення та заповнюємо таблицю
    for (int j = 0; j < numCriteria; j++) {
        for (int i = 0; i < numAlternatives; i++) {
            // Нормалізуємо значення (ділимо на максимальне для параметра)
            double normalized = matrix[i][j] / maxValues[j];

            // Створюємо новий елемент таблиці з результатом (форматування до 3 цифр)
            QTableWidgetItem *item = new QTableWidgetItem(QString::number(normalized, 'g', 3));

            // Робимо комірку нередагованою
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);

            // Встановлюємо комірку у відповідне місце в таблиці нормалізації
            normalizedTable->setItem(i, j, item);
        }
    }

    // Перемикаємо вкладку на таблицю з результатами нормалізації
    int index = tabWidget->indexOf(normalizedTable);
    tabWidget->setCurrentIndex(index);

    // Оновлюємо стан кнопок, зважаючи на те, що таблиця тепер заповнена
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

            QTableWidgetItem *item = new QTableWidgetItem(QString::number(value, 'g', 3));
            item->setFlags(item->flags() & ~Qt::ItemIsEditable); // Заборонити редагування
            minimizedTable->setItem(i, j, item);
        }
    }

    int index = tabWidget->indexOf(minimizedTable);
    tabWidget->setCurrentIndex(index);

    updateButtonsState();
}

bool MainWindow::validateWeightSum()
{
    bool anyInput = false;
    for (QLineEdit *edit : weightEdits) {
        if (!edit->isEnabled())
            continue;
        if (!edit->text().isEmpty()) {
            anyInput = true;
            break;
        }
    }

    // Якщо нічого не введено — не підсвічуємо
    if (!anyInput) {
        for (QLineEdit *edit : weightEdits)
            edit->setStyleSheet("");
        return false;
    }

    double sum = 0.0;
    bool allValid = true;

    for (QLineEdit *edit : weightEdits) {
        if (!edit->isEnabled())
            continue;

        bool ok;
        double val = edit->text().toDouble(&ok);
        if (!ok || edit->text().isEmpty()) {
            allValid = false;
            edit->setToolTip("Некоректне значення! Введіть число від 0 до 1.");
            continue;
        } else {
            edit->setToolTip("");
            sum += val;
        }
    }

    const double epsilon = 0.0001;
    bool sumError = (std::abs(sum - 1.0) > epsilon);

    for (QLineEdit *edit : weightEdits) {
        if (!edit->isEnabled())
            continue;
        if (sumError && allValid) {
            QString tip = QString("Сума всіх активних ваг: %1 (має бути 1.0)").arg(sum, 0, 'g', 3);
            edit->setToolTip(tip);
        } else if (!allValid && edit->toolTip().isEmpty()) {
            edit->setToolTip("");
        }
        edit->setStyleSheet((sumError || !allValid) ? "background-color: #ffcccc" : "");
    }

    return allValid && !sumError;
}

void MainWindow::clearLayout(
    QLayout *layout)
{
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }
}

void MainWindow::setupDoubleValidator(
    QLineEdit *edit)
{
    QRegularExpression regex(R"(^([0]|1|0[.,]\d{0,8}|1[.,]0{0,8})?$)");
    auto *validator = new QRegularExpressionValidator(regex, edit);
    edit->setValidator(validator);

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
}

void MainWindow::updateWeightInputs()
{
    qDeleteAll(weightEdits);
    weightEdits.clear();

    clearLayout(weightsLayout);

    int cols = numCriteria;

    // Визначаємо вибрані чекбокси
    int selectedCount = 0;
    for (QCheckBox *check : parameterChecks)
        if (check->isChecked())
            ++selectedCount;

    for (int i = 0; i < cols; ++i) {
        QLineEdit *edit = new QLineEdit(this);
        edit->setPlaceholderText(QString("П%1").arg(i + 1));

        setupDoubleValidator(edit);

        connect(edit, &QLineEdit::textChanged, this, [this]() {
            validateWeightSum();
            updateButtonsState(); // одразу оновлюємо кнопку, коли вводиться вага
        });

        edit->setFixedWidth(partWidth);

        // Встановлюємо активність поля згідно з чекбоксом
        if (selectedCount > 0)
            edit->setEnabled(parameterChecks[i]->isChecked());
        else
            edit->setEnabled(true); // якщо нічого не вибрано — всі активні

        weightEdits.append(edit);
        weightsLayout->addWidget(edit);
    }

    updateButtonsState();
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

void MainWindow::updateGraphParameterCheckboxes()
{
    QLayoutItem *child;
    while ((child = graphChecksLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }
    graphParameterChecks.clear();

    for (int i = 0; i < numCriteria; ++i) {
        QCheckBox *check = new QCheckBox(QString::number(i + 1), this);
        check->setFixedWidth(partWidth);
        graphParameterChecks.append(check);
        connect(check, &QCheckBox::stateChanged, this, &MainWindow::onParameterGraphCheckChanged);
        graphChecksLayout->addWidget(check);
    }
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
        check->setFixedWidth(partWidth);
        parameterChecks.append(check);
        connect(check, &QCheckBox::stateChanged, this, &MainWindow::onParameterCheckChanged);
        checksLayout->addWidget(check);
    }
}

void MainWindow::onParameterGraphCheckChanged(
    int)
{
    // Рахуємо кількість обраних
    int selectedCount = 0;
    for (QCheckBox *check : graphParameterChecks) {
        if (check->isChecked())
            ++selectedCount;
    }

    // Якщо обрано вже 2 — блокуємо інші (ті, що не обрані)
    for (QCheckBox *check : graphParameterChecks) {
        if (!check->isChecked())
            check->setEnabled(selectedCount < 2);
    }
}

void MainWindow::onParameterCheckChanged(
    int)
{
    // Рахуємо кількість обраних
    int selectedCount = 0;
    for (QCheckBox *check : parameterChecks) {
        if (check->isChecked())
            ++selectedCount;
    }
    updateWeightInputs();
}

void MainWindow::setupTable(
    QTableWidget *table, int cols, const QStringList &headers)
{
    table->clear();
    table->setRowCount(numAlternatives);
    table->setColumnCount(cols);
    table->setHorizontalHeaderLabels(headers);
}

void MainWindow::updateTableSize()
{
    numAlternatives = altSpin->value();
    numCriteria = critSpin->value();

    QStringList headers;
    for (int i = 0; i < numCriteria; ++i)
        headers << QString("Параметр %1").arg(i + 1);
    setupTable(inputTable, numCriteria, headers);
    setupTable(normalizedTable, numCriteria, headers);
    setupTable(minimizedTable, numCriteria, headers);
    headers << "Статус";
    setupTable(paretoTable, (numCriteria + 1), headers);
    setupTable(valueFunctionTable, 2, {"Розрахунок", "Ф. цінності"});
    setupTable(graphSummaryTable, 3, {"Параметр X", "Параметр Y", "Статус"});

    updateParameterCheckboxes();
    updateGraphParameterCheckboxes();
    initConnections();
    updateWeightInputs();

    qDeleteAll(minmaxCombos);
    clearLayout(minmaxLayout);
    minmaxCombos.clear();

    for (int i = 0; i < numCriteria; ++i) {
        QComboBox *combo = new QComboBox();
        combo->addItem("min");
        combo->addItem("max");
        combo->setCurrentText("min");
        combo->setFixedWidth(partWidth);
        minmaxCombos.append(combo);
        minmaxLayout->addWidget(combo);
    }
}

QVector<bool> MainWindow::getSelectedColumnsMask() const
{
    QVector<bool> selectedCols(numCriteria, false);
    bool anyChecked = false;

    for (int i = 0; i < parameterChecks.size(); ++i) {
        if (parameterChecks[i]->isChecked()) {
            selectedCols[i] = true;
            anyChecked = true;
        }
    }

    // Якщо нічого не вибрано — всі true
    if (!anyChecked) {
        selectedCols.fill(true);
    }

    return selectedCols;
}

QVector<double> MainWindow::getActiveWeights(
    const QVector<bool> &selectedCols) const
{
    QVector<double> activeWeights;
    for (int i = 0; i < weightEdits.size(); ++i) {
        if (!selectedCols[i])
            continue;

        QLineEdit *edit = weightEdits[i];
        if (!edit->isEnabled())
            continue;

        bool ok;
        double val = edit->text().toDouble(&ok);
        if (!ok)
            return {};
        activeWeights.append(val);
    }
    return activeWeights;
}

void MainWindow::selectSingleOption()
{
    valueFunctionTable->clearContents();

    const QVector<QVector<double>> matrix = getMatrixFromTable(minimizedTable);
    const QVector<bool> selectedCols = getSelectedColumnsMask();
    const QVector<double> activeWeights = getActiveWeights(selectedCols);

    if (activeWeights.isEmpty()) {
        QMessageBox::warning(this, "Помилка", "Некоректне значення ваги.");
        return;
    }

    int activeCount = std::count(selectedCols.begin(), selectedCols.end(), true);
    if (activeWeights.size() != activeCount) {
        QMessageBox::warning(this,
                             "Помилка",
                             "Кількість ваг не відповідає кількості вибраних критеріїв.");
        return;
    }

    for (int row = 0; row < matrix.size(); ++row) {
        QString status = paretoTable->item(row, paretoTable->columnCount() - 1)->text();
        if (status == "ПО") {
            double value = 0.0;
            QStringList formulaParts;
            int weightIdx = 0;
            for (int col = 0; col < selectedCols.size(); ++col) {
                if (!selectedCols[col])
                    continue;
                double criterion = matrix[row][col];
                double weight = activeWeights[weightIdx++];
                value += criterion * weight;
                formulaParts << QString::number(weight, 'g', 3) + "*"
                                    + QString::number(criterion, 'g', 3);
            }

            QString formula = formulaParts.join(" + ") + " = ";
            QTableWidgetItem *formulaItem = new QTableWidgetItem(formula);
            formulaItem->setFlags(formulaItem->flags() & ~Qt::ItemIsEditable);
            valueFunctionTable->setItem(row, 0, formulaItem);

            QTableWidgetItem *item = new QTableWidgetItem(QString::number(value, 'g', 4));
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            valueFunctionTable->setItem(row, 1, item);
        } else {
            const QColor rowColor = QColor(255, 200, 200);
            for (int j = 0; j < valueFunctionTable->columnCount(); ++j) {
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
    valueFunctionTable->resizeColumnToContents(0);

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
        item->setFlags(item->flags() & ~Qt::ItemIsEditable); // Заборонити редагування
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

    while (!in.atEnd() && row < inputTable->rowCount()) {
        QString line = in.readLine().trimmed();

        // Прибираємо все зайве, але залишаємо кому в числі
        // Замінимо табуляцію та крапку з комою на пробіл
        line.replace("\t", " ");
        line.replace(";", " ");
        line = line.simplified(); // усуває повторні пробіли

        // Тепер розділяємо рядок за пробілами
        QStringList values = line.split(' ', Qt::SkipEmptyParts);

        for (int col = 0; col < values.size() && col < inputTable->columnCount(); ++col) {
            QString value = values[col];
            value.replace(",", "."); // замінюємо кому на крапку, якщо вона була в дробі
            inputTable->setItem(row, col, new QTableWidgetItem(value));
        }

        ++row;
    }

    file.close();
}
