#include "vismo.h"
#include "colors.h"
#include <QtCore/qmath.h>

QList <QString> Vismo::PBstatuses =
    {
    "НЕИЗВ.","НЕИЗВЕСТНО",
    "Реле1 выкл.","Реле1 выключено", //"ЗАБЛОК.","ЗАБЛОКИРОВАНО"
    "Реле1 вкл.","Реле1 включено", //"РАЗБЛОК.","РАЗБЛОКИРОВАНО"
    "Реле2 вкл.","Реле2 включено", //"ИНИЦИИР.","ИНИЦИИРОВАНО"
    };

QList <QString> Vismo::PBcommands =
    {"ПРОВЕРКА СВЯЗИ",
     "Выключить Реле1", //"ЗАБЛОКИРОВАТЬ" Реле1: 00 = ЗБ
     "Включить Реле1", //"РАЗБЛОКИРОВАТЬ" Реле1: 01 = РБ
     "Запустить Реле2"}; //"ПУСК"

QList <QString> Vismo::PBgroups_names = {"ГРУППА 1", "ГРУППА 2", "ГРУППА 3", "ГРУППА 4", "ГРУППА 5"};
QList <QColor>  Vismo::PBgroups_clr = {QColor(251,220,4), QColor(89,197,58), QColor(255,0,0), QColor(0,176,240), QColor(216,146,207)};
int Vismo::PBgroups_len = PBgroups_names.size();

QFont Vismo::textFont = QFont();

QFont Vismo::shortStatusFont = QFont();
int Vismo::shortStatusWidth = 0;

QFont Vismo::fullStatusFont = QFont();
int Vismo::fullStatusWidth = 0;

QFont Vismo::commandFont = QFont();
int Vismo::commandWidth = 0;

QFont Vismo::pbGroupsFont = QFont();
int Vismo::pbGroupsWidth = 0;

int Vismo::activePBGroup = 0;

QPainter* Vismo::FontPainter = new QPainter();

Vismo::Vismo(int _mo07, int type)
{
    moActive = false;
    mo07 = _mo07;
    moType = type;
    statusbarTxt = "";
    pb = NULL;
    clickEnabled = true;
}

void Vismo::setGeom(int x, int y, int w, int hTxtLines, int hCmdLines, int _CellSpace, int BtnDiam) {
    moX = x;
    moY = y;
    moWidth = w;
    moTxtLineHeight = hTxtLines;
    moCmdLineHeight = hCmdLines;
    moCellSpace = _CellSpace;
    moButtonDiam = BtnDiam;

    CopyRect =  QRect(0,0,0,0);
    DelRect =   QRect(0,0,0,0);
    SetupRect = QRect(0,0,0,0);


    TestLinkRect = QRect(0,0,0,0);
    UnblockRect = QRect(0,0,0,0);
    BlockRect = QRect(0,0,0,0);
    StartRect = QRect(0,0,0,0);
    EditRect = QRect(0,0,0,0);

    for (int i = 0; i<4; i++)
        menuRect[i] = QRect(0,0,0,0);

    for (int i = 0; i<5; i++)
        groupRect[i] = QRect(0,0,0,0);

    grTestLinkRect = QRect(0,0,0,0);
    grBlockRect = QRect(0,0,0,0);
    grUnblockRect = QRect(0,0,0,0);
    grStartRect = QRect(0,0,0,0);

    StatusbarRect = QRect(0,0,0,0);

}

void Vismo::setPB(QList<Saver>* _pb, int _pbIndex) {
    pb = _pb;
    pbIndex = _pbIndex;
}

//void Vismo::setActiveCnt(int a) {
//    ActiveCnt[activePBGroup] = a;
//}

//int Vismo::getActiveCnt() {
//  return ActiveCnt[activePBGroup];
//}

int Vismo::isPointInCircle(QRect r, QPoint p){
    return (qPow(r.x()+r.width()/2-p.x(),2) + qPow(r.y()+r.height()/2-p.y(),2)) <= moButtonDiam*moButtonDiam/4;
}

bool Vismo::isActive(){
    return moActive;
}

