#ifndef MULTISTEPDIALOG_H
#define MULTISTEPDIALOG_H

#include <QDialog>

namespace Ui {
class MultiStepDialog;
}

class MultiStepDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MultiStepDialog(QWidget *parent = 0);
    ~MultiStepDialog();

signals:
    void NotifyMultiNext(int nIns);

private slots:
    void on_buttonBox_accepted();

private:
    Ui::MultiStepDialog *ui;
};

#endif // MULTISTEPDIALOG_H
