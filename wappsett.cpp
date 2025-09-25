#include "wappsett.h"
#include "ui_wappsett.h"
#include <QtSerialPort/QSerialPortInfo>
#include <QList>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QTabBar>
#include <QLayout>
#include <QFontDatabase>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QTimer>
#include <QDesktopWidget>
#include <QLineEdit>
#include <QLabel>

#ifndef QT_VERSION_CHECK
#define QT_VERSION_CHECK(major, minor, patch) ((major<<16)|(minor<<8)|(patch))
#endif

// Centralized sizing/scaling constants for Settings dialog
namespace {
    // Base logical size the layout was designed for
    const int WAPPSETT_BASE_W = 1000;   // px
    const int WAPPSETT_BASE_H = 750;    // px

    // Margin kept from screen edges when sizing/centering
    const int WAPPSETT_SCREEN_MARGIN = 20; // px

    // Limits for proportional UI scale factor
    const double WAPPSETT_MIN_SCALE = 0.3;
    const double WAPPSETT_MAX_SCALE = 4.5;
}

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
    // Re-wrap the "Основные" tab so content hugs the top without large gaps
    {
        QVBoxLayout* topLayout = new QVBoxLayout();
        topLayout->setContentsMargins(6, 6, 6, 6);
        topLayout->setSpacing(6);
        topLayout->setAlignment(Qt::AlignTop);
        ui->tab->setLayout(topLayout);
        if (ui->gridLayoutWidget) {
            ui->gridLayoutWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
            ui->gridLayoutWidget->move(0, 0);
            ui->gridLayoutWidget->setMinimumHeight(0);
            topLayout->addWidget(ui->gridLayoutWidget, 0, Qt::AlignTop);
        }
        topLayout->addStretch(1);
    }
    ui->tab_2->setLayout(ui->gridLayout_3);
    ui->tab_7->setLayout(ui->gridLayout_5);

    // Use native pages everywhere to avoid blank tabs
    QTabWidget* tw = ui->tabWidget;
    tw->setMinimumSize(WAPPSETT_BASE_W, WAPPSETT_BASE_H);
    tw->setCurrentIndex(0);

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

    // compact layout tweeks for 650px height: shrink spacers and margins, reduce control sizes
    applyCompactLayout();
    // initial scale pass after UI laid out (Qt5 only)
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
    ui->e13->setText(QString::number(pWin->Usb->_sendTimeoutMs()));

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
    if (e13Accepted) pWin->Usb->setSendTimeoutMs(E13);

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

void wAppsett::on_e13_textChanged(const QString &arg1)
{
    pWin->TestInRange(10, 5000, E13, arg1, ui->e13, e13Accepted);
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
    Q_UNUSED(event);
    // Base reference size
    const double baseW = (double)WAPPSETT_BASE_W;
    const double baseH = (double)WAPPSETT_BASE_H;
    QRect avail = QApplication::desktop()->availableGeometry(this);
    // Choose a target size up to base while respecting available geometry
    int targetW = qMin((int)baseW, avail.width()  - WAPPSETT_SCREEN_MARGIN);
    int targetH = qMin((int)baseH, avail.height() - WAPPSETT_SCREEN_MARGIN);
    // Compute proportional scale relative to base to scale fonts/margins
    double scaleW = (double)targetW / baseW;
    double scaleH = (double)targetH / baseH;
    double scale = qBound(WAPPSETT_MIN_SCALE, qMin(scaleW, scaleH), WAPPSETT_MAX_SCALE);

    resize(targetW, targetH);
    int nx = avail.x() + (avail.width()  - targetW)/2;
    int ny = avail.y() + (avail.height() - targetH)/2;
    move(nx, ny);

    applyProportionalLayoutScale(scale);

    ui->textEdit->setText(pWin->wAboutprog->getAboutText());

    // Reserve space for footer buttons so they never go off-screen
    if (ui && ui->tabWidget && ui->okBtn && ui->cancelBtn) {
        int footerH = qMax(ui->okBtn->sizeHint().height(), ui->cancelBtn->sizeHint().height()) + 22; // paddings
        int availH  = height() - footerH - 16; // top/bottom margins
        if (availH > 200) {
            ui->tabWidget->setMinimumHeight(availH);
            ui->tabWidget->setMaximumHeight(availH);
        }
    }

    // Native pages only; no viewbox scaling
}