int Vismo::ClickDispatch(QPoint p, int& number){

    if (!clickEnabled)
        return -1;

    int ret = -1; //вне органов управления

    ClickedPoint = p;

    if (moActive && SetupRect.contains(p)) {
        ret = 0;
        ClickedArea = SetupRect;
        if (CopyRect.contains(p) && isPointInCircle(CopyRect,ClickedPoint)){
            ret = 1;
            ClickedArea = CopyRect;
        } else if (DelRect.contains(p) && isPointInCircle(DelRect,ClickedPoint)){
            ret = 2;
            ClickedArea = DelRect;
        }
    }

    number = mo07;

    return ret;
}

int Vismo::SysCmdClickDispatch(QPoint p){

    if (!clickEnabled)
        return -1;

    int ret = -1; //вне органов управления

    if (!moActive) return -1;

    if      (menuRect[0].contains(p))
        ret = 0;
    else if (menuRect[1].contains(p))
        ret = 1;
    else if (menuRect[2].contains(p))
        ret = 2;
    else if (menuRect[3].contains(p))
        ret = 3;
    else if (grTestLinkRect.contains(p))
        ret = 4;
    else if (grBlockRect.contains(p))
        ret = 5;
    else if (grUnblockRect.contains(p))
        ret = 6;
    else if (grStartRect.contains(p))
        ret = 7;
    else if (StatusbarRect.contains(p))
        ret = 8;
    else if (groupRect[0].contains(p))
        ret = 9;
    else if (groupRect[1].contains(p))
        ret = 10;
    else if (groupRect[2].contains(p))
        ret = 11;
    else if (groupRect[3].contains(p))
        ret = 12;
    else if (groupRect[4].contains(p))
        ret = 13;

    return ret;
}

int Vismo::PersonalRemoteClickDispatch(QPoint p){

    if (!clickEnabled)
        return -1;

    int ret = -1; //вне органов управления

    if (!moActive) return -1;

    if      (TestLinkRect.contains(p))
        ret = 0;
    else if (BlockRect.contains(p))
        ret = 1;
    else if (UnblockRect.contains(p))
        ret = 2;
    else if (StartRect.contains(p))
        ret = 3;
    else if (EditRect.contains(p))
        ret = 4;

    return ret;
}

void Vismo::setActiveNumber(bool Active, int Number) {

    //для изображений пультов управления:
    // - если был активным,  уменьшить счетчик активных
    // - если был пассивным, увеличить счетчик активных

    /*if      ((moActive == 1) && (Active == 0) && (moType==0))
        setActiveCnt(getActiveCnt()-1);
    else if ((moActive == 0) && (Active == 1) && (moType==0))
        setActiveCnt(getActiveCnt()+1);*/

    moActive = Active;
    moNumber = Number;
}

