/********************************************************************************
** Form generated from reading UI file 'wabout.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WABOUT_H
#define UI_WABOUT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_wAbout
{
public:
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer_7;
    QTextEdit *textEdit;
    QSpacerItem *horizontalSpacer_8;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer_3;
    QLabel *label;
    QSpacerItem *horizontalSpacer_4;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QSpacerItem *horizontalSpacer_5;
    QPushButton *okBtn;
    QSpacerItem *horizontalSpacer_6;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *verticalSpacer_2;
    QSpacerItem *verticalSpacer_3;

    void setupUi(QDialog *wAbout)
    {
        if (wAbout->objectName().isEmpty())
            wAbout->setObjectName(QStringLiteral("wAbout"));
        wAbout->resize(700, 439);
        gridLayoutWidget = new QWidget(wAbout);
        gridLayoutWidget->setObjectName(QStringLiteral("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(140, 50, 587, 302));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalSpacer_7 = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_7);

        textEdit = new QTextEdit(gridLayoutWidget);
        textEdit->setObjectName(QStringLiteral("textEdit"));
        QFont font;
        font.setPointSize(16);
        textEdit->setFont(font);
        textEdit->setReadOnly(true);

        horizontalLayout_2->addWidget(textEdit);

        horizontalSpacer_8 = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_8);


        gridLayout->addLayout(horizontalLayout_2, 2, 0, 1, 2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalSpacer_3 = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);

        label = new QLabel(gridLayoutWidget);
        label->setObjectName(QStringLiteral("label"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);
        label->setFont(font);

        horizontalLayout_3->addWidget(label);

        horizontalSpacer_4 = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_4);


        gridLayout->addLayout(horizontalLayout_3, 1, 0, 1, 2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_5);

        okBtn = new QPushButton(gridLayoutWidget);
        okBtn->setObjectName(QStringLiteral("okBtn"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(okBtn->sizePolicy().hasHeightForWidth());
        okBtn->setSizePolicy(sizePolicy1);
        okBtn->setMinimumSize(QSize(0, 40));
        okBtn->setFont(font);
        okBtn->setAutoFillBackground(false);
        okBtn->setStyleSheet(QLatin1String("background-color: rgb(0, 175, 80);\n"
"color: rgb(255, 255, 255);"));

        horizontalLayout->addWidget(okBtn);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_6);

        horizontalSpacer_2 = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        gridLayout->addLayout(horizontalLayout, 3, 0, 1, 2);

        verticalSpacer_2 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed);

        gridLayout->addItem(verticalSpacer_2, 4, 0, 1, 2);

        verticalSpacer_3 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed);

        gridLayout->addItem(verticalSpacer_3, 0, 0, 1, 1);


        retranslateUi(wAbout);

        QMetaObject::connectSlotsByName(wAbout);
    } // setupUi

    void retranslateUi(QDialog *wAbout)
    {
        wAbout->setWindowTitle(QApplication::translate("wAbout", "Dialog", Q_NULLPTR));
        label->setText(QApplication::translate("wAbout", "\320\237\321\200\320\276\320\263\321\200\320\260\320\274\320\274\320\260 \321\203\320\277\321\200\320\260\320\262\320\273\320\265\320\275\320\270\321\217 \302\253\320\230\320\275\320\270\321\206\320\270\320\260\321\202\320\276\321\200 2017\302\273\n"
"\320\222\320\265\321\200\321\201\320\270\321\217: 27.02.21\n"
"\320\236\320\236\320\236 \320\235\320\237\320\237 \302\253\320\232\320\276\320\275\321\202\320\260\320\272\321\202-\320\222\320\240\302\273", Q_NULLPTR));
        okBtn->setText(QApplication::translate("wAbout", "  \320\237\321\200\320\270\320\275\321\217\321\202\321\214  ", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class wAbout: public Ui_wAbout {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WABOUT_H
