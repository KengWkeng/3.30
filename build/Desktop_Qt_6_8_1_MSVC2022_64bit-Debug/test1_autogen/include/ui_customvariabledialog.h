/********************************************************************************
** Form generated from reading UI file 'customvariabledialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CUSTOMVARIABLEDIALOG_H
#define UI_CUSTOMVARIABLEDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CustomVariableDialog
{
public:
    QVBoxLayout *verticalLayout_2;
    QSplitter *splitter;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QLabel *label_3;
    QListWidget *listWidgetDefinitions;
    QWidget *layoutWidget1;
    QGridLayout *gridLayout;
    QLabel *label;
    QLineEdit *lineEditVariableName;
    QLabel *label_2;
    QLineEdit *lineEditFormula;
    QPushButton *pushButtonAdd;
    QPushButton *pushButtonDelete;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *CustomVariableDialog)
    {
        if (CustomVariableDialog->objectName().isEmpty())
            CustomVariableDialog->setObjectName("CustomVariableDialog");
        CustomVariableDialog->resize(450, 400);
        verticalLayout_2 = new QVBoxLayout(CustomVariableDialog);
        verticalLayout_2->setObjectName("verticalLayout_2");
        splitter = new QSplitter(CustomVariableDialog);
        splitter->setObjectName("splitter");
        splitter->setOrientation(Qt::Horizontal);
        layoutWidget = new QWidget(splitter);
        layoutWidget->setObjectName("layoutWidget");
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        label_3 = new QLabel(layoutWidget);
        label_3->setObjectName("label_3");

        verticalLayout->addWidget(label_3);

        listWidgetDefinitions = new QListWidget(layoutWidget);
        listWidgetDefinitions->setObjectName("listWidgetDefinitions");

        verticalLayout->addWidget(listWidgetDefinitions);

        splitter->addWidget(layoutWidget);
        layoutWidget1 = new QWidget(splitter);
        layoutWidget1->setObjectName("layoutWidget1");
        gridLayout = new QGridLayout(layoutWidget1);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(layoutWidget1);
        label->setObjectName("label");

        gridLayout->addWidget(label, 0, 0, 1, 1);

        lineEditVariableName = new QLineEdit(layoutWidget1);
        lineEditVariableName->setObjectName("lineEditVariableName");

        gridLayout->addWidget(lineEditVariableName, 1, 0, 1, 2);

        label_2 = new QLabel(layoutWidget1);
        label_2->setObjectName("label_2");

        gridLayout->addWidget(label_2, 2, 0, 1, 1);

        lineEditFormula = new QLineEdit(layoutWidget1);
        lineEditFormula->setObjectName("lineEditFormula");

        gridLayout->addWidget(lineEditFormula, 3, 0, 1, 2);

        pushButtonAdd = new QPushButton(layoutWidget1);
        pushButtonAdd->setObjectName("pushButtonAdd");

        gridLayout->addWidget(pushButtonAdd, 4, 0, 1, 1);

        pushButtonDelete = new QPushButton(layoutWidget1);
        pushButtonDelete->setObjectName("pushButtonDelete");

        gridLayout->addWidget(pushButtonDelete, 4, 1, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        gridLayout->addItem(verticalSpacer, 5, 0, 1, 1);

        splitter->addWidget(layoutWidget1);

        verticalLayout_2->addWidget(splitter);

        buttonBox = new QDialogButtonBox(CustomVariableDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout_2->addWidget(buttonBox);


        retranslateUi(CustomVariableDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, CustomVariableDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, CustomVariableDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(CustomVariableDialog);
    } // setupUi

    void retranslateUi(QDialog *CustomVariableDialog)
    {
        CustomVariableDialog->setWindowTitle(QCoreApplication::translate("CustomVariableDialog", "\345\256\232\344\271\211\350\207\252\345\256\232\344\271\211\345\217\230\351\207\217 (D\351\200\232\351\201\223)", nullptr));
        label_3->setText(QCoreApplication::translate("CustomVariableDialog", "\345\267\262\345\256\232\344\271\211\345\217\230\351\207\217:", nullptr));
        label->setText(QCoreApplication::translate("CustomVariableDialog", "\345\217\230\351\207\217\345\220\215 (\345\246\202 D_0):", nullptr));
        label_2->setText(QCoreApplication::translate("CustomVariableDialog", "\345\205\254\345\274\217 (\344\275\277\347\224\250 A_n, B_n, C_n, D_m):", nullptr));
        pushButtonAdd->setText(QCoreApplication::translate("CustomVariableDialog", "\346\267\273\345\212\240/\346\233\264\346\226\260", nullptr));
        pushButtonDelete->setText(QCoreApplication::translate("CustomVariableDialog", "\345\210\240\351\231\244", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CustomVariableDialog: public Ui_CustomVariableDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CUSTOMVARIABLEDIALOG_H
