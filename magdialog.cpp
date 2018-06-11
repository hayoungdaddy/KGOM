#include "magdialog.h"
#include "ui_magdialog.h"

MagDialog::MagDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MagDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
}

MagDialog::~MagDialog()
{
    delete ui;
}
