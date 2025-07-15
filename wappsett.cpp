#include "wappsett.h"
#include "ui_wappsett.h"
#include <QtSerialPort/QSerialPortInfo>
#include <QList>

wAppsett::wAppsett(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::wAppsett)
{
    pWin = qobject_cast<Window *> (parent);

    if (!pWin) throw 985;

    ui->setupUi(this);
    //setLayout(ui->gridLayout);
    setLayout(ui->gridLayout_2);

    ui->tab_3->setLayout(ui->verticalLayout);
    ui->tab->setLayout(ui->gridLayout);
    ui->tab_2->setLayout(ui->gridLayout_3);
    ui->tab_7->setLayout(ui->gridLayout_5);

    ui->tabWidget->setCurrentWidget(ui->tab);//ui->tabWidget->findChild(QWidget, )

    ui->okBtn->setDefault(true);

    //ui->horizontalSpacer_20->
    //        >setSizePolicy
    //        (QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);

    connect(ui->okBtn, SIGNAL( clicked() ), SLOT( accept() ) );
    connect(ui->cancelBtn, SIGNAL( clicked() ), SLOT( reject() ) );

//    connect(ui->comboBox, SIGNAL(currentIndexChanged()), SLOT(comportChange()));

    e8Accepted = 1;

    fillComboBox();

    ui->stat0->setStyleSheet   ("QLabel {background-color: rgb(0,175,80); color: white}");
    ui->lab_cmd0->setStyleSheet("QLabel {background-color: rgb(0,175,80); color: white}");
    ui->stat1->setStyleSheet   ("QLabel {background-color: rgb(187,2,0); color: white}");
    ui->lab_cmd1->setStyleSheet("QLabel {background-color: rgb(187,2,0); color: white}");
    ui->stat2->setStyleSheet   ("QLabel {background-color: rgb(255,128,0); color: white}");
    ui->lab_cmd2->setStyleSheet("QLabel {background-color: rgb(255,128,0); color: white}");


//    ui->statSh0->setText(""); ui->statLn0->setText("");
//    ui->stat0->setText(_names[1] + "/" + _names[0]);

}

void wAppsett::comportChange(){
}

void wAppsett::setE(int _e1, int _e2, double _e3, int _e4, double _e5,
                    int _e6, int _e7, int _e8, int _e9, int _e10,
                    double _e11, int _e12, QList <QString> & _names, QList <QFont> & _fonts, QList <int> & _fontsMaxWidth,
                    QPainter* _FontPainter){

    FontPainter = _FontPainter;

    ui->e1->setText(QString::number(_e1));
    ui->e2->setText(QString::number(_e2));
    ui->e3->setText(QString::number(_e3,0,1));
    ui->e4->setText(QString::number(_e4));
    ui->e5->setText(QString::number(_e5,0,1));
    ui->e6->setText(QString::number(_e6));

    ui->e7->setText(QString::number(_e7));
    ui->checkUseRBdlit->setChecked(_e8);
    on_checkUseRBdlit_clicked(_e8);
    ui->e9->setText(QString::number(_e9));
    ui->e10->setText(QString::number(_e10));
    ui->e11->setText(QString::number(_e11, 0,1));
    ui->e12->setText(QString::number(_e12));

    //важно проинициализировать до установки текста
    NamesFonts = _fonts;
    NamesMaxWidth = _fontsMaxWidth;

    names = _names;

    //команды должны иниц. до статусов
    ui->lab_cmd0->setText(_names[6] + ":"); ui->cmd0->setText("");
    ui->lab_cmd1->setText(_names[7] + ":"); ui->cmd1->setText("");
    ui->lab_cmd2->setText(_names[8] + ":"); ui->cmd2->setText("");

    ui->statSh0->setText(""); ui->statLn0->setText("");
    ui->stat0->setText(_names[1] + "/" + _names[0] + ":");

    ui->statSh1->setText(""); ui->statLn1->setText("");
    ui->stat1->setText(_names[3] + "/" + _names[2] + ":");

    ui->statSh2->setText(""); ui->statLn2->setText("");
    ui->stat2->setText(_names[5] + "/" + _names[4] + ":");

    ui->lab_eg0->setText(_names[9]  + ":"); ui->eg0->setText("");
    ui->lab_eg1->setText(_names[10] + ":"); ui->eg1->setText("");
    ui->lab_eg2->setText(_names[11] + ":"); ui->eg2->setText("");
    ui->lab_eg3->setText(_names[12] + ":"); ui->eg3->setText("");
    ui->lab_eg4->setText(_names[13] + ":"); ui->eg4->setText("");

#ifdef Dbg
    ui->statLn0->setText(_names[1]);
#endif

}

