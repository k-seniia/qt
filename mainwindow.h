#include <QCheckBox>
#include <QComboBox>
#include <QFile>
#include <QFileDialog>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QTextStream>
#include <QVBoxLayout>
#include <QVector>
#include <QWidget>

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
    int numCriteria = 0;     // Кількість критеріїв (стовпців)
    const int margin = 30;
    const int width = 400;    // Ширина графіка
    const int height = 400;   // Висота графіка
    const int partWidth = 94; // Ширина віджетів (для комбо/чекбоксів)

    // Основні контейнери
    QWidget *tab = nullptr;
    QWidget *central = nullptr;

    // Графічні елементи
    QGraphicsView *graphView = nullptr;
    QGraphicsScene *graphScene = nullptr;

    // Головні Layout-и
    QVBoxLayout *mainLayout = nullptr;
    QVBoxLayout *graphChecksLayout = nullptr;
    QHBoxLayout *sizeLayout = nullptr;
    QHBoxLayout *fileLayout = nullptr;
    QHBoxLayout *inputLayout = nullptr;
    QHBoxLayout *checksLayout = nullptr;
    QHBoxLayout *minmaxLayout = nullptr;
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
    QSpinBox *critSpin = nullptr;
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

    // Вкладка з графіком
    QTabWidget *tabWidget = nullptr;

    // Допоміжні методи
    QVector<bool> getSelectedColumnsMask() const;
    bool isTableFilled(QTableWidget *table) const;
    QVector<QString> getOptimizationTargets() const;
    QVector<int> getSelectedParameterIndexes() const;
    bool isTableFilledOnlyForActiveColumns(QTableWidget *table) const;
    QVector<QVector<double>> getMatrixFromTable(QTableWidget *table) const;
    QVector<double> getActiveWeights(const QVector<bool> &selectedCols) const;
};
