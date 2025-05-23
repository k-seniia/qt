#include <QMainWindow>
#include <QVector>

class QTableWidget;
class QLineEdit;
class QCheckBox;
class QSpinBox;
class QHBoxLayout;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void loadMatrixFromFile();
    void plotGraph();
    void runParetoOptimization();
    void selectSingleOption();
    void updateTableSize();
    void updateParameterCheckboxes();
    bool validateTableData();
    QVector<QVector<double>> getMatrixFromTable();
    QVector<double> getWeights();
    void updateWeightInputs();
    void fillNormalizedTable();
    void fillMinimizedTable();

private:
    QTableWidget *inputTable;
    QLineEdit *filePathEdit;
    QVector<QCheckBox *> parameterChecks;
    QSpinBox *altSpin;
    QSpinBox *critSpin;
    QHBoxLayout *checksLayout;
    int numAlternatives = 0;
    int numCriteria = 0;
    QVector<QLineEdit *> weightEdits;
    QHBoxLayout *weightsLayout;
    QTabWidget *tabWidget;
    QTableWidget *normalizedTable;
    QTableWidget *minimizedTable;
};
