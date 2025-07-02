/********************************************************************************
** Form generated from reading UI file 'wlog.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WLOG_H
#define UI_WLOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_wLog
{
public:
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QTableWidget *tableWidget;
    QGridLayout *gridLayout_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton_2;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *verticalSpacer;
    QTextEdit *textEdit;

    void setupUi(QDialog *wLog)
    {
        if (wLog->objectName().isEmpty())
            wLog->setObjectName(QStringLiteral("wLog"));
        wLog->resize(867, 487);
        gridLayoutWidget = new QWidget(wLog);
        gridLayoutWidget->setObjectName(QStringLiteral("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(20, 20, 821, 441));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        tableWidget = new QTableWidget(gridLayoutWidget);
        if (tableWidget->columnCount() < 1)
            tableWidget->setColumnCount(1);
        tableWidget->setObjectName(QStringLiteral("tableWidget"));
        QFont font;
        font.setPointSize(16);
        tableWidget->setFont(font);
        tableWidget->setRowCount(0);
        tableWidget->setColumnCount(1);

        gridLayout->addWidget(tableWidget, 1, 0, 1, 4);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer, 0, 0, 1, 1);

        pushButton_2 = new QPushButton(gridLayoutWidget);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setFont(font);

        gridLayout_2->addWidget(pushButton_2, 0, 1, 1, 1);

        pushButton = new QPushButton(gridLayoutWidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setFont(font);

        gridLayout_2->addWidget(pushButton, 0, 2, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_2, 0, 3, 1, 1);

        verticalSpacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed);

        gridLayout_2->addItem(verticalSpacer, 1, 1, 1, 1);


        gridLayout->addLayout(gridLayout_2, 2, 0, 1, 4);

        textEdit = new QTextEdit(gridLayoutWidget);
        textEdit->setObjectName(QStringLiteral("textEdit"));
        textEdit->setFont(font);

        gridLayout->addWidget(textEdit, 0, 0, 1, 4);


        retranslateUi(wLog);

        QMetaObject::connectSlotsByName(wLog);
    } // setupUi

    void retranslateUi(QDialog *wLog)
    {
        wLog->setWindowTitle(QApplication::translate("wLog", "Dialog", Q_NULLPTR));
        pushButton_2->setText(QApplication::translate("wLog", "   \320\227\320\260\320\263\321\200\321\203\320\267\320\270\321\202\321\214 \320\273\320\276\320\263 \320\270\320\267 \321\204\320\260\320\271\320\273\320\260   ", Q_NULLPTR));
        pushButton->setText(QApplication::translate("wLog", "   \320\227\320\260\320\272\321\200\321\213\321\202\321\214   ", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class wLog: public Ui_wLog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WLOG_H
