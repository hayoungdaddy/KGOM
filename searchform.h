#ifndef SEARCHFORM_H
#define SEARCHFORM_H

#include <QDialog>
#include <QDate>
#include <QButtonGroup>
#include <QDebug>

namespace Ui {
class SearchForm;
}

class SearchForm : public QDialog
{
    Q_OBJECT

public:
    explicit SearchForm(QWidget *parent = 0);
    ~SearchForm();

    QButtonGroup *magBG, *dateBG;

    void setCheckBox(int, int);

public slots:
    void setDateEdit(QDate, QDate);

private:
    Ui::SearchForm *ui;

private slots:
    void getSearchOptions();

signals:
    void sendSearchOptionsToMainWindow(int, double, double, int, int, int, QDate, QDate);
};

#endif // SEARCHFORM_H
