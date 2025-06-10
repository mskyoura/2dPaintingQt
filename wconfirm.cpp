#include "wconfirm.h"
#include "ui_wconfirm.h"

wConfirm::wConfirm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::wConfirm)
{
    ui->setupUi(this);

    setWindowFlags (windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowModality(Qt::ApplicationModal);
    ui->okBtn->setDefault(true);

    connect(ui->okBtn,     SIGNAL( clicked() ), SLOT( accept() ) );
    connect(ui->cancelBtn, SIGNAL( clicked() ), SLOT( reject() ) );

    setLayout(ui->gridLayout);
    setWindowTitle("Подтверждение");

}

wConfirm::~wConfirm() {
    delete ui;
}

int wConfirm::showConfirm(QString s) {
    ui->label->setText(s);
    return exec();
}


void wConfirm::showEvent(QShowEvent * event){

    int x = parentWidget()->geometry().x();
    int y = parentWidget()->geometry().y();
    int w = parentWidget()->geometry().width();
    int h = parentWidget()->geometry().height();

    setGeometry(x + (w-geometry().width())/2, y + (h-geometry().height())/2,geometry().width(),geometry().height());
}

void wConfirm::accept() {
    QDialog::accept();
}

void wConfirm::reject() {
    QDialog::reject();
}