void Vismo::DrawText(int enbld, double kFontSize, double kH, QString s, QColor bkg, QColor clrL,
                     bool boldL = 0,
                     QColor clrR=Qt::magenta, bool boldR = 0) {

    int x = moX,
        y = moY + LineY,
        w = moWidth,
        h = moTxtLineHeight;

    LineY += h*kH;
    if (enbld ==0) return;

    textFont.setBold(boldL);


    kFontSize = kFontSize<0? -kFontSize:kFontSize;

    textFont.setPixelSize(moTxtLineHeight*kFontSize);

    int maxTextWidth = 0.9 * w;

    FitFontSize(maxTextWidth,s,kFontSize,textFont);

    if (bkg != Qt::magenta) {
        QBrush b = painter->brush();
        painter->setBrush(bkg);
        BorderPen.setColor(Qt::white);
        BorderPen.setWidth(2);
        painter->setPen(BorderPen);
        painter->drawRect(QRect(x+1,y,w-2,h*kH));
        painter->setBrush(b);
    }

    TextPen.setColor(clrL);
    painter->setPen(TextPen);
    textFont.setBold(0);

    if (clrR == Qt::magenta) {
        painter->setPen(TextPen);
        painter->drawText(QRect(x,y,w,h*kH), Qt::AlignCenter | Qt::AlignVCenter, s);
    } else {
        int pos = s.indexOf(":")+1;
        QString sL = s.left(pos);
        QString sR = s.right(s.length()-pos);

        QFont textFontR = textFont;
        textFontR.setBold(boldR);




        painter->setFont(textFontR);
        QFontMetrics fm = painter->fontMetrics();
        int _wR = fm.width(sR);//хорошо для Bold, для неBold вернет ерунду


        while ((QFontMetrics(textFont).width(sL) > (maxTextWidth - _wR)*0.8) && (kFontSize > 0.3)) {
            kFontSize -= 0.05;
            textFont.setPixelSize(moTxtLineHeight*kFontSize);
        }

        int _wL = QFontMetrics(textFont).width(sL);//хорошо для неBold, для Bold вернет как неBold

        int _w = _wL + _wR;

        painter->setFont(textFont);
/*      для проверки прямоугольников, в которые впишутся non-bold и bold тексты
        QColor bkg;
        bkg.setRgb(190,70,50);
        painter->setBrush(bkg);
        painter->drawRect(QRect(x+(w-_w)/2 + _wL,y,_wR,h*kH));

        bkg.setRgb(70,90,50);
        painter->setBrush(bkg);
        painter->drawRect(QRect(x+(w-_w)/2,y,_wL,h*kH));
*/
        TextPen.setColor(clrR);
        painter->drawText(QRect(x+(w-_w)/2,y,_wL,h*kH),       Qt::AlignCenter | Qt::AlignVCenter, sL);
        painter->setPen(TextPen);
        painter->setFont(textFontR);
        textFont.setBold(0);
        painter->drawText(QRect(x+(w-_w)/2 + _wL,y,_wR,h*kH), Qt::AlignCenter | Qt::AlignVCenter, sR);
    }
}

void Vismo::DrawCmd(int enbld, QString type, char adj, QColor bkg=Qt::magenta, bool bold=0, QColor clr=Qt::magenta) {

    if (enbld ==0) return;

    int X = 0;
    if (adj == 'R')
        X = moX + 0.75*(moWidth-2.0*moButtonDiam) + moButtonDiam;
    else if (adj == 'L')
        X = moX + 0.25*(moWidth-2.0*moButtonDiam);
    else if (adj == 'C')
        X = moX + moWidth/2 - moButtonDiam/2;

    int Y = moY + LineY + moCellSpace;

    int w = 8;
    int w2 = -1;//w/2;

    if (type == "+") { // сделать компактнее (+ и * вместе)

        CmdPen.setColor(Qt::gray);

        int R = 0.6*moButtonDiam;

        int x1 = X + moButtonDiam/2,
            y1 = Y + moButtonDiam/2;

        CmdPen.setWidth(w);
        painter->setPen(CmdPen);

        painter->drawLine(x1-R/2-w2,y1-w2,x1+R/2-w2,y1-w2);
        painter->drawLine(x1-w2,y1-R/2-w2,x1-w2,y1+R/2-w2);

        CopyRect.setX(X);
        CopyRect.setY(Y);
        CopyRect.setWidth(moButtonDiam);
        CopyRect.setHeight(moButtonDiam);

        CmdPen.setWidth(2);
        painter->setPen(CmdPen);
        painter->drawEllipse(X, Y, moButtonDiam, moButtonDiam);

    } else if (type == "x") {

        CmdPen.setColor(RBcolor);

        int R = 0.6*moButtonDiam;

        int x1 = X + moButtonDiam/2,
            y1 = Y + moButtonDiam/2;

        double d = R/2.0*qSin(qDegreesToRadians(45.0));

        CmdPen.setWidth(w);
        painter->setPen(CmdPen);

        painter->drawLine(x1-d-w2,y1-d-w2,x1+d-w2,y1+d-w2);
        painter->drawLine(x1+d-w2,y1-d-w2,x1-d-w2,y1+d-w2);

        DelRect.setX(X);
        DelRect.setY(Y);
        DelRect.setWidth(moButtonDiam);
        DelRect.setHeight(moButtonDiam);

        CmdPen.setWidth(2);
        painter->setPen(CmdPen);
        painter->drawEllipse(X, Y, moButtonDiam, moButtonDiam);

    } else if (type.length() == 2) {

        QString s = "---";
        int w = 0.8 * moWidth;
        QRect dr(0.5*(moWidth-w),Y, w, 0.75*moCmdLineHeight);

        if        (type == "ПС") {
            s = PBcommands[0];
            TestLinkRect = dr;
        } else if (type == "ЗБ") {
            s = PBcommands[1];
            BlockRect = dr;
        } else if (type == "РБ") {
            s = PBcommands[2];
            UnblockRect = dr;
        } else if (type == "ПК") {
            s = PBcommands[3];
            StartRect = dr;
        }

        if (bkg != Qt::magenta) {
            QPen pen;
            pen.setColor(Qt::white);
            pen.setWidth(2);
            painter->setBrush(bkg);
            painter->setPen(pen);
            painter->drawRect(dr);
        }

        double kFontSize = 0.8;
        textFont.setPixelSize(moTxtLineHeight*kFontSize);
        textFont.setBold(bold);
        int tw = 0;
        while ((tw = QFontMetrics(textFont).width(s)) > 0.9*w && (kFontSize > 0.3)) {
            kFontSize -= 0.05;
            textFont.setPixelSize(moTxtLineHeight*kFontSize);
        }

        painter->setFont(textFont);
        painter->setPen(clr);
        painter->drawText(dr, Qt::AlignCenter | Qt::AlignVCenter, s);

        textFont.setBold(0);
    }
}

