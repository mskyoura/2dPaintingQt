#include "wabout.h"
#include "ui_wabout.h"

wAbout::wAbout(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::wAbout)
{
    ui->setupUi(this);

    ui->okBtn->setDefault(true);

    connect( ui->okBtn,     SIGNAL( clicked() ), SLOT( accept() ) );
//    connect( ui->cancelBtn, SIGNAL( clicked() ), SLOT( reject() ) );

    setLayout(ui->gridLayout);

    setWindowTitle("О программе");

}

wAbout::~wAbout()
{
    delete ui;
}


void wAbout::setAboutText(QString s){
    ui->textEdit->setText(s);
}

QString wAbout::getAboutText(){
    return ui->textEdit->toPlainText();
}
