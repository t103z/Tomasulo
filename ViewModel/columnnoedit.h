#ifndef TOMASULO_COLUMNNOEDIT_H
#define TOMASULO_COLUMNNOEDIT_H

#include <QItemDelegate>

class NoEditableDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    explicit NoEditableDelegate(QObject *parent = 0): QItemDelegate(parent) {}

protected:
    bool editorEvent(QEvent *, QAbstractItemModel *, const QStyleOptionViewItem &,
                     const QModelIndex &)
    {
        return false;
    }

    QWidget *createEditor(QWidget *, const QStyleOptionViewItem &, const QModelIndex &) const
    {
        return Q_NULLPTR;
    }
};

#endif // TOMASULO_COLUMNNOEDIT_H
