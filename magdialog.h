#ifndef MAGDIALOG_H
#define MAGDIALOG_H

#include <QDialog>

namespace Ui {
class MagDialog;
}

class MagDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MagDialog(QWidget *parent = 0);
    ~MagDialog();

private:
    Ui::MagDialog *ui;
};

#endif // MAGDIALOG_H
