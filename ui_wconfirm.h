/********************************************************************************
** Form generated from reading UI file 'wconfirm.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WCONFIRM_H
#define UI_WCONFIRM_H

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
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_wConfirm
{
public:
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QSpacerItem *verticalSpacer_2;
    QSpacerItem *verticalSpacer_3;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *cancelBtn;
    QPushButton *okBtn;
    QSpacerItem *horizontalSpacer_2;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer_4;
    QLabel *label;
    QSpacerItem *horizontalSpacer_3;

    void setupUi(QDialog *wConfirm)
    {
        if (wConfirm->objectName().isEmpty())
            wConfirm->setObjectName(QStringLiteral("wConfirm"));
        wConfirm->resize(404, 250);
        gridLayoutWidget = new QWidget(wConfirm);
        gridLayoutWidget->setObjectName(QStringLiteral("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(9, 9, 381, 261));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        verticalSpacer_2 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed);

        gridLayout->addItem(verticalSpacer_2, 5, 0, 1, 2);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer_3, 3, 0, 1, 2);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 0, 0, 1, 2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        cancelBtn = new QPushButton(gridLayoutWidget);
        cancelBtn->setObjectName(QStringLiteral("cancelBtn"));
        cancelBtn->setMinimumSize(QSize(0, 40));
        QFont font;
        font.setPointSize(16);
        cancelBtn->setFont(font);
        cancelBtn->setStyleSheet(QLatin1String("background-color: rgb(187, 2, 0);\n"
"color: rgb(255, 255, 255);"));

        horizontalLayout->addWidget(cancelBtn);

        okBtn = new QPushButton(gridLayoutWidget);
        okBtn->setObjectName(QStringLiteral("okBtn"));
        okBtn->setMinimumSize(QSize(100, 40));
        okBtn->setBaseSize(QSize(0, 0));
        okBtn->setFont(font);
        okBtn->setAutoFillBackground(false);
        okBtn->setStyleSheet(QLatin1String("background-color: rgb(0, 175, 80);\n"
"color: rgb(255, 255, 255);"));

        horizontalLayout->addWidget(okBtn);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        gridLayout->addLayout(horizontalLayout, 4, 0, 1, 2);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_4);

        label = new QLabel(gridLayoutWidget);
        label->setObjectName(QStringLiteral("label"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);
        label->setFont(font);
        label->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(label);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);


        gridLayout->addLayout(horizontalLayout_2, 2, 0, 1, 2);


        retranslateUi(wConfirm);

        QMetaObject::connectSlotsByName(wConfirm);
    } // setupUi

    void retranslateUi(QDialog *wConfirm)
    {
        wConfirm->setWindowTitle(QApplication::translate("wConfirm", "Dialog", Q_NULLPTR));
        cancelBtn->setText(QApplication::translate("wConfirm", " \320\236\321\202\320\274\320\265\320\275\320\270\321\202\321\214 ", Q_NULLPTR));
        okBtn->setText(QApplication::translate("wConfirm", " \320\237\321\200\320\270\320\275\321\217\321\202\321\214 ", Q_NULLPTR));
        label->setText(QApplication::translate("wConfirm", "TextLabel", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class wConfirm: public Ui_wConfirm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WCONFIRM_H
