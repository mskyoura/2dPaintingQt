#include "wlog.h"
#include "ui_wlog.h"

wLog::wLog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::wLog)
{

    //pWin = qobject_cast<Window *> (parent);

    ui->setupUi(this);
    setLayout(ui->gridLayout);

    ui->pushButton->setDefault(true);

    NextLogRowsColor = false;

    connect(ui->pushButton,     SIGNAL( clicked() ), SLOT( accept() ) );

    setWindowTitle("Лог команд");

   // ui->tableWidget->setVisible(0);
}

wLog::~wLog()
{
    delete ui;
}

void wLog::setNextLogRowsColor(){
    NextLogRowsColor = !NextLogRowsColor;
}

QColor wLog::logRowsColor(){
    return NextLogRowsColor? QColor(220,220,220):QColor(255,255,255);
}

QTableWidget* wLog::table() {
    return ui->tableWidget;
}

void wLog::on_pushButton_2_clicked()
{
    //Раньше была кнопка Очистить
    //ui->tableWidget->setRowCount(0);

    //ui->textEdit-> (pWin->getLogFName)
}

void wLog::setFocusOnDefaultBtn(){
    ui->pushButton->setFocus();
}

void wLog::showEvent(QShowEvent * event){

    int x = parentWidget()->geometry().x();
    int y = parentWidget()->geometry().y();
    int w = parentWidget()->geometry().width();
    int h = parentWidget()->geometry().height();

    setGeometry(geometry().x(),geometry().y(),w*0.95,geometry().height());
    setGeometry(x + (w-geometry().width())/2, y + (h-geometry().height())/2,geometry().width(),geometry().height());
}
