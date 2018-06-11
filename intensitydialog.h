#ifndef INTENSITYDIALOG_H
#define INTENSITYDIALOG_H

#include <QDialog>

namespace Ui {
class IntensityDialog;
}

class IntensityDialog : public QDialog
{
    Q_OBJECT

public:
    explicit IntensityDialog(QWidget *parent = 0);
    ~IntensityDialog();

private:
    Ui::IntensityDialog *ui;
};

#endif // INTENSITYDIALOG_H
