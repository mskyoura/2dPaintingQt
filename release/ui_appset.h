/********************************************************************************
** Form generated from reading UI file 'appset.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_APPSET_H
#define UI_APPSET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Appset
{
public:
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QLabel *label_12;
    QHBoxLayout *horizontalLayout_4;
    QSpacerItem *horizontalSpacer_5;
    QComboBox *eMsg;
    QPushButton *clrLstBtn;
    QSpacerItem *horizontalSpacer_6;
    QHBoxLayout *horizontalLayout_6;
    QLineEdit *eDst;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer_3;
    QSpacerItem *horizontalSpacer_15;
    QPushButton *cancelBtn;
    QPushButton *okBtn;
    QSpacerItem *horizontalSpacer_14;
    QSpacerItem *horizontalSpacer_4;
    QLabel *label_3;
    QLabel *label_8;
    QHBoxLayout *horizontalLayout_7;
    QLineEdit *eT1;
    QLabel *label_10;
    QSpacerItem *horizontalSpacer_9;
    QSpacerItem *spacer_2;
    QSpacerItem *verticalSpacer_5;
    QSpacerItem *verticalSpacer_4;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout_8;
    QLineEdit *eT2;
    QLabel *label_9;
    QSpacerItem *horizontalSpacer_2;
    QGridLayout *gridLayout_2;
    QHBoxLayout *horizontalLayout_9;
    QWidget *wRYG;
    QSpacerItem *verticalSpacer_2;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *eU1;
    QLabel *label_11;
    QLineEdit *eU2;
    QLabel *label_6;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer_10;
    QLineEdit *eAnsw;
    QSpacerItem *horizontalSpacer_11;
    QLabel *label_2;
    QGridLayout *gridLayout_3;
    QSpacerItem *verticalSpacer_3;
    QLabel *label_4;
    QLabel *label_7;
    QLabel *label;
    QLineEdit *eID;
    QHBoxLayout *horizontalLayout_5;
    QSpacerItem *horizontalSpacer_7;
    QPushButton *sendBtn;
    QSpacerItem *horizontalSpacer_8;
    QComboBox *ePolarity;
    QHBoxLayout *horizontalLayout_10;
    QSpacerItem *horizontalSpacer_12;
    QLabel *label_5;
    QSpacerItem *horizontalSpacer_13;

    void setupUi(QDialog *Appset)
    {
        if (Appset->objectName().isEmpty())
            Appset->setObjectName(QStringLiteral("Appset"));
        Appset->resize(849, 544);
        gridLayoutWidget = new QWidget(Appset);
        gridLayoutWidget->setObjectName(QStringLiteral("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(10, 10, 1070, 566));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label_12 = new QLabel(gridLayoutWidget);
        label_12->setObjectName(QStringLiteral("label_12"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_12->sizePolicy().hasHeightForWidth());
        label_12->setSizePolicy(sizePolicy);
        QFont font;
        font.setPointSize(16);
        label_12->setFont(font);
        label_12->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_12, 15, 0, 1, 6);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalSpacer_5 = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_5);

        eMsg = new QComboBox(gridLayoutWidget);
        eMsg->setObjectName(QStringLiteral("eMsg"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(eMsg->sizePolicy().hasHeightForWidth());
        eMsg->setSizePolicy(sizePolicy1);
        eMsg->setFont(font);
        eMsg->setEditable(true);
        eMsg->setSizeAdjustPolicy(QComboBox::AdjustToContents);

        horizontalLayout_4->addWidget(eMsg);

        clrLstBtn = new QPushButton(gridLayoutWidget);
        clrLstBtn->setObjectName(QStringLiteral("clrLstBtn"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(clrLstBtn->sizePolicy().hasHeightForWidth());
        clrLstBtn->setSizePolicy(sizePolicy2);
        clrLstBtn->setFont(font);

        horizontalLayout_4->addWidget(clrLstBtn);

        horizontalSpacer_6 = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_6);


        gridLayout->addLayout(horizontalLayout_4, 14, 0, 1, 6);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        eDst = new QLineEdit(gridLayoutWidget);
        eDst->setObjectName(QStringLiteral("eDst"));
        eDst->setMaximumSize(QSize(16777215, 16777215));
        eDst->setFont(font);

        horizontalLayout_6->addWidget(eDst);


        gridLayout->addLayout(horizontalLayout_6, 1, 2, 1, 2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer_3 = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_3);

        horizontalSpacer_15 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_15);

        cancelBtn = new QPushButton(gridLayoutWidget);
        cancelBtn->setObjectName(QStringLiteral("cancelBtn"));
        sizePolicy2.setHeightForWidth(cancelBtn->sizePolicy().hasHeightForWidth());
        cancelBtn->setSizePolicy(sizePolicy2);
        cancelBtn->setMinimumSize(QSize(0, 40));
        cancelBtn->setFont(font);
        cancelBtn->setStyleSheet(QLatin1String("background-color: rgb(187, 2, 0);\n"
"color: rgb(255, 255, 255);"));

        horizontalLayout->addWidget(cancelBtn);

        okBtn = new QPushButton(gridLayoutWidget);
        okBtn->setObjectName(QStringLiteral("okBtn"));
        sizePolicy2.setHeightForWidth(okBtn->sizePolicy().hasHeightForWidth());
        okBtn->setSizePolicy(sizePolicy2);
        okBtn->setMinimumSize(QSize(0, 40));
        okBtn->setMaximumSize(QSize(250, 16777215));
        okBtn->setFont(font);
        okBtn->setAutoFillBackground(false);
        okBtn->setStyleSheet(QLatin1String("background-color: rgb(0, 175, 80);\n"
"color: rgb(255, 255, 255);"));

        horizontalLayout->addWidget(okBtn);

        horizontalSpacer_14 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_14);

        horizontalSpacer_4 = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_4);


        gridLayout->addLayout(horizontalLayout, 10, 0, 1, 6);

        label_3 = new QLabel(gridLayoutWidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        sizePolicy.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
        label_3->setSizePolicy(sizePolicy);
        label_3->setFont(font);
        label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_3, 4, 1, 1, 1);

        label_8 = new QLabel(gridLayoutWidget);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setFont(font);
        label_8->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_8, 1, 1, 1, 1);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        eT1 = new QLineEdit(gridLayoutWidget);
        eT1->setObjectName(QStringLiteral("eT1"));
        eT1->setMaximumSize(QSize(120, 16777215));
        eT1->setFont(font);

        horizontalLayout_7->addWidget(eT1);

        label_10 = new QLabel(gridLayoutWidget);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setFont(font);

        horizontalLayout_7->addWidget(label_10);

        horizontalSpacer_9 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer_9);


        gridLayout->addLayout(horizontalLayout_7, 3, 2, 1, 4);

        spacer_2 = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        gridLayout->addItem(spacer_2, 6, 0, 1, 1);

        verticalSpacer_5 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed);

        gridLayout->addItem(verticalSpacer_5, 19, 2, 1, 1);

        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Fixed);

        gridLayout->addItem(verticalSpacer_4, 11, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 19, 1, 1, 1);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        eT2 = new QLineEdit(gridLayoutWidget);
        eT2->setObjectName(QStringLiteral("eT2"));
        sizePolicy2.setHeightForWidth(eT2->sizePolicy().hasHeightForWidth());
        eT2->setSizePolicy(sizePolicy2);
        eT2->setMaximumSize(QSize(120, 16777215));
        eT2->setFont(font);

        horizontalLayout_8->addWidget(eT2);

        label_9 = new QLabel(gridLayoutWidget);
        label_9->setObjectName(QStringLiteral("label_9"));
        sizePolicy.setHeightForWidth(label_9->sizePolicy().hasHeightForWidth());
        label_9->setSizePolicy(sizePolicy);
        label_9->setFont(font);

        horizontalLayout_8->addWidget(label_9);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_8->addItem(horizontalSpacer_2);


        gridLayout->addLayout(horizontalLayout_8, 4, 2, 1, 4);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName(QStringLiteral("horizontalLayout_9"));
        wRYG = new QWidget(gridLayoutWidget);
        wRYG->setObjectName(QStringLiteral("wRYG"));
        wRYG->setMaximumSize(QSize(16777215, 30));

        horizontalLayout_9->addWidget(wRYG);


        gridLayout_2->addLayout(horizontalLayout_9, 1, 0, 1, 2);

        verticalSpacer_2 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed);

        gridLayout_2->addItem(verticalSpacer_2, 1, 2, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        eU1 = new QLineEdit(gridLayoutWidget);
        eU1->setObjectName(QStringLiteral("eU1"));
        sizePolicy2.setHeightForWidth(eU1->sizePolicy().hasHeightForWidth());
        eU1->setSizePolicy(sizePolicy2);
        eU1->setMaximumSize(QSize(120, 16777215));
        eU1->setFont(font);

        horizontalLayout_2->addWidget(eU1);

        label_11 = new QLabel(gridLayoutWidget);
        label_11->setObjectName(QStringLiteral("label_11"));
        sizePolicy.setHeightForWidth(label_11->sizePolicy().hasHeightForWidth());
        label_11->setSizePolicy(sizePolicy);
        label_11->setFont(font);

        horizontalLayout_2->addWidget(label_11);

        eU2 = new QLineEdit(gridLayoutWidget);
        eU2->setObjectName(QStringLiteral("eU2"));
        sizePolicy2.setHeightForWidth(eU2->sizePolicy().hasHeightForWidth());
        eU2->setSizePolicy(sizePolicy2);
        eU2->setMaximumSize(QSize(120, 16777215));
        eU2->setFont(font);

        horizontalLayout_2->addWidget(eU2);

        label_6 = new QLabel(gridLayoutWidget);
        label_6->setObjectName(QStringLiteral("label_6"));
        sizePolicy.setHeightForWidth(label_6->sizePolicy().hasHeightForWidth());
        label_6->setSizePolicy(sizePolicy);
        label_6->setFont(font);

        horizontalLayout_2->addWidget(label_6);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);


        gridLayout_2->addLayout(horizontalLayout_2, 0, 0, 1, 2);


        gridLayout->addLayout(gridLayout_2, 6, 2, 1, 3);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalSpacer_10 = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_10);

        eAnsw = new QLineEdit(gridLayoutWidget);
        eAnsw->setObjectName(QStringLiteral("eAnsw"));
        sizePolicy2.setHeightForWidth(eAnsw->sizePolicy().hasHeightForWidth());
        eAnsw->setSizePolicy(sizePolicy2);
        eAnsw->setFont(font);
        eAnsw->setReadOnly(true);

        horizontalLayout_3->addWidget(eAnsw);

        horizontalSpacer_11 = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_11);


        gridLayout->addLayout(horizontalLayout_3, 16, 0, 1, 6);

        label_2 = new QLabel(gridLayoutWidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setFont(font);
        label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_2, 3, 1, 1, 1);

        gridLayout_3 = new QGridLayout();
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        verticalSpacer_3 = new QSpacerItem(20, 25, QSizePolicy::Minimum, QSizePolicy::Fixed);

        gridLayout_3->addItem(verticalSpacer_3, 1, 0, 1, 1);

        label_4 = new QLabel(gridLayoutWidget);
        label_4->setObjectName(QStringLiteral("label_4"));
        sizePolicy.setHeightForWidth(label_4->sizePolicy().hasHeightForWidth());
        label_4->setSizePolicy(sizePolicy);
        label_4->setFont(font);
        label_4->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_3->addWidget(label_4, 0, 0, 1, 1);


        gridLayout->addLayout(gridLayout_3, 6, 1, 1, 1);

        label_7 = new QLabel(gridLayoutWidget);
        label_7->setObjectName(QStringLiteral("label_7"));
        sizePolicy.setHeightForWidth(label_7->sizePolicy().hasHeightForWidth());
        label_7->setSizePolicy(sizePolicy);
        label_7->setFont(font);
        label_7->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_7, 7, 1, 1, 1);

        label = new QLabel(gridLayoutWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setFont(font);
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label, 0, 1, 1, 1);

        eID = new QLineEdit(gridLayoutWidget);
        eID->setObjectName(QStringLiteral("eID"));
        eID->setMaximumSize(QSize(120, 16777215));
        eID->setFont(font);

        gridLayout->addWidget(eID, 0, 2, 1, 1);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_7);

        sendBtn = new QPushButton(gridLayoutWidget);
        sendBtn->setObjectName(QStringLiteral("sendBtn"));
        sizePolicy2.setHeightForWidth(sendBtn->sizePolicy().hasHeightForWidth());
        sendBtn->setSizePolicy(sizePolicy2);
        sendBtn->setMinimumSize(QSize(0, 40));
        sendBtn->setFont(font);
        sendBtn->setStyleSheet(QLatin1String("background-color: rgb(63, 63, 63);\n"
"color: rgb(255, 255, 255);"));

        horizontalLayout_5->addWidget(sendBtn);

        horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_8);


        gridLayout->addLayout(horizontalLayout_5, 18, 0, 1, 6);

        ePolarity = new QComboBox(gridLayoutWidget);
        ePolarity->setObjectName(QStringLiteral("ePolarity"));
        sizePolicy2.setHeightForWidth(ePolarity->sizePolicy().hasHeightForWidth());
        ePolarity->setSizePolicy(sizePolicy2);
        ePolarity->setFont(font);

        gridLayout->addWidget(ePolarity, 7, 2, 1, 2);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName(QStringLiteral("horizontalLayout_10"));
        horizontalSpacer_12 = new QSpacerItem(40, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_10->addItem(horizontalSpacer_12);

        label_5 = new QLabel(gridLayoutWidget);
        label_5->setObjectName(QStringLiteral("label_5"));
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(label_5->sizePolicy().hasHeightForWidth());
        label_5->setSizePolicy(sizePolicy3);
        label_5->setFont(font);
        label_5->setAlignment(Qt::AlignCenter);

        horizontalLayout_10->addWidget(label_5);

        horizontalSpacer_13 = new QSpacerItem(40, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_10->addItem(horizontalSpacer_13);


        gridLayout->addLayout(horizontalLayout_10, 13, 0, 1, 6);

        QWidget::setTabOrder(eID, ePolarity);

        retranslateUi(Appset);

        QMetaObject::connectSlotsByName(Appset);
    } // setupUi

    void retranslateUi(QDialog *Appset)
    {
        Appset->setWindowTitle(QApplication::translate("Appset", "Dialog", Q_NULLPTR));
        label_12->setText(QApplication::translate("Appset", "\320\236\321\202\320\262\320\265\321\202 \320\275\320\260 \320\277\321\200\320\276\320\270\320\267\320\262\320\276\320\273\321\214\320\275\321\203\321\216 \320\272\320\276\320\274\320\260\320\275\320\264\321\203:", Q_NULLPTR));
        clrLstBtn->setText(QApplication::translate("Appset", " \320\236\321\207\320\270\321\201\321\202\320\270\321\202\321\214 ", Q_NULLPTR));
        cancelBtn->setText(QApplication::translate("Appset", "   \320\236\321\202\320\274\320\265\320\275\320\270\321\202\321\214   ", Q_NULLPTR));
        okBtn->setText(QApplication::translate("Appset", "   \320\241\320\276\321\205\321\200\320\260\320\275\320\270\321\202\321\214   ", Q_NULLPTR));
        label_3->setText(QApplication::translate("Appset", "\320\227\320\260\320\264\320\265\321\200\320\266\320\272\320\260 \320\262\320\272\320\273\321\216\321\207\320\265\320\275\320\270\321\217 \321\200\320\265\320\273\320\265 \320\237\320\243\320\241\320\232 (0.0...120.0):", Q_NULLPTR));
        label_8->setText(QApplication::translate("Appset", "\320\235\320\260\320\267\320\262\320\260\320\275\320\270\320\265:", Q_NULLPTR));
        label_10->setText(QApplication::translate("Appset", "\321\201", Q_NULLPTR));
        label_9->setText(QApplication::translate("Appset", "\321\201", Q_NULLPTR));
        label_11->setText(QApplication::translate("Appset", "\320\222", Q_NULLPTR));
        label_6->setText(QApplication::translate("Appset", "\320\222", Q_NULLPTR));
        label_2->setText(QApplication::translate("Appset", "\320\222\321\200\320\265\320\274\321\217 \320\262\320\272\320\273\321\216\321\207\320\265\320\275\320\270\321\217 \321\200\320\265\320\273\320\265 \320\237\320\243\320\241\320\232 (0...255):", Q_NULLPTR));
        label_4->setText(QApplication::translate("Appset", "\320\237\320\276\321\200\320\276\320\263\320\270 \320\276\321\202\320\276\320\261\321\200\320\260\320\266\320\265\320\275\320\270\321\217 \321\206\320\262\320\265\321\202\320\260 \320\277\320\270\321\202\320\260\320\275\320\270\321\217 (0.0...15.0):", Q_NULLPTR));
        label_7->setText(QApplication::translate("Appset", "\320\237\320\276\320\273\321\217\321\200\320\275\320\276\321\201\321\202\321\214 \320\272\320\276\320\275\321\202\321\200\320\276\320\273\321\217 \321\201\320\276\320\277\321\200\320\276\321\202\320\270\320\262\320\273\320\265\320\275\320\270\321\217:", Q_NULLPTR));
        label->setText(QApplication::translate("Appset", "ID (0...FE):", Q_NULLPTR));
        sendBtn->setText(QApplication::translate("Appset", "\320\236\321\202\320\277\321\200\320\260\320\262\320\270\321\202\321\214", Q_NULLPTR));
        label_5->setText(QApplication::translate("Appset", "\320\236\321\202\320\277\321\200\320\260\320\262\320\272\320\260 \320\277\321\200\320\276\320\270\320\267\320\262\320\276\320\273\321\214\320\275\320\276\320\271 \320\272\320\276\320\274\320\260\320\275\320\264\321\213:", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class Appset: public Ui_Appset {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_APPSET_H