void wAppsett::setStartIndicatorFading(bool b){
    ui->cStartIndicator->setChecked(b);
}

void wAppsett::getE(int& _e1, int& _e2, double& _e3, int& _e4, double& _e5, int& _e6, int &_e7,
                    int &_e8, int &_e9, int &_e10, double &_e11, int &_e12, QList <QString> &_names){
    _e1 = E1;
    _e2 = E2;
    _e3 = E3;
    _e4 = E4;
    _e5 = E5;
    _e6 = E6;
    _e7 = E7;
    _e8 = E8;
    _e9 = E9;
    _e10 = E10;
    _e11 = E11;
    _e12 = E12;

    names [ 0] = ui->statSh0->text()==""? names[ 0]:ui->statSh0->text();
    names [ 1] = ui->statLn0->text()==""? names[ 1]:ui->statLn0->text();
    names [ 2] = ui->statSh1->text()==""? names[ 2]:ui->statSh1->text();
    names [ 3] = ui->statLn1->text()==""? names[ 3]:ui->statLn1->text();
    names [ 4] = ui->statSh2->text()==""? names[ 4]:ui->statSh2->text();
    names [ 5] = ui->statLn2->text()==""? names[ 5]:ui->statLn2->text();

    names [ 6] = ui->cmd0->text()==""?    names[ 6]:ui->cmd0->text();
    names [ 7] = ui->cmd1->text()==""?    names[ 7]:ui->cmd1->text();
    names [ 8] = ui->cmd2->text()==""?    names[ 8]:ui->cmd2->text();

    names [ 9] = ui->eg0->text()==""?     names[ 9]: ui->eg0->text();
    names [10] = ui->eg1->text()==""?     names[10]: ui->eg1->text();
    names [11] = ui->eg2->text()==""?     names[11]: ui->eg2->text();
    names [12] = ui->eg3->text()==""?     names[12]: ui->eg3->text();
    names [13] = ui->eg4->text()==""?     names[13]: ui->eg4->text();

    _names = names;

}

void wAppsett::getStartIndicatorFading(bool& b){
    b = eSflag;
}

void wAppsett::getComPortNum(QString& a){
    a = ui->comboBox->itemText(ui->comboBox->currentIndex());
}

wAppsett::~wAppsett()
{
    delete ui;
}

void wAppsett::accept(){
    if (e1Accepted && e2Accepted && e3Accepted && e4Accepted && e5Accepted && e6Accepted && e7Accepted
                   && e8Accepted && e9Accepted && e10Accepted && e11Accepted) {
        pWin->wAboutprog->setAboutText(ui->textEdit->toPlainText());
        QDialog::accept();
    }
}

void wAppsett::on_e1_textChanged(const QString &arg1)
{
    pWin->TestInRange(10, 3000, E1, arg1, ui->e1, e1Accepted);
}

void wAppsett::on_e2_textChanged(const QString &arg1)
{
    pWin->TestInRange(1, 5, E2, arg1, ui->e2, e2Accepted);
}

void wAppsett::on_e3_textChanged(const QString &arg1)
{
    pWin->TestInRange(0.0, 10.0, E3, arg1, ui->e3, e3Accepted);
}

