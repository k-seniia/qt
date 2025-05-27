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

private:
    int numAlternatives = 0;
    int numCriteria = 0;
    QGraphicsView *graphView;
    QGraphicsScene *graphScene;
    QHBoxLayout *checksLayout;
    QHBoxLayout *weightsLayout;
    QHBoxLayout *optimizationLayout;
    QLabel *weightErrorLabel;
    QLineEdit *filePathEdit;
    QVector<QLineEdit *> weightEdits;
    QVector<QCheckBox *> parameterChecks;
    QVector<QComboBox *> optimizationCombos;
    QSpinBox *altSpin;
    QSpinBox *critSpin;
    QTabWidget *tabWidget;
    QTableWidget *inputTable;
    QTableWidget *normalizedTable;
    QTableWidget *minimizedTable;
    QTableWidget *paretoTable;
    QTableWidget *valueFunctionTable;
    QTableWidget *graphSummaryTable;
    QPushButton *fillNormButton = nullptr;
    QPushButton *fillMinButton = nullptr;
    QPushButton *analyzeDominanceButton = nullptr;
    QPushButton *singleOptionButton = nullptr;

    QVector<double> getWeights() const;
    QVector<QString> getOptimizationTargets() const;
    QVector<QVector<double>> getMatrixFromTable(QTableWidget *table) const;
};

bool dominates(const QVector<double> &a, const QVector<double> &b);
QVector<QVector<double>> normalizeMatrix(const QVector<QVector<double>> &matrix);
