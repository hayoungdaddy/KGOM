#include "searchform.h"
#include "ui_searchform.h"

SearchForm::SearchForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SearchForm)
{
    ui->setupUi(this);

    magBG = new QButtonGroup(this);
    magBG->addButton(ui->mag1CB); magBG->setId(ui->mag1CB, 0);
    magBG->addButton(ui->mag2CB); magBG->setId(ui->mag2CB, 1);
    magBG->addButton(ui->mag3CB); magBG->setId(ui->mag3CB, 2);
    magBG->addButton(ui->mag4CB); magBG->setId(ui->mag4CB, 3);
    magBG->addButton(ui->mag5CB); magBG->setId(ui->mag5CB, 4);
    magBG->addButton(ui->mag6CB); magBG->setId(ui->mag6CB, 5);

    dateBG = new QButtonGroup(this);
    dateBG->addButton(ui->date1CB); dateBG->setId(ui->date1CB, 0);
    dateBG->addButton(ui->date2CB); dateBG->setId(ui->date2CB, 1);
    dateBG->addButton(ui->date3CB); dateBG->setId(ui->date3CB, 2);

    ui->endDE->setDate(QDate::currentDate());
    ui->startDE->setDate(QDate::currentDate().addDays(-30));

    connect(ui->searchPB, SIGNAL(clicked(bool)), this, SLOT(getSearchOptions()));
}

SearchForm::~SearchForm()
{
    delete ui;
}

void SearchForm::setCheckBox(int dateIndex, int magIndex)
{
    if(dateIndex == 0) ui->date1CB->setChecked(true);
    else if(dateIndex == 1) ui->date2CB->setChecked(true);
    else if(dateIndex == 2) ui->date3CB->setChecked(true);

    if(magIndex == 0) ui->mag1CB->setChecked(true);
    else if(magIndex == 1) ui->mag2CB->setChecked(true);
    else if(magIndex == 2) ui->mag3CB->setChecked(true);
    else if(magIndex == 3) ui->mag4CB->setChecked(true);
    else if(magIndex == 4) ui->mag5CB->setChecked(true);
    else if(magIndex == 5) ui->mag6CB->setChecked(true);
}

void SearchForm::setDateEdit(QDate startD, QDate endD)
{
    ui->startDE->setDate(startD);
    ui->endDE->setDate(endD);
}

void SearchForm::getSearchOptions()
{
    int magIndex = 0, dateIndex = 0, nEvents = 10, nDays = 10;
    double minMag = 0, maxMag = 0;
    QDate sDate, eDate;

    nEvents = ui->nEventsSB->value();
    nDays = ui->nDaysSB->value();
    sDate = ui->startDE->date();
    eDate = ui->endDE->date();
    minMag = ui->sMagLE->text().toDouble();
    maxMag = ui->eMagLE->text().toDouble();

    magIndex = magBG->checkedId();
    dateIndex = dateBG->checkedId();

    emit sendSearchOptionsToMainWindow(magIndex, minMag, maxMag, dateIndex, nEvents, nDays, sDate, eDate);
    accept();
}