void wAppsett::on_e4_textChanged(const QString &arg1)
{
    pWin->TestInRange(1, 5, E4, arg1, ui->e4, e4Accepted);
}

void wAppsett::on_e5_textChanged(const QString &arg1)
{
    pWin->TestInRange(0.0, 10.0, E5, arg1, ui->e5, e5Accepted);
}

void wAppsett::on_e6_textChanged(const QString &arg1)
{
    pWin->TestInRange(10, 1000, E6, arg1, ui->e6, e6Accepted);
}


void wAppsett::on_e7_textChanged(const QString &arg1)
{
    pWin->TestInRange(1, 255, E7, arg1, ui->e7, e7Accepted);
}

void wAppsett::on_e9_textChanged(const QString &arg1)
{
    pWin->TestInRange(0, 255, E9, arg1, ui->e9, e9Accepted);
}


void wAppsett::on_checkUseRBdlit_clicked(bool checked)
{
    E8 = checked;
    e8Accepted = true;
    ui->label_18->setDisabled(!checked);
    ui->label_19->setDisabled(!checked);
    ui->e7->setDisabled(!checked);
}

bool wAppsett::isTextLong(QFont& textFont, QString  s, int maxwidth) {

    FontPainter->setFont(textFont);

    bool bold = textFont.bold();
    int sz = textFont.pixelSize();

    QFontMetrics fm(textFont);
    int width = fm.width(s);

    width = fm.boundingRect(s).width();//QFontMetrics(textFont).width(s);

    return width > maxwidth;
}

void wAppsett::on_cmd0_textChanged(const QString &arg1){
    ui->cmd0->setStyleSheet(QFontMetrics(NamesFonts[0]).width(arg1) > NamesMaxWidth[0]?"QLineEdit {background-color: yellow;}":"QLineEdit {background-color:white;}");
    if (arg1.endsWith("ТЬ", Qt::CaseInsensitive) && arg1.length()>=3) {
        QString status = arg1.left(arg1.length()-3);
        ui->statLn0->setText(status+"---");
        ui->statSh0->setText(status+".");
    } else {
        ui->statLn0->setText(arg1);
        ui->statSh0->setText(arg1);
    }
}
void wAppsett::on_cmd1_textChanged(const QString &arg1){
    ui->cmd1->setStyleSheet(QFontMetrics(NamesFonts[0]).width(arg1) > NamesMaxWidth[0]?"QLineEdit {background-color: yellow;}":"QLineEdit {background-color:white;}");
    if (arg1.endsWith("ТЬ", Qt::CaseInsensitive) && arg1.length()>=3) {
        QString status = arg1.left(arg1.length()-3);
        ui->statLn1->setText(status+"---");
        ui->statSh1->setText(status+".");
    } else {
        ui->statLn1->setText(arg1);
        ui->statSh1->setText(arg1);
    }
}
void wAppsett::on_cmd2_textChanged(const QString &arg1){
    ui->cmd2->setStyleSheet(QFontMetrics(NamesFonts[0]).width(arg1) > NamesMaxWidth[0]?"QLineEdit {background-color: yellow;}":"QLineEdit {background-color:white;}");
    if (arg1.endsWith("ТЬ", Qt::CaseInsensitive) && arg1.length()>=3) {
        QString status = arg1.left(arg1.length()-3);
        ui->statLn2->setText(status+"---");
        ui->statSh2->setText(status+".");
    } else {
        ui->statLn2->setText(arg1);
        ui->statSh2->setText(arg1);
    }
}

//                0                     1                        2                         3
//fonts << Vismo::commandFont << Vismo::fullStatusFont << Vismo::shortStatusFont << Vismo::pbGroupsFont;

