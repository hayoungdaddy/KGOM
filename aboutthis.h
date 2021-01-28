#ifndef ABOUTTHIS_H
#define ABOUTTHIS_H

#include <QDialog>

namespace Ui {
class AboutThis;
}

class AboutThis : public QDialog
{
    Q_OBJECT

public:
    explicit AboutThis(QString logDir = 0, QWidget *parent = 0);
    ~AboutThis();

private:
    Ui::AboutThis *ui;
};

#endif // ABOUTTHIS_H
