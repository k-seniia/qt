#include "doubleitemdelegate1.h"
#include <QDebug>
#include <QLineEdit>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

DoubleItemDelegate1::DoubleItemDelegate1(
    QObject *parent)
    : QStyledItemDelegate(parent)
{}

QWidget *DoubleItemDelegate1::createEditor(
    QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const
{
    QLineEdit *editor = new QLineEdit(parent);

    // Дозволяє десяткові числа з крапкою або комою
    QRegularExpression regex(R"(^([0]|1|0[.,]\d{0,8}|1[.,]0{0,8})?$)");
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(regex, editor);
    editor->setValidator(validator);

    // Автоматична заміна ком на крапки
    connect(editor, &QLineEdit::textChanged, editor, [editor]() {
        QString text = editor->text();
        if (text.contains(',')) {
            int cursorPos = editor->cursorPosition();
            text.replace(',', '.');
            editor->setText(text);
            editor->setCursorPosition(cursorPos);
        }
    });

    return editor;
}

void DoubleItemDelegate1::setEditorData(
    QWidget *editor, const QModelIndex &index) const
{
    QString value = index.model()->data(index, Qt::EditRole).toString();
    QLineEdit *lineEdit = static_cast<QLineEdit *>(editor);
    lineEdit->setText(value);
}

void DoubleItemDelegate1::setModelData(
    QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit *lineEdit = static_cast<QLineEdit *>(editor);
    QString value = lineEdit->text();
    value.replace(',', '.');

    bool ok;
    double number = value.toDouble(&ok);

    // Додаткова перевірка діапазону
    if (ok && number >= 0.0 && number <= 1.0) {
        model->setData(index, value, Qt::EditRole);
    } else {
        model->setData(index, "", Qt::EditRole); // або нічого не змінюємо
    }
}

void DoubleItemDelegate1::updateEditorGeometry(
    QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const
{
    editor->setGeometry(option.rect);
}
