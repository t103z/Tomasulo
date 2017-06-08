#ifndef ADDINSTDIALOG_H
#define ADDINSTDIALOG_H

#include <QDialog>

namespace Ui {
class AddInstDialog;
}

class AddInstDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddInstDialog(QWidget *parent = 0);
    ~AddInstDialog();

    void clearText();

signals:
    void NotifyAppendInst(const std::string &insStr);

private slots:
    void on_buttonBox_accepted();

private:
    Ui::AddInstDialog *ui;
};

#endif // ADDINSTDIALOG_H
