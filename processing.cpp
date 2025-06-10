#include "processing.h"
#include "ui_processing.h"

Processing::Processing(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Processing)
{
    ui->setupUi(this);

    Parent = parent;

    ui->cancelBtn->setDefault(true);
    connect( ui->cancelBtn, SIGNAL( clicked() ), SLOT( reject() ) );
    setLayout(ui->gridLayout);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    _wasCancelled = false;
}

Processing::~Processing()
{
    delete ui;
}

void Processing::showEvent( QShowEvent * event ){
    _wasCancelled = false;
    setGeometry((Parent->width()-width())/2.0 + Parent->x(),
                (Parent->height()-height())/2.0 + Parent->y(),
                width(), height());
}

void Processing::setText(QString s){
    ui->label->setText(s);
    update();
}

bool Processing::wasCancelled(){
    return _wasCancelled;
}

void Processing::setProgress(int percent){
    ui->progressBar->setValue(percent);
    update();
}

void Processing::reject(){
    _wasCancelled = true;
}