void wAppsett::resizeEvent(QResizeEvent* event)
{
    QDialog::resizeEvent(event);
    int minW = WAPPSETT_BASE_W;
    int maxH = WAPPSETT_BASE_H;
    int newW = qMax(width(), minW);
    int newH = qMin(height(), maxH);
    // Proportional scale based on reference base size (scale up or down)
    const double baseW = (double)WAPPSETT_BASE_W;
    const double baseH = (double)WAPPSETT_BASE_H;
    double scaleW = (double)newW / baseW;
    double scaleH = (double)newH / baseH;
    double scale = qBound(WAPPSETT_MIN_SCALE, qMin(scaleW, scaleH), WAPPSETT_MAX_SCALE);
    applyProportionalLayoutScale(scale);
    if (newW != width() || newH != height()) resize(newW, newH);

    // Keep footer visible by constraining tab height
    if (ui && ui->tabWidget && ui->okBtn && ui->cancelBtn) {
        int footerH = qMax(ui->okBtn->height(), ui->cancelBtn->height()) + 22;
        int availH  = height() - footerH - 16;
        if (availH > 200) {
            ui->tabWidget->setMinimumHeight(availH);
            ui->tabWidget->setMaximumHeight(availH);
        }
    }

    rescaleViewBoxes();
}

void wAppsett::applyCompactLayout()
{
    // Reduce margins on major layouts
    if (ui && ui->gridLayout_2) ui->gridLayout_2->setContentsMargins(6, 6, 6, 6);
    if (ui && ui->gridLayout)   {
        ui->gridLayout->setContentsMargins(6, 6, 6, 6);
        ui->gridLayout->setAlignment(Qt::AlignTop);
        // Ensure rows above controls don't stretch, push content to top
        for (int r = 0; r < ui->gridLayout->rowCount(); ++r) ui->gridLayout->setRowStretch(r, 0);
        ui->gridLayout->setRowStretch(ui->gridLayout->rowCount()-1, 1);
    }
    if (ui && ui->gridLayout_3) ui->gridLayout_3->setContentsMargins(6, 6, 6, 6);
    if (ui && ui->gridLayout_5) ui->gridLayout_5->setContentsMargins(6, 6, 6, 6);

    // Decrease vertical spacers heights to pack content tighter
    // Basic tab
    if (ui->verticalSpacer)      ui->verticalSpacer->changeSize(0, 0, QSizePolicy::Fixed,   QSizePolicy::Fixed);
    if (ui->verticalSpacer_2)    ui->verticalSpacer_2->changeSize(0, 0, QSizePolicy::Fixed,   QSizePolicy::Fixed);
    if (ui->verticalSpacer_5)    ui->verticalSpacer_5->changeSize(0, 0, QSizePolicy::Fixed,   QSizePolicy::Fixed);
    if (ui->verticalSpacer_10)   ui->verticalSpacer_10->changeSize(0, 0, QSizePolicy::Fixed,   QSizePolicy::Fixed);

    // Commands tab
    if (ui->verticalSpacer_4)    ui->verticalSpacer_4->changeSize(0, 0,  QSizePolicy::Fixed,   QSizePolicy::Fixed);
    if (ui->verticalSpacer_3)    ui->verticalSpacer_3->changeSize(0, 0,  QSizePolicy::Fixed,   QSizePolicy::Fixed);

    // Names tab
    if (ui->verticalSpacer_11)   ui->verticalSpacer_11->changeSize(0, 0,  QSizePolicy::Fixed,   QSizePolicy::Fixed);

    // Additional tab
    if (ui->verticalSpacer_7)    ui->verticalSpacer_7->changeSize(20, 6,  QSizePolicy::Minimum, QSizePolicy::Fixed);
    if (ui->verticalSpacer_8)    ui->verticalSpacer_8->changeSize(20, 6,  QSizePolicy::Minimum, QSizePolicy::Fixed);
    if (ui->verticalSpacer_9)    ui->verticalSpacer_9->changeSize(20, 6,  QSizePolicy::Minimum, QSizePolicy::Fixed);
}

