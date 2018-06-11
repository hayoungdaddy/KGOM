#include "intensitydialog.h"
#include "ui_intensitydialog.h"

IntensityDialog::IntensityDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::IntensityDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
}

IntensityDialog::~IntensityDialog()
{
    delete ui;
}
