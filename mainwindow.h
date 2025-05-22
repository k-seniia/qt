#include <QMainWindow>
#include <QVector>

class QTableWidget;
class QLineEdit;
class QCheckBox;

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

private:
    QTableWidget *table;
    QLineEdit *filePathEdit;
    QVector<QCheckBox *> parameterChecks;
    int numAlternatives = 0;
    int numCriteria = 0;
};
