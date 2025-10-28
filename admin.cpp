#include "admin.h"
#include "ui_admin.h"
#include "window.h"
#include <QFont>

Admin::Admin(QWidget *parent, QRect _desktop) :
    QDialog(parent),
    ui(new Ui::Admin)
{
    ui->setupUi(this);
    ui->lineEdit->setEchoMode(QLineEdit::Password);

    Parent = parent;
    desktop = _desktop;

    ui->okBtn->setDefault(true);

    connect( ui->okBtn,     SIGNAL( clicked() ), SLOT( accept() ) );
    connect( ui->cancelBtn, SIGNAL( clicked() ), SLOT( reject() ) );

    setLayout(ui->gridLayout);

    mode = 0;
}

Admin::~Admin()
{
    delete ui;
}

void Admin::showEvent(QShowEvent * event){

    int x = desktop.x();
    int y = desktop.y();
    int w = desktop.width();
    int h = desktop.height();

    setGeometry(x + (w-geometry().width())/2, y + (h-geometry().height())/2,geometry().width(),geometry().height());
}

void Admin::Reset(QString s, int mode=0){

    if (mode == 0) {
        ui->lineEdit ->show();
        ui->label_2    ->show();
        ui->cancelBtn->show();
    } else if (mode == 1){
        ui->lineEdit ->hide();
        ui->label_2    ->hide();
        ui->cancelBtn->hide();
    }

    ui->lineEdit->setText("");
    ui->lineEdit->setFocus();
    ui->label->setText(s);

}


void Admin::accept() {
    if (mode == 0) {
        QDialog::accept();
    //-----------------------------
    } else if (mode == 10) {

        if (ui->lineEdit->text() == "") {
            // Отладочная информация для пустого пароля (только в debug режиме)
            Window* pWin = qobject_cast<Window*>(Parent);
            if (pWin && pWin->wAppsettings && pWin->wAppsettings->getIsDebug()) {
                QString debugInfo = QString("Admin::accept() - режим %1: пустой пароль\n").arg(mode);
                debugInfo += QString("Текущий режим: %1\n").arg(mode);
                debugInfo += QString("Текст в поле: '%1'\n").arg(ui->lineEdit->text());
                
                if (pWin->warn) {
                    pWin->warn->showWarning(debugInfo);
                }
            }
        } else {
            mode = 11;
            launch_pwd1 = hash(ui->lineEdit->text());
            Reset("Подтвердите новый пароль:");
        }
    } else if (mode == 11) {
        launch_pwd2 = hash(ui->lineEdit->text());
        if (launch_pwd1 == launch_pwd2) {
            launch_pwd = launch_pwd1;
            Reset("Новый пароль успешно установлен.",1);
            mode = 12;
        } else {
            Reset("Ошибка при повторном вводе - новый пароль не установлен.",1);
            mode = 12;
        }
    } else if (mode == 12) {
            QDialog::reject();
    //-----------------------------
    } else if (mode == 20) {
        if (launch_pwd == hash(ui->lineEdit->text())) {
            mode = 0;
            QDialog::accept();
        } else {
            Reset("Ошибка при вводе пароля - попробуйте еще раз:");
            mode = 20;
        }

    } else {
        // Отладочная информация для неизвестного режима (только в debug режиме)
        Window* pWin = qobject_cast<Window*>(Parent);
        if (pWin && pWin->wAppsettings && pWin->wAppsettings->getIsDebug()) {
            QString debugInfo = QString("Admin::accept() - неизвестный режим: %1\n").arg(mode);
            debugInfo += QString("Текст в поле: '%1'\n").arg(ui->lineEdit->text());
            debugInfo += QString("launch_pwd1: %1\n").arg(launch_pwd1);
            debugInfo += QString("launch_pwd2: %1\n").arg(launch_pwd2);
            
            if (pWin->warn) {
                pWin->warn->showWarning(debugInfo);
            }
        }
    }
}

void Admin::dialogSetPwd(){
    Reset("Установите новый пароль:");
    mode = 10;
}

void Admin::dialogAskPwd(QString s){
    Reset(s);
    mode = 20;
}

std::size_t Admin::getCurrentPwd(){
    return launch_pwd;
}

void Admin::setCurrentPwd(std::size_t a){
    launch_pwd = a;
}

std::size_t Admin::hash(QString s) {
    std::size_t h1 = std::hash<std::string>{}(s.toStdString().c_str()); //http://en.cppreference.com/w/cpp/utility/hash
    std::size_t h2 = std::hash<std::string>{}("3,1415926535897932384626433832795");
    return h1 ^ (h2 << 1);
}