void wAppsett::applyFontScale(double scale)
{
    if (!ui) return;
    // Scale fonts on key controls to fit vertically, preserve readability
    auto scaleWidgetFont = [scale](QWidget* w){
        if (!w) return;
        QFont f = w->font();
        // use a saved base font size to avoid cumulative scaling
        bool ok = false;
        int basePt = w->property("__basePointSize").toInt(&ok);
        if (!ok || basePt <= 0) {
            basePt = f.pointSize() > 0 ? f.pointSize() : 12;
            w->setProperty("__basePointSize", basePt);
        }
        int newPt = qMax(9, (int)qRound(basePt * scale));
        int currentPt = f.pointSize() > 0 ? f.pointSize() : basePt;
        if (newPt != currentPt) {
            f.setPointSize(newPt);
            w->setFont(f);
        }
    };

    // Tab widget
    scaleWidgetFont(ui->tabWidget);

    // Basic tab
    QStringList basicLabels = { "label_2", "label_3", "label_4" };
    for (const QString& name : basicLabels) scaleWidgetFont(ui->gridLayoutWidget->findChild<QWidget*>(name));

    // Commands tab: labels and edits
    QWidget* cmds[] = { ui->label, ui->label_5, ui->label_7, ui->label_6, ui->label_9, ui->label_11,
                        ui->label_15, ui->label_21, ui->label_25, ui->label_24, ui->label_27,
                        ui->label_12, ui->label_13, ui->label_14, ui->label_16, ui->label_19, ui->label_20,
                        ui->label_22, ui->label_23, ui->label_sendtimeout, ui->label_sendtimeout_units,
                        ui->e1, ui->e2, ui->e3, ui->e4, ui->e5, ui->e6, ui->e7, ui->e9, ui->e10, ui->e11, ui->e12, ui->e13 };
    for (QWidget* w : cmds) scaleWidgetFont(w);

    // Names tab
    QWidget* names[] = { ui->label_47, ui->label_55, ui->label_61,
                         ui->stat0, ui->stat1, ui->stat2, ui->lab_cmd0, ui->lab_cmd1, ui->lab_cmd2,
                         ui->statLn0, ui->statLn1, ui->statLn2, ui->statSh0, ui->statSh1, ui->statSh2,
                         ui->cmd0, ui->cmd1, ui->cmd2, ui->eg0, ui->eg1, ui->eg2, ui->eg3, ui->eg4,
                         ui->lab_eg0, ui->lab_eg1, ui->lab_eg2, ui->lab_eg3, ui->lab_eg4 };
    for (QWidget* w : names) scaleWidgetFont(w);

    // Additional tab
    QWidget* add[] = { ui->cStartIndicator, ui->label_10, ui->textEdit };
    for (QWidget* w : add) scaleWidgetFont(w);
}

void wAppsett::applyProportionalLayoutScale(double scale)
{
    if (!ui) return;
    // Scale fonts
    applyFontScale(scale);
    // Adjust key spacers/margins proportionally to keep layout density
    int m = qMax(4, (int)qRound(6 * scale));
    if (ui->gridLayout_2) ui->gridLayout_2->setContentsMargins(m, m, m, m);
    if (ui->gridLayout)   ui->gridLayout->setContentsMargins(m, m, m, m);
    if (ui->gridLayout_3) ui->gridLayout_3->setContentsMargins(m, m, m, m);
    if (ui->gridLayout_5) ui->gridLayout_5->setContentsMargins(m, m, m, m);

    auto scaleSpacer = [](QSpacerItem* s, int h){ if (s) s->changeSize(20, h, QSizePolicy::Minimum, QSizePolicy::Fixed); };
    scaleSpacer(ui->verticalSpacer,    qMax(8,  (int)qRound(40 * scale)));
    scaleSpacer(ui->verticalSpacer_2,  qMax(8,  (int)qRound(40 * scale)));
    scaleSpacer(ui->verticalSpacer_5,  qMax(6,  (int)qRound(20 * scale)));
    scaleSpacer(ui->verticalSpacer_10, qMax(6,  (int)qRound(20 * scale)));
    scaleSpacer(ui->verticalSpacer_4,  qMax(6,  (int)qRound(20 * scale)));
    scaleSpacer(ui->verticalSpacer_3,  qMax(10, (int)qRound(28 * scale)));
    scaleSpacer(ui->verticalSpacer_11, qMax(10, (int)qRound(28 * scale)));
    scaleSpacer(ui->verticalSpacer_7,  qMax(6,  (int)qRound(20 * scale)));
    scaleSpacer(ui->verticalSpacer_8,  qMax(6,  (int)qRound(20 * scale)));
    scaleSpacer(ui->verticalSpacer_9,  qMax(6,  (int)qRound(20 * scale)));

    // Ensure row heights accommodate larger fonts: raise min heights of edits and labels
    int rowH = qMax(22, (int)qRound(24 * scale));
    foreach (QLineEdit* e, ui->tabWidget->findChildren<QLineEdit*>()) {
        if (!e) continue;
        e->setMinimumHeight(rowH);
        e->setMaximumHeight(16777215);
        QSizePolicy sp = e->sizePolicy();
        sp.setVerticalPolicy(QSizePolicy::Preferred);
        e->setSizePolicy(sp);
    }
    foreach (QLabel* l, ui->tabWidget->findChildren<QLabel*>()) {
        if (!l) continue;
        l->setMinimumHeight(rowH);
        QSizePolicy sp = l->sizePolicy();
        sp.setVerticalPolicy(QSizePolicy::Preferred);
        l->setSizePolicy(sp);
    }
}

