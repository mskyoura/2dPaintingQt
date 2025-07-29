/********************************************************************************
** Form generated from reading UI file 'pbsetup.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PBSETUP_H
#define UI_PBSETUP_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>

QT_BEGIN_NAMESPACE

class Ui_PBsetup
{
public:

    void setupUi(QDialog *PBsetup)
    {
        if (PBsetup->objectName().isEmpty())
            PBsetup->setObjectName(QStringLiteral("PBsetup"));
        PBsetup->setWindowModality(Qt::NonModal);
        PBsetup->resize(400, 300);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(PBsetup->sizePolicy().hasHeightForWidth());
        PBsetup->setSizePolicy(sizePolicy);

        retranslateUi(PBsetup);

        QMetaObject::connectSlotsByName(PBsetup);
    } // setupUi

    void retranslateUi(QDialog *PBsetup)
    {
        PBsetup->setWindowTitle(QApplication::translate("PBsetup", "\320\233\320\276\320\263 \320\272\320\276\320\274\320\260\320\275\320\264", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class PBsetup: public Ui_PBsetup {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PBSETUP_H