double Vismo::getSumOfKSize(){
    return 9.6;
}

double Vismo::getSumOfKSizePUindiv(){
    return Inside(false, 0,0,0);
}

double Vismo::Inside(bool painting, int i, bool blinktoggle, int ActiveCntInGroup){
    LineY = 0;

    bool isWaitingForDelay = false;

    bool active = (pb!=NULL) && (pbIndex >-1);
    QString empty("");

    QColor GrayTxt = QColor(188, 188, 188);

    QString dst, id, elpsd, blnk, t1, t2, u, coil;
    QColor  uClr, coilClr, statusClr;
    QString nums;

    int mode = 0;
    if (moType == 0 || moType == 1)
        mode = 1;
    else if (moType == 2)
        mode = 2;

    QString debugText = "";

    if (active) {
        dst = (*pb)[pbIndex].getDst(1);
        id = (*pb)[pbIndex].getID(1);
        elpsd = (*pb)[pbIndex].getElapsedTime();

        //2021-01-22
        isWaitingForDelay = (*pb)[pbIndex].isWaitingForDelayT2();
        //В режиме отладки почему-то ответы от платы не отрабатываются (нет ответа). В режиме релиза - работает, отлаживать через debugText.
        //debugText = (*pb)[pbIndex].getPressedButton() + "/" + (*pb)[pbIndex].getDebugText();
        if (debugText != 0){
            static QFile dbgf("d:\\debugText.txt");
            static bool dbgfReady = dbgf.open(QIODevice::WriteOnly);
            static QTextStream out(&dbgf);
            out.setCodec("Windows-1251");
            out << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") << ": " << debugText << endl;
        }
        //-

        blnk = !((*pb)[pbIndex].getHasLastOperationGoodAnswer()) && blinktoggle?
                    empty: (*pb)[pbIndex].getLastOperationWithGoodAnswer(mode, statusClr, Vismo::PBstatuses);
        t1 = (*pb)[pbIndex].getT1(1);
        t2 = (*pb)[pbIndex].getT2(1);
        (*pb)[pbIndex].getU_coil(u,uClr,coil,coilClr);
        nums = "#" + QString::number((*pb)[pbIndex].CmdNumReq()) + "/" + QString::number((*pb)[pbIndex].CmdNumReq());
    }

    QColor  statusClrModif = isWaitingForDelay? /*RBdelaycolor*/ /*Qt::yellow*/ QColor(255,60,0) : statusClr;
    QString blnkModif      = isWaitingForDelay? (*pb)[pbIndex].getLastOperationWithGoodAnswer(mode, statusClr, Vismo::PBstatuses) : blnk;

    if (mode == 1) { //отрисовка всех доступных ПБ1-8, кнопки добавления ПБ (+)

        //при изменении кол-ва и значений коэф-тов - обнови результат getSumOfKSize()

        DrawText(i,0.8,  1.0, QString("ПБ") + QString::number(moNumber) //+ "."+ QString::number(mo07)
                 ,                   Qt::magenta, QColor(83, 151, 200),1);

//2021 потом удалить!
        if (debugText != "") DrawText(i,0.7,  1.0, debugText,   Qt::gray, Qt::yellow);
//-2021

        DrawText(i,0.7,  1.0, dst,   Qt::magenta, QColor(83, 151, 200));

        DrawText(i,0.7,  1.0, id,    Qt::magenta, QColor(83, 151, 200));
        LineY +=         0.1*moTxtLineHeight;
        DrawText(i,0.7,  1.0, elpsd, Qt::magenta, QColor(242, 194, 94));
        LineY +=         0.1*moTxtLineHeight;
        DrawText(i,0.7,  1.0, blnkModif,  statusClrModif, Qt::white);
        LineY +=         0.1*moTxtLineHeight;
        DrawText(i,0.7,  1.0, t1,    Qt::magenta, GrayTxt);
        DrawText(i,0.7,  1.0, t2,    Qt::magenta, GrayTxt);
        DrawText(i,0.7,  1.0, u,     Qt::magenta, uClr);
        DrawText(i,0.7,  1.0, coil,  Qt::magenta, coilClr);
        LineY +=         0.3*moTxtLineHeight;

        if (ActiveCntInGroup<8) DrawCmd(i,"+",'L');
        /*if (getActiveCnt()>1) #nonePB*/ DrawCmd(i,"x",'R');
        LineY += 1.1*moButtonDiam;

        moHeight = LineY;
    }

    if (mode == 2) { // персональный ПБ

        QPen pen;
        pen.setColor(Qt::white);
        pen.setWidth(2);
        if (painting) {
            painter->setBrush(QColor(66,113,157));
            painter->setPen(pen);
            painter->drawRect(1, 1, moWidth-2, (1.0+0.8+0.8+0.2)*moTxtLineHeight-2);
        }

        bool active = (pb!=NULL) && (pbIndex >-1);
        QString none("---");

        LineY +=         0.2*moTxtLineHeight;
        DrawText(i,1.2,  1.0,
                 QString("ПБ") + QString::number(moNumber),
                                                    Qt::magenta, QColor(255, 255, 255), 1);
        DrawText(i,0.7,  0.8, active? (*pb)[pbIndex].getDst(1):none, Qt::magenta, QColor(188, 188, 188));

//2021 потом удалить!
        if (debugText != "") DrawText(i,0.7,  1.0, debugText,   Qt::gray, Qt::yellow);
//-2021

        DrawText(i,0.7,  0.8, QString("ID: ")+
                             (active? (*pb)[pbIndex].getID (1):none),
                              Qt::magenta, QColor(165, 165, 165),
                              0, QColor(188,188,188), 1);

        LineY += 0.3*moTxtLineHeight;
        DrawText(i,0.7,  0.8, "Последние данные",   Qt::magenta, QColor(128, 128, 128));
        DrawText(i,0.7,  0.8,elpsd +
                              " назад",     Qt::magenta, QColor(188,188,188), 1);
        LineY += 0.3*moTxtLineHeight;
        DrawText(i,0.7,  1.2, blnkModif,  statusClrModif, QColor(215, 215, 215), 1);
        LineY += 0.3*moTxtLineHeight;
        DrawText(i,0.7,  0.8, QString("Длительность Реле2") + ": " +
                             (active? (*pb)[pbIndex].getT1()+" с":none), Qt::magenta, QColor(165, 165, 165), 0, QColor(188,188,188), 1);
        DrawText(i,0.7,  0.8, QString("Задержка Реле2: ")+
                             (active? (*pb)[pbIndex].getT2()+" с":none),Qt::magenta, QColor(165, 165, 165), 0, QColor(188,188,188), 1);
        LineY += 0.2*moTxtLineHeight;
        DrawText(i,0.7,  0.8, QString("Питание: ")+
                              u,   Qt::magenta,     QColor(165, 165, 165), 0, uClr, 1);
        DrawText(i,0.7,  0.8, QString("Цепь ЭВС: ")+
                              coil,    Qt::magenta, QColor(165, 165, 165), 0, coilClr, 1);
        LineY += 0.3*moTxtLineHeight;

        DrawCmd(i,"ПС",'C', PScolor,   1, QColor(215, 215, 215));
        LineY += 0.9*moCmdLineHeight;
        DrawCmd(i,"ЗБ",'C', ZBcolor,  1, QColor(215, 215, 215));
        LineY += 0.9*moCmdLineHeight;
        DrawCmd(i,"РБ",'C', RBcolor,    1, QColor(215, 215, 215));
        LineY += 0.9*moCmdLineHeight;
        DrawCmd(i,"ПК",'C', PKcolor,  1, QColor(215, 215, 215));
        LineY += 0.9*moCmdLineHeight;

        LineY += 0.2*moCmdLineHeight;

        moHeight = LineY;

        EditRect = QRect(0,0,moWidth, moHeight);
    }

    return moHeight;
}