void wAppsett::rescaleViewBoxes()
{
    if (scaledViews.isEmpty()) return; // Native pages on Qt4
    for (int i = 0; i < scaledViews.size() && i < scaledProxies.size(); ++i) {
        QGraphicsView* view = scaledViews[i];
        QGraphicsProxyWidget* proxy = scaledProxies[i];
        if (!view || !proxy) continue;
        QSizeF avail = view->viewport()->size();
        QSizeF content = proxy->size();
        if (content.width() <= 0 || content.height() <= 0) continue;
        double sx = avail.width()  / content.width();
        double sy = avail.height() / content.height();
        double s = qMin(sx, sy);
        if (s <= 0) s = 1.0;
        proxy->setScale(s);
        proxy->setPos(0, 0);
        // expand the scene rect so view paints
        view->scene()->setSceneRect(QRectF(0, 0, avail.width(), avail.height()));
        view->viewport()->update();
    }
}

void wAppsett::wrapPageInViewBox(QWidget* page)
{
    if (!page || !ui || !ui->tabWidget) return;
    QTabWidget* tw = ui->tabWidget;
    int idx = tw->indexOf(page);
    if (idx < 0) return;
    QString title = tw->tabText(idx);
    QIcon icon = tw->tabIcon(idx);

    // Detach the page from the tab widget to avoid the Qt warning and blank tab
    tw->removeTab(idx);
    page->setParent(nullptr);
    page->setWindowFlags(Qt::Widget);

    // Create scene and view
    QGraphicsScene* scene = new QGraphicsScene(tw);
    QGraphicsView* view = new QGraphicsView(scene, tw);
    view->setFrameShape(QFrame::NoFrame);
    view->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    view->setMinimumSize(10, 10);

    // Ensure base size for proper scaling
    QSize base = page->sizeHint();
    if (base.isEmpty()) base = page->minimumSizeHint();
    if (base.width() < 1024) base.setWidth(1024);
    if (base.height() < 600) base.setHeight(600);
    page->resize(base);

    QGraphicsProxyWidget* proxy = scene->addWidget(page);
    proxy->setTransformOriginPoint(0,0);
    proxy->setFlag(QGraphicsItem::ItemIsFocusable, true);
    proxy->resize(base);

    // Keep for future rescaling on resize
    scaledViews << view;
    scaledProxies << proxy;

    tw->insertTab(idx, view, icon, title);
}

void wAppsett::fallbackToNativeIfBlank()
{
    if (!ui || !ui->tabWidget) return;
    bool anyBlank = false;
    for (int i = 0; i < scaledViews.size(); ++i) {
        QGraphicsView* v = scaledViews[i];
        if (!v) continue;
        QSize s = v->viewport()->size();
        if (s.width() < 5 || s.height() < 5) { anyBlank = true; break; }
        // Also detect if scene rect is empty
        if (!v->scene() || v->scene()->sceneRect().isEmpty()) { anyBlank = true; break; }
    }
    if (!anyBlank) return;

    // Replace each view back with the original widget stored in the proxy
    QTabWidget* tw = ui->tabWidget;
    for (int i = 0; i < scaledViews.size() && i < scaledProxies.size(); ++i) {
        QGraphicsView* v = scaledViews[i];
        QGraphicsProxyWidget* p = scaledProxies[i];
        if (!v || !p) continue;
        QWidget* w = p->widget();
        if (!w) continue;
        int idx = tw->indexOf(v);
        QString title = (idx >= 0) ? tw->tabText(idx) : QString();
        QIcon icon    = (idx >= 0) ? tw->tabIcon(idx) : QIcon();
        if (idx >= 0) tw->removeTab(idx);
        w->setParent(tw);
        tw->insertTab(idx >= 0 ? idx : tw->count(), w, icon, title);
        v->deleteLater();
        // keep widget visible
        w->show();
    }
    scaledViews.clear();
    scaledProxies.clear();
}

void wAppsett::on_launch_pwdEnbl_clicked(bool checked)
{
    if (checked) {
        ui->launch_pwdEnbl->setChecked(!checked);
        pWin->admin->dialogAskPwd(QString("Отключить запрос пароля при запуске программы?"));
        if (pWin->admin->exec() == QDialog::Accepted){
            ui->launch_pwdEnbl->setChecked(!checked);
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
