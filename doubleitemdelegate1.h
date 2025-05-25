#ifndef DOUBLEITEMDELEGATE1_H
#define DOUBLEITEMDELEGATE1_H

#include <QStyledItemDelegate>

class DoubleItemDelegate1 : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit DoubleItemDelegate1(QObject *parent = nullptr);

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
