#include "wwarning.h"
#include "ui_wwarning.h"

wWarning::wWarning(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::wWarning)
{
    ui->setupUi(this);

    setWindowFlags (windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowModality(Qt::ApplicationModal);
    ui->okBtn->setDefault(true);
    connect(ui->okBtn, SIGNAL( clicked() ), SLOT( accept() ) );
    setLayout(ui->gridLayout);
    setWindowTitle("Предупреждение");

}

wWarning::~wWarning()
{
    delete ui;
}

void wWarning::showWarning(QString s) {

    ui->label->setText(s);
    show();

}


void wWarning::showEvent(QShowEvent * event){

    int x = parentWidget()->geometry().x();
    int y = parentWidget()->geometry().y();
    int w = parentWidget()->geometry().width();
    int h = parentWidget()->geometry().height();

    setGeometry(x + (w-geometry().width())/2, y + (h-geometry().height())/2,geometry().width(),geometry().height());
}
