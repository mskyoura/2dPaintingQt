#include "appset.h"
#include "ui_appset.h"

Appset::Appset(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Appset)
{
    pWin = qobject_cast<Window *> (parent);
    if (!pWin) throw 985;

    ui->setupUi(this);
    ui->ePolarity->addItem("Прямая");
    ui->ePolarity->addItem("Обратная");

    ui->okBtn->setDefault(true);

    connect( ui->okBtn,     SIGNAL( clicked() ), SLOT( accept() ) );
    connect( ui->cancelBtn, SIGNAL( clicked() ), SLOT( reject() ) );

    setLayout(ui->gridLayout);

}

Appset::~Appset()
{
    delete ui;
}

void Appset::set(QString _ID, QString _Dst, int _T1, double _T2, double _U1, double _U2, bool _Polarity, const QList<int> & _usedIDs) {

    ID = _ID;
    Dst = _Dst;
    T1 = _T1;
    T2 = _T2;
    U1 = _U1;
    U2 = _U2;
    Polarity = _Polarity;
    usedIDs = _usedIDs;

    ui->eID->setText(ID);
    on_eID_textChanged(ui->eID->text());

    ui->eDst->setText(Dst);
    ui->eT1->setText(QString::number(T1));
    ui->eT2->setText(QString::number(T2, 0, 1));

    ui->eU1->setText(QString::number(U1, 0, 2));
    ui->eU2->setText(QString::number(U2, 0, 2));

    ui->ePolarity->setCurrentIndex(Polarity==false? 0:1);

    //ui->eMsg->setText("");

}

void Appset::accept(){
    if (IDaccepted && T1accepted && T2accepted && U1accepted && U2accepted) {

        bool found = false;

        if (usedIDs.size()>0 && ID != "") {

            bool ok = false;
            int _int = ID.toInt(&ok,10);
            if (ok)
                for (int i=0; i<usedIDs.size(); ++i) {
                    if (usedIDs[i] == _int) {
                        found = true;
                        break;
                    }
                }
        }

        if ((found && (pWin->confirm->showConfirm(QString("ID ") + ID + QString(" уже используется в другой группе (группах),\nВы уверены, что следует использовать этот ID здесь?")) == QDialog::Accepted))
            || !found)
            QDialog::accept();
    }

}

void Appset::on_eID_textChanged(const QString &arg1)
{
    arg1.toUpper();
    TestInRange(0, 254, ID, arg1, ui->eID, IDaccepted);
}

void Appset::on_eDst_textChanged(const QString &arg1)
{
    Dst = arg1;
}

void Appset::on_eT1_textChanged(const QString &arg1)
{
    TestInRange(1, 255, T1, arg1, ui->eT1, T1accepted);
}

void Appset::on_eT2_textChanged(const QString &arg1)
{
    TestInRange(0.0, 120.0, T2, arg1, ui->eT2, T2accepted);
}

void Appset::on_eU1_textChanged(const QString &arg1)
{
    TestInRange(0.0, 15.0, U1, arg1, ui->eU1, U1accepted);
}

void Appset::on_eU2_textChanged(const QString &arg1)
{
    TestInRange(0.0, 15.0, U2, arg1, ui->eU2, U2accepted);
}

void Appset::on_ePolarity_currentIndexChanged(int index)
{
    Polarity = index==1? true:false;
}

void Appset::TestInRange(int min, int max, QString& var, QString value, QLineEdit *e, bool &eAccepted){
    bool ok = false;
    int _int = value.toInt(&ok,16);

    ok = ok && (_int >= min) && (_int <= max);
    if (ok)
        var = value;
    e->setStyleSheet(ok?"QLineEdit {background-color: white;}":"QLineEdit {background-color:#FFBFBF;}");

    eAccepted = ok;
}

void Appset::TestInRange(int min, int max, int& var, QString value, QLineEdit *e, bool &eAccepted){
    bool ok = false;
    int _int = value.toInt(&ok);

    ok = ok && (_int >= min) && (_int <= max);
    var = _int;
    e->setStyleSheet(ok?"QLineEdit {background-color: white;}":"QLineEdit {background-color:#FFBFBF;}");

    eAccepted = ok;
}

void Appset::TestInRange(double min, double max, double &var,
                         QString value, QLineEdit *e, bool &eAccepted){
    bool ok = false;
    double _dbl = value.toDouble(&ok);

    ok = ok && (_dbl >= min) && (_dbl <= max);
    if (ok) var = _dbl;
    e->setStyleSheet(ok?"QLineEdit {background-color: white;}":"QLineEdit {background-color:#FFBFBF;}");

    eAccepted = ok;
}
