#include <QGraphicsView>
#include <QMainWindow>
#include <QVector>

class QTableWidget;
class QLineEdit;
class QCheckBox;
class QSpinBox;
class QHBoxLayout;
class QComboBox;
class QLabel;
class QPushButton;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void plotGraph();
    void loadMatrixFromFile();
    void selectSingleOption();
    void fillNormalizedTable();
    void fillMinimizedTable();
    void analyzeDominance();
    void highlightRowWithMinValue();
    bool validateTableData();
    void validateWeightSum();
    void updateButtonsState();
    void updateTableSize();
    void updateParameterCheckboxes();
    void updateWeightInputs();
    void drawGraphAxes(QGraphicsScene *scene);
    void onParameterCheckChanged(int);
    void initConnections();
    void updateWeightsState();

private:
    int numAlternatives = 0;
    int numCriteria = 0;
    const int margin = 30;
    const int width = 375;
    const int height = 375;
    const int partWidth = 94;
    QGraphicsView *graphView = nullptr;
    QGraphicsScene *graphScene = nullptr;
    QHBoxLayout *checksLayout = nullptr;
    QHBoxLayout *weightsLayout = nullptr;
    QHBoxLayout *optimizationLayout = nullptr;
    QLabel *weightErrorLabel = nullptr;
    QLineEdit *filePathEdit = nullptr;

    // Ваги
    QVector<QLineEdit *> weightEdits;

    // Параметри для графіка
    QVector<QCheckBox *> parameterChecks;
    QVector<QComboBox *> optimizationCombos;
    QSpinBox *altSpin = nullptr;
    QSpinBox *critSpin = nullptr;
    QTabWidget *tabWidget = nullptr;

    // Таблиці
    QTableWidget *inputTable = nullptr;
    QTableWidget *normalizedTable = nullptr;
    QTableWidget *minimizedTable = nullptr;
    QTableWidget *paretoTable = nullptr;
    QTableWidget *valueFunctionTable = nullptr;
    QTableWidget *graphSummaryTable = nullptr;

    // Кнопки
    QPushButton *fillNormButton = nullptr;
    QPushButton *fillMinButton = nullptr;
    QPushButton *analyzeDominanceButton = nullptr;
    QPushButton *singleOptionButton = nullptr;
    QPushButton *plotButton = nullptr;
    QWidget *tab = nullptr;

    QVector<double> getWeights() const;
    QVector<QString> getOptimizationTargets() const;
    QVector<QVector<double>> getMatrixFromTable(QTableWidget *table) const;
};

QVector<QVector<double>> normalizeMatrix(const QVector<QVector<double>> &matrix);
