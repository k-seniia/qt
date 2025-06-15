#include "doubleitemdelegate.h"

DoubleItemDelegate::DoubleItemDelegate(
    QObject *parent)
    : QStyledItemDelegate(parent)
{}

QWidget *DoubleItemDelegate::createEditor(
    QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const
{
    QLineEdit *editor = new QLineEdit(parent);

    // Дозволяє десяткові числа з крапкою або комою (макс 7 цифр до і 8 після крапки)
    QRegularExpression regex(R"(^-?\d{0,7}([.,]\d{0,8})?$)");
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(regex, editor);
    editor->setValidator(validator);

    // Автоматична заміна ком на крапки
    connect(editor, &QLineEdit::textChanged, editor, [editor]() {
        QString text = editor->text();
        if (text.contains(',')) {
            int cursorPos = editor->cursorPosition();
            text.replace(',', '.');
            editor->setText(text);
            editor->setCursorPosition(cursorPos); // Зберігаємо позицію курсору
        }
    });

    return editor;
}

void DoubleItemDelegate::setEditorData(
    QWidget *editor, const QModelIndex &index) const
{
    QString value = index.model()->data(index, Qt::EditRole).toString();
    QLineEdit *lineEdit = static_cast<QLineEdit *>(editor);
    lineEdit->setText(value);
}

void DoubleItemDelegate::setModelData(
    QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit *lineEdit = static_cast<QLineEdit *>(editor);
    QString value = lineEdit->text();

    // Заміна коми на крапку перед збереженням (як додатковий захист)
    value.replace(',', '.');

    model->setData(index, value, Qt::EditRole);
}

void DoubleItemDelegate::updateEditorGeometry(
    QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const
{
    editor->setGeometry(option.rect);
}
