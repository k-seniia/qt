#ifndef DOUBLEITEMDELEGATE_H
#define DOUBLEITEMDELEGATE_H

#include <QLineEdit>                   // Поля для введення (наприклад, ваг)
#include <QRegularExpression>          // Шаблони для валідації
#include <QRegularExpressionValidator> // Валідація введення
#include <QStyledItemDelegate>         // Клас делегата для обробки введення в QTableWidget

class DoubleItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit DoubleItemDelegate(QObject *parent = nullptr);

    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor,
                      QAbstractItemModel *model,
                      const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const override;
};

#endif // DOUBLEITEMDELEGATE_H