void wAppsett::on_statLn0_textChanged(const QString &arg1){

#ifdef Dbg
    int z = NamesMaxWidth[1];
    ui->label_55->setText(QString::number(QFontMetrics(NamesFonts[1]).width(arg1))+" "+QString::number(NamesMaxWidth[1]));
#endif

    ui->statLn0->setStyleSheet(isTextLong(NamesFonts[1], arg1, NamesMaxWidth[1])?"QLineEdit {background-color: yellow;}":"QLineEdit {background-color:white;}");
}
void wAppsett::on_statLn1_textChanged(const QString &arg1){
    ui->statLn1->setStyleSheet(QFontMetrics(NamesFonts[1]).width(arg1) > NamesMaxWidth[1]?"QLineEdit {background-color: yellow;}":"QLineEdit {background-color:white;}");
}
void wAppsett::on_statLn2_textChanged(const QString &arg1){
    ui->statLn2->setStyleSheet(QFontMetrics(NamesFonts[1]).width(arg1) > NamesMaxWidth[1]?"QLineEdit {background-color: yellow;}":"QLineEdit {background-color:white;}");
}

void wAppsett::on_statSh0_textChanged(const QString &arg1){
    ui->statSh0->setStyleSheet(QFontMetrics(NamesFonts[2]).width(arg1) > NamesMaxWidth[2]?"QLineEdit {background-color: yellow;}":"QLineEdit {background-color:white;}");
}
void wAppsett::on_statSh1_textChanged(const QString &arg1){
    ui->statSh1->setStyleSheet(QFontMetrics(NamesFonts[2]).width(arg1) > NamesMaxWidth[2]?"QLineEdit {background-color: yellow;}":"QLineEdit {background-color:white;}");
}
void wAppsett::on_statSh2_textChanged(const QString &arg1){
    ui->statSh2->setStyleSheet(QFontMetrics(NamesFonts[2]).width(arg1) > NamesMaxWidth[2]?"QLineEdit {background-color: yellow;}":"QLineEdit {background-color:white;}");
}

void wAppsett::on_eg0_textChanged(const QString &arg1){
    ui->eg0->setStyleSheet(QFontMetrics(NamesFonts[3]).width(arg1) > NamesMaxWidth[3]?"QLineEdit {background-color: yellow;}":"QLineEdit {background-color:white;}");
}
void wAppsett::on_eg1_textChanged(const QString &arg1){
    ui->eg1->setStyleSheet(QFontMetrics(NamesFonts[3]).width(arg1) > NamesMaxWidth[3]?"QLineEdit {background-color: yellow;}":"QLineEdit {background-color:white;}");
}
void wAppsett::on_eg2_textChanged(const QString &arg1){
    ui->eg2->setStyleSheet(QFontMetrics(NamesFonts[3]).width(arg1) > NamesMaxWidth[3]?"QLineEdit {background-color: yellow;}":"QLineEdit {background-color:white;}");
}
void wAppsett::on_eg3_textChanged(const QString &arg1){
    ui->eg3->setStyleSheet(QFontMetrics(NamesFonts[3]).width(arg1) > NamesMaxWidth[3]?"QLineEdit {background-color: yellow;}":"QLineEdit {background-color:white;}");
}
void wAppsett::on_eg4_textChanged(const QString &arg1){
    ui->eg4->setStyleSheet(QFontMetrics(NamesFonts[3]).width(arg1) > NamesMaxWidth[3]?"QLineEdit {background-color: yellow;}":"QLineEdit {background-color:white;}");
}

void wAppsett::setFocusOnDefaultBtn(){
    ui->okBtn->setFocus();
}

void wAppsett::on_cStartIndicator_stateChanged(int arg1)
{
    eSflag = arg1;
}

/*void wAppsett::on_pushButton_3_clicked()
{
    pWin->admin->dialogAskPwd(QString("Изменить текущий пароль?"),false);
    if (pWin->admin->exec() == QDialog::Accepted) {
        pWin->admin->dialogSetPwd(false);
        if (pWin->admin->exec() == QDialog::Accepted) {
        }
    }
}*/