void Vismo::FitFontSize(int maxTextWidth, QString s, double kFontSize, QFont textFont) {
    painter->setFont(textFont);
    QFontMetrics fm = painter->fontMetrics();

    int realW = 0;

    while ((realW = fm.width(s)) > maxTextWidth && (kFontSize > 0.3)) {
        kFontSize -= 0.01;
        textFont.setPixelSize(moTxtLineHeight*kFontSize);
        painter->setFont(textFont);
        fm = painter->fontMetrics();
    }
}


void Vismo::Draw(QPainter *_painter, bool blinktoggle, int ActiveCntInGroup) {

    if (!moActive)
        return;

    painter = _painter;

    TextPen.setColor(Qt::black);

    CmdPen.setWidth(3);
    CmdPen.setCapStyle(Qt::RoundCap);

    for (int i=0; i<2; i++) //первый раз - расчет высоты и отрисовка контура и тела блока, второй - текст
    {
        Inside(true, i, blinktoggle,ActiveCntInGroup);

        if (moType == 3) { // vmMenu
            QPen pen;
            pen.setColor(Qt::white);
            pen.setWidth(2);
            painter->setPen(pen);

            textFont.setPixelSize(moTxtLineHeight*0.5);
            painter->setFont(textFont);
            painter->setPen(Qt::white);

            const int len = 4;
            QString s[len] = {"Диагностика", "Настройки", "О программе", "ВЫХОД"};
            QRect r = QRect(0,0,0,0);
            int totalW=0,
                tw=0;

            int maxlenIndex = 0;
            for (int i=1; i<len; i++)
                if (s[i].length() > s[maxlenIndex].length())
                    maxlenIndex = i;


            for (int i=0; i<len; i++) {

                if (i == len-1)
                    painter->setBrush(QColor(255,0,0));
                else
                    painter->setBrush(QColor(163,163,163));

                tw = 2.0 * QFontMetrics(textFont).width(s[maxlenIndex]);
                r = QRect(moX + totalW, moY, tw, moTxtLineHeight);
                painter->drawRect(r);
                painter->drawText(r, Qt::AlignCenter | Qt::AlignVCenter, s[i]);
                totalW += tw;
                menuRect[i] = r;
            }
        }

        if (moType == 4) { // vmStatus

            QPen pen;
            QColor stclr = QColor(50,50,50);
            pen.setColor(stclr);
            pen.setWidth(0);
            painter->setPen(pen);
            painter->setBrush(stclr);

            QRect r =  QRect(moX, moY, moWidth, moTxtLineHeight);
            QRect r2 = QRect(moX, moY-moTxtLineHeight, moWidth, moTxtLineHeight);
            StatusbarRect = QRect(moX, moY+0*moTxtLineHeight, moWidth, 1*moTxtLineHeight);

            painter->drawRect(StatusbarRect);

            textFont.setPixelSize(moTxtLineHeight*0.5);
            painter->setFont(textFont);
            painter->setPen(Qt::white);

            painter->drawText(r, Qt::AlignLeft | Qt::AlignVCenter, QString(" ")+statusbarTxt);
            if (statusbarTxt2 != "") {
                painter->setPen(statusbarClr2);
                painter->drawText(r2, Qt::AlignLeft | Qt::AlignVCenter, QString(" ")+statusbarTxt2);
            }

        }

        if (moType == 5) { // vmGrComm
            QPen pen;
            pen.setColor(Qt::white);
            pen.setWidth(2);
            painter->setBrush(QColor(38,38,38));
            painter->setPen(pen);
            painter->drawRect(moX, moY, moWidth, 3.5*moTxtLineHeight);

            textFont.setPixelSize(moTxtLineHeight*0.6);
            textFont.setBold(false);
            painter->setFont(textFont);

            QRect r = QRect(moX, moY, moWidth, moTxtLineHeight);
            painter->drawText(r, Qt::AlignCenter | Qt::AlignVCenter, "Групповые команды");

            double kws = 0.05;//зазор м. кнопками в долях от ширины кнопки
            int w = (double)moWidth/4.0/(1.0+2.0*kws),
                totalW = moX + kws*w;

            QString s="";


            double kFontSize = 0.8;
            textFont.setPixelSize(moTxtLineHeight*kFontSize);
            textFont.setBold(true);
            int tw = 0;

            int maxTextWidth = 0.9 * w;

            commandFont = textFont;
            commandWidth = maxTextWidth;

            s = PBcommands[0];
            FitFontSize(maxTextWidth,s,kFontSize,textFont);
            painter->setBrush(PScolor);
            r = QRect(totalW, moY + moTxtLineHeight*1.5, w, 0.75*moCmdLineHeight);
            painter->drawRect(r);
            painter->drawText(r, Qt::AlignCenter | Qt::AlignVCenter, s);
            totalW += (kws+kws+1.0)*w;
            grTestLinkRect = r;

            s = PBcommands[1];
            FitFontSize(maxTextWidth,s,kFontSize,textFont);
            painter->setBrush(ZBcolor);
            r = QRect(totalW, moY + moTxtLineHeight*1.5, w, 0.75*moCmdLineHeight);
            painter->drawRect(r);
            painter->drawText(r, Qt::AlignCenter | Qt::AlignVCenter, s);
            totalW += (kws+kws+1.0)*w;
            grBlockRect = r;

            s = PBcommands[2];
            FitFontSize(maxTextWidth,s,kFontSize,textFont);
            painter->setBrush(RBcolor);
            r = QRect(totalW, moY + moTxtLineHeight*1.5, w, 0.75*moCmdLineHeight);
            painter->drawRect(r);
            painter->drawText(r, Qt::AlignCenter | Qt::AlignVCenter, s);
            totalW += (kws+kws+1.0)*w;
            grUnblockRect = r;

            s = PBcommands[3];
            FitFontSize(maxTextWidth,s,kFontSize,textFont);
            painter->setBrush(PKcolor);
            r = QRect(totalW, moY + moTxtLineHeight*1.5, w, 0.75*moCmdLineHeight);
            painter->drawRect(r);
            painter->drawText(r, Qt::AlignCenter | Qt::AlignVCenter, s);
            totalW += (kws+kws+1.0)*w;
            grStartRect = r;

            textFont.setBold(false);

        }

        if (moType == 6) { //groups of PB
            QPen pen;
            pen.setColor(Qt::white);
            pen.setWidth(2);
            painter->setPen(pen);

            double kFontSize = 0.7;
            textFont.setPixelSize(moTxtLineHeight*kFontSize);
            painter->setFont(textFont);
            painter->setPen(Qt::white);

            QRect r = QRect(0,0,0,0);

            int tw = 0.9 / ((double) PBgroups_len) * moWidth;//;3.0 * QFontMetrics(textFont).width(s[maxlenIndex]);

            int d = (moWidth - ((double) PBgroups_len)*tw)/((double) PBgroups_len);
            int totalW = 0.5 * d;

            int maxTextWidth = tw * 0.9;

            for (int i=0; i<PBgroups_len; i++) {

                if (i == activePBGroup) {
                    textFont.setBold(true);

                    pbGroupsFont = textFont;
                    pbGroupsWidth = maxTextWidth;

                    FitFontSize(maxTextWidth,PBgroups_names[i],kFontSize,textFont);
                    r = QRect(moX + totalW, moY, tw, moTxtLineHeight);



                    painter->setBrush(PBgroups_clr[i]);
                    painter->setPen(PBgroups_clr[i]);
                    painter->drawRect(r);
                    painter->drawRect(moX, moY + moTxtLineHeight, moWidth, moTxtLineHeight/10);

                    painter->setPen(Qt::white);
                    painter->drawText(r, Qt::AlignCenter | Qt::AlignVCenter, PBgroups_names[i]);

                    textFont.setBold(false);
                    painter->setFont(textFont);
                } else {
                    r = QRect(moX + totalW, moY, tw, moTxtLineHeight);

                    FitFontSize(maxTextWidth,PBgroups_names[i],kFontSize,textFont);

                    painter->setBrush(QColor(Qt::black));
                    painter->setPen(PBgroups_clr[i]);
                    //painter->drawRect(r);
                    painter->drawText(r, Qt::AlignCenter | Qt::AlignVCenter, PBgroups_names[i]);
                }

                groupRect[i] = r;

                totalW += tw + d;
            }
        }

        if (i==1) {
            //painter->setPen(Qt::green);
            //painter->drawEllipse(ClickedPoint.x()-5, ClickedPoint.y()-5, 10, 10);
            //painter->drawRect(ClickedArea);
        }

        if (i==0 &&  (moType == 0)) { //тело и контур ПУ1-8
            SetupRect.setX(moX+1);
            SetupRect.setY(moY);
            SetupRect.setWidth(moWidth-2);
            SetupRect.setHeight(LineY+10);

            BorderPen.setColor(Qt::white);
            BorderPen.setWidth(2);
            painter->setPen(BorderPen);
            QBrush b = painter->brush();
            painter->setBrush(Qt::black);
            painter->drawRect(SetupRect);
            painter->setBrush(b);

            shortStatusWidth = 0.9 * moWidth;
            shortStatusFont = textFont;
            shortStatusFont.setBold(false);
            shortStatusFont.setPixelSize(0.7 * moTxtLineHeight);

            fullStatusWidth = 0.9 * 3.0 * moWidth;
            fullStatusFont = textFont;
            fullStatusFont.setBold(true);
            fullStatusFont.setPixelSize(0.7 * moTxtLineHeight);

            FontPainter = painter;

        }

        if (i==0 &&  (moType == 2)) { //тело и контур ПУиндив
            SetupRect.setX(1);
            SetupRect.setY(1);
            SetupRect.setWidth(moWidth-2);
            SetupRect.setHeight(moHeight-2);

            QPen p = BorderPen;
            BorderPen.setColor(Qt::white);
            BorderPen.setWidth(2);
            painter->setPen(BorderPen);
            BorderPen = p;
            QBrush b = painter->brush();
            painter->setBrush(Qt::black);
            painter->drawRect(SetupRect);
            painter->setBrush(b);
        }

        if (i==0 && moType == 1) {//(+)
            SetupRect.setX(moX+1);
            SetupRect.setY(moY);
            SetupRect.setWidth(moWidth-2);
            SetupRect.setHeight(LineY+10);

            int tmp = LineY;
            LineY = (SetupRect.height() - moCmdLineHeight)/2;
            DrawCmd(1,"+",'C');
            LineY = tmp;
            break;
        }
    }
}

void Vismo::CopySettingsTo(Vismo &vm){
/*
    -Wrect
    -PBmodulesWidth
    -PBmodulesY
    -PBmodulesWidth
    hTxtLines
    hCmdLines
    PBmodulesCellSpace
    PBmodulesRButton    */

    vm.moTxtLineHeight = moTxtLineHeight;
    vm.moCmdLineHeight = moCmdLineHeight;
    vm.moActive = moActive;
    vm.moCellSpace = moCellSpace;
    vm.moButtonDiam = moButtonDiam;

    //vm.moNumber = moNumber;

    //-------------------------

    vm.moNumber        = moNumber;

}

void Vismo::setType(int t){
    moType = t;
}

int Vismo::GetVisualNumber() {
    return moNumber;
}

void Vismo::SetVisualNumber(int n) {
    moNumber = n;
}

int Vismo::getpbIndex() {
    return pbIndex;
}
