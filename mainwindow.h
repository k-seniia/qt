#include <QAbstractItemModel>   // Базовий клас для моделей даних
#include <QBrush>               // Кольори фону
#include <QCheckBox>            // Чекбокси для вибору параметрів
#include <QColor>               // Кольори
#include <QComboBox>            // Комбо-бокси для min/max
#include <QFile>                // Для роботи з файлами
#include <QFileDialog>          // Діалог вибору файлу
#include <QGraphicsScene>       // Сцена для графіка
#include <QGraphicsTextItem>    // Текст на графіку
#include <QGraphicsView>        // Відображення графіка
#include <QGroupBox>            // Групування віджетів
#include <QHBoxLayout>          // Горизонтальні контейнери
#include <QHeaderView>          // Налаштування заголовків таблиць
#include <QIODevice>            // Базовий клас для вводу/виводу
#include <QLabel>               // Текстові надписи
#include <QLayout>              // Базовий клас для layout'ів
#include <QLayoutItem>          // Елементи layout'ів
#include <QLineEdit>            // Поля для введення (наприклад, ваг)
#include <QMainWindow>          // Основне вікно
#include <QMessageBox>          // Вікно повідомлень
#include <QModelIndex>          // Індексація в таблицях/деревах
#include <QObject>              // Базовий клас для об’єктів з сигналами/слотами
#include <QPair>                // Пари (наприклад, для домінування)
#include <QPen>                 // Ручки для малювання графіків
#include <QPointF>              // 2D-точки для графіків
#include <QPushButton>          // Кнопки
#include <QRectF>               // Прямокутники у графіках
#include <QSpinBox>             // Спінбокси для введення чисел
#include <QString>              // Рядки
#include <QStringList>          // Списки рядків
#include <QStyleOptionViewItem> // Стилі для делегатів
#include <QTabWidget>           // Вкладки
#include <QTableWidget>         // Таблиці даних
#include <QTableWidgetItem>     // Елементи таблиць
#include <QTextStream>          // Зчитування та запис файлів
#include <QVBoxLayout>          // Вертикальні контейнери
#include <QVector>              // Вектори даних
#include <QWidget>              // Базовий віджет
#include <QtAlgorithms>         // qSort, qMin, qMax тощо

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    // Основні дії користувача
    void plotGraph();
    void analyzeDominance();
    void loadMatrixFromFile();
    void selectSingleOption();
    void fillMinimizedTable();
    void fillNormalizedTable();
    void highlightRowWithMinValue();

    // Перевірки та оновлення
    void updateTableSize();
    bool validateTableData();
    bool validateWeightSum();
    void updateButtonsState();
    void updateWeightInputs();
    void updateWeightsState();
    void updateParameterCheckboxes();
    void onParameterCheckChanged(int);
    void updateGraphParameterCheckboxes();
    void onParameterGraphCheckChanged(int);

    // Допоміжні
    void initConnections();
    void clearLayout(QLayout *layout);
    void drawGraphAxes(QGraphicsScene *scene);
    void setupDoubleValidator(QLineEdit *edit);
    void setupTable(QTableWidget *table, int cols, const QStringList &headers);

private:
    // Загальні налаштування
    int numAlternatives = 0; // Кількість альтернатив (рядків)
    int numParameters = 0;   // Кількість критеріїв (стовпців)
    const int margin = 30;
    const int width = 400;    // Ширина графіка
    const int height = 400;   // Висота графіка
    const int partWidth = 94; // Ширина віджетів (для комбо/чекбоксів)

    // Основні контейнери
    QWidget *tab = nullptr;
    QWidget *central = nullptr;

    // Вкладка з графіком
    QTabWidget *tabWidget = nullptr;

    // Графічні елементи
    QGraphicsView *graphView = nullptr;
    QGraphicsScene *graphScene = nullptr;

    // Головні Layout-и
    QVBoxLayout *mainLayout = nullptr;
    QVBoxLayout *graphChecksLayout = nullptr;
    QHBoxLayout *sizeLayout = nullptr;
    QHBoxLayout *fileLayout = nullptr;
    QHBoxLayout *graphLayout = nullptr;
    QHBoxLayout *inputLayout = nullptr;
    QHBoxLayout *checksLayout = nullptr;
    QHBoxLayout *minmaxLayout = nullptr;
    QHBoxLayout *buttonLayout = nullptr;
    QHBoxLayout *weightsLayout = nullptr;

    // Групи (рамки)
    QGroupBox *sizeContainer = nullptr;
    QGroupBox *fileContainer = nullptr;
    QGroupBox *minmaxContainer = nullptr;
    QGroupBox *weightContainer = nullptr;
    QGroupBox *checksContainer = nullptr;

    // Поля для ваг
    QVector<QLineEdit *> weightEdits;

    // Параметри та опції
    QVector<QComboBox *> minmaxCombos;         // комбо-бокси (min/max)
    QVector<QCheckBox *> parameterChecks;      // чекбокси для параметрів
    QVector<QCheckBox *> graphParameterChecks; // чекбокси для графіка

    // Віджети введення
    QSpinBox *altSpin = nullptr;
    QSpinBox *paramSpin = nullptr;
    QLineEdit *filePathEdit = nullptr;

    // Таблиці
    QTableWidget *inputTable = nullptr;
    QTableWidget *paretoTable = nullptr;
    QTableWidget *minimizedTable = nullptr;
    QTableWidget *normalizedTable = nullptr;
    QTableWidget *graphSummaryTable = nullptr;
    QTableWidget *valueFunctionTable = nullptr;

    // Кнопки
    QPushButton *plotButton = nullptr;
    QPushButton *loadButton = nullptr;
    QPushButton *resizeButton = nullptr;
    QPushButton *fillMinButton = nullptr;
    QPushButton *fillNormButton = nullptr;
    QPushButton *singleOptionButton = nullptr;
    QPushButton *analyzeDominanceButton = nullptr;

    // Допоміжні методи
    QVector<bool> getSelectedColumnsMask() const;
    bool isTableFilled(QTableWidget *table) const;
    QVector<QString> getOptimizationTargets() const;
    QVector<int> getSelectedParameterIndexes() const;
    bool isTableFilledOnlyForActiveColumns(QTableWidget *table) const;
    QVector<QVector<double>> getMatrixFromTable(QTableWidget *table) const;
    QVector<double> getActiveWeights(const QVector<bool> &selectedCols) const;
};