void wAppsett::on_pushButton_5_clicked()
{
    pWin->admin->dialogAskPwd(QString("Изменить текущий пароль при запуске программы?"));
    if (pWin->admin->exec() == QDialog::Accepted) {
        pWin->admin->dialogSetPwd();
        if (pWin->admin->exec() == QDialog::Accepted) {
        }
    }
}

void wAppsett::showEvent(QShowEvent * event){
    int x = pWin->geometry().x();
    int y = pWin->geometry().y();
    int w = pWin->geometry().width();
    int h = pWin->geometry().height();

    setGeometry(x + (w-geometry().width())/2, y + (h-geometry().height())/2,geometry().width(),geometry().height());

    ui->textEdit->setText(pWin->wAboutprog->getAboutText());
}

void wAppsett::on_launch_pwdEnbl_clicked(bool checked)
{
    if (ui->launch_pwdEnbl->isChecked()) {
        ui->launch_pwdEnbl->setChecked(!ui->launch_pwdEnbl->isChecked());
        pWin->admin->dialogAskPwd(QString("Отключить запрос пароля при запуске программы?"));
        if (pWin->admin->exec() == QDialog::Accepted){
            ui->launch_pwdEnbl->setChecked(!ui->launch_pwdEnbl->isChecked());
        }
    }
}
void wAppsett::setAdminPwdEnabled(int a){
    ui->launch_pwdEnbl->setChecked(a);
}

int  wAppsett::adminPwdEnabled(){
    return ui->launch_pwdEnbl->isChecked();
}

void wAppsett::fillComboBox(){

    ui->comboBox->clear();

    ui->comboBox->addItem("<не выбран>");

    QList<QSerialPortInfo> serialPortInfoList = QSerialPortInfo::availablePorts();
    foreach (const QSerialPortInfo &serialPortInfo, serialPortInfoList)
        ui->comboBox->addItem(serialPortInfo.portName());//+(serialPortInfo.isBusy()?" (занят)":""));

//    ui->comboBox->addItem("COMa");
//    ui->comboBox->addItem("COMb");
//    ui->comboBox->addItem("COMc");

    ui->comboBox->setCurrentIndex(0);
}

void wAppsett::setLogFName(QString s){
    ui->logFName->setText(s);
}

void wAppsett::setComPortName(QString s){
    savedComPortName = s;

    ui->comboBox->setCurrentIndex(0);

    for (int i=0; i<ui->comboBox->count(); i++)
        if (ui->comboBox->itemText(i) == s){
            ui->comboBox->setCurrentIndex(i);
            break;
        }
}

QString wAppsett::comPortName(int mode){
    if (mode == 0)
        return ui->comboBox->currentText().left(3)=="COM"? ui->comboBox->currentText():savedComPortName;
    else
        return ui->comboBox->currentText();
}

void wAppsett::on_pushButton_clicked()
{
    fillComboBox();
}


void wAppsett::on_cancelBtn_clicked()
{
}

void wAppsett::on_logfileWrEnabled_clicked()
{
}

bool wAppsett::getCheckLogWriteOn(){
    return ui->logfileWrEnabled->isChecked();
}

void wAppsett::setCheckLogWriteOn(bool a){
    ui->logfileWrEnabled->setChecked(a);
}

bool wAppsett::getValueLogWriteOn(){
    return ValueLogWriteOn;
}

void wAppsett::setValueLogWriteOn(bool a){
    ValueLogWriteOn = a;
}



void wAppsett::on_e10_textChanged(const QString &arg1)
{
    pWin->TestInRange(0.0, 120.0, E10, arg1, ui->e10, e10Accepted);
}

void wAppsett::on_e11_textChanged(const QString &arg1)
{
    pWin->TestInRange(0.0, 255.0, E11, arg1, ui->e11, e11Accepted);
}

void wAppsett::on_e12_textChanged(const QString &arg1)
{
    pWin->TestInRange(0.0, 1000, E12, arg1, ui->e12, e12Accepted);
}
