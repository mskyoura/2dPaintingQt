#ifndef VISMO_H
#define VISMO_H

#include <QPainter>
#include <QPaintEvent>
#include <QWidget>
#include <QBrush>
#include <QFont>
#include <QPen>
#include <QList>
#include <QDateTime>

#include "dbg.h"
#include "saver.h"

class Vismo //visual module
{
    int moType;             //0 - пульты, 1 - "все модули", 2 - ПБ 1-8
    int moTxtLineHeight;    //высота строки
    int moCmdLineHeight;    //высота строки
    int moButtonDiam;       //радиус кнопки
    bool moActive;          //активный (видимый на экране и реагирующий на кнопки)

    int moCellSpace;        //отступ от границы для круглой кнопки

    int LineY;

    QPen BorderPen;
    QPen TextPen;
    QPen CmdPen;

    void DrawText(int enbld, double kFontSize, double kH, QString s, QColor bkg, QColor clrL, bool boldL, QColor clrR, bool boldR);
    void DrawCmd(int enbld, QString type, char adj, QColor bkg, bool bold, QColor clr);

    QPainter* painter;

    QRect CopyRect;
    QRect DelRect;
    QRect SetupRect;

    QRect TestLinkRect;
    QRect UnblockRect;
    QRect BlockRect;
    QRect StartRect;
    QRect EditRect;


    QRect menuRect[4];
    QRect groupRect[5];

    QRect grTestLinkRect;
    QRect grBlockRect;
    QRect grUnblockRect;
    QRect grStartRect;

    QRect StatusbarRect;

    QPoint ClickedPoint;
    QRect ClickedArea;

    int mo07;//порядковый номер в списке (задается раз и навсегда при ини)

    int isPointInCircle(QRect r, QPoint p);

    QList<Saver>* pb;
    int pbIndex;

    double Inside(bool painting, int i, bool blinktoggle, int ActiveCntInGroup);

    void FitFontSize(int maxTextWidth, QString s, double kFontSize, QFont textFont);

public:

    int moX, moY;       //левая верхняя точка виджета
    int moNumber;       //число, отображаемое на модуле

    static QList <QString> PBstatuses;
    static QList <QString> PBcommands;
    static QList <QString> PBgroups_names;
    static QList <QColor>  PBgroups_clr;
    static int PBgroups_len;

    QString statusbarTxt;
    QString statusbarTxt2;
    QColor statusbarClr2;

    static QFont textFont;

    static QFont shortStatusFont;
    static int shortStatusWidth;
    static QFont fullStatusFont;
    static int fullStatusWidth;
    static QFont commandFont;
    static int commandWidth;
    static QFont pbGroupsFont;
    static int pbGroupsWidth;
    static QPainter* FontPainter;

    Vismo(int _mo07=-1, int type=-1);
    void Draw(QPainter *_painter, bool blinktoggle=false, int ActiveCntInGroup = 0);
    void setGeom(int x, int y, int w, int hTxtLines, int hCmdLines, int _CellSpace, int BtnDiam);
    void setActiveNumber(bool Active, int Number);
    int ClickDispatch(QPoint p, int &number);
    int SysCmdClickDispatch(QPoint p);
    int PersonalRemoteClickDispatch(QPoint p);
    //static int getActiveCnt();
    bool isActive();
    int  GetVisualNumber();
    void SetVisualNumber(int n);
    void setType(int t);
    void CopySettingsTo(Vismo& vm);
    void setPB(QList<Saver>* _pb = NULL, int _pbIndex = -1);

    double getSumOfKSizePUindiv();

    int info(int i) {
        if (i==0)
            return moActive;
        else if (i==1)
            return mo07;
        else if (i==2)
            return getpbIndex();
    }

    int moWidth;        //ширина виджета
    int moHeight;       //высота виджета

    static int activePBGroup;

    int getpbIndex();

    static double getSumOfKSize();

    bool clickEnabled;
};

#endif // VISMO_H
