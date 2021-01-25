#include "aboutthis.h"
#include "ui_aboutthis.h"

#include <QImage>
#include <QPixmap>

AboutThis::AboutThis(QString homeDir, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutThis)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Drawer);

    QImage *img = new QImage();
    QPixmap *pix = new QPixmap();

    img->load(homeDir + "/params/images/PCIcon.png");
    *pix = QPixmap::fromImage(*img);
    *pix = pix->scaled(200, 200);
    ui->iconLB->setPixmap(*pix);

    img->load(homeDir + "/params/images/kigam.png");
    *pix = QPixmap::fromImage(*img);
    *pix = pix->scaled(350, 60);
    ui->ciLB->setPixmap(*pix);
}

AboutThis::~AboutThis()
{
    delete ui;
}
