/********************************************************************************
** Form generated from reading UI file 'xPlay.ui'
**
** Created by: Qt User Interface Compiler version 5.12.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_XPLAY_H
#define UI_XPLAY_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include "videowidget.h"
#include "xslider.h"

QT_BEGIN_NAMESPACE

class Ui_xPlayClass
{
public:
    QLabel *playTime;
    QLabel *totalTime;
    VideoWidget *openGLWidget;
    XSlider *playSlider;
    QPushButton *openBtn;
    QPushButton *playBtn;

    void setupUi(QWidget *xPlayClass)
    {
        if (xPlayClass->objectName().isEmpty())
            xPlayClass->setObjectName(QString::fromUtf8("xPlayClass"));
        xPlayClass->resize(800, 600);
        xPlayClass->setMinimumSize(QSize(800, 600));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/xPlay/Resources/logo.ico"), QSize(), QIcon::Normal, QIcon::Off);
        xPlayClass->setWindowIcon(icon);
        playTime = new QLabel(xPlayClass);
        playTime->setObjectName(QString::fromUtf8("playTime"));
        playTime->setGeometry(QRect(30, 580, 90, 20));
        playTime->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"font: 12pt \"\351\273\221\344\275\223\";"));
        totalTime = new QLabel(xPlayClass);
        totalTime->setObjectName(QString::fromUtf8("totalTime"));
        totalTime->setGeometry(QRect(120, 580, 80, 20));
        totalTime->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);\n"
"font: 12pt \"\351\273\221\344\275\223\";"));
        openGLWidget = new VideoWidget(xPlayClass);
        openGLWidget->setObjectName(QString::fromUtf8("openGLWidget"));
        openGLWidget->setGeometry(QRect(0, 0, 800, 600));
        playSlider = new XSlider(xPlayClass);
        playSlider->setObjectName(QString::fromUtf8("playSlider"));
        playSlider->setGeometry(QRect(30, 530, 731, 22));
        playSlider->setMaximum(999);
        playSlider->setOrientation(Qt::Horizontal);
        openBtn = new QPushButton(xPlayClass);
        openBtn->setObjectName(QString::fromUtf8("openBtn"));
        openBtn->setGeometry(QRect(470, 570, 61, 28));
        openBtn->setStyleSheet(QString::fromUtf8("QPushButton:!hover{border-image: url(:/xPlay/Resources/open_normal.png);}\n"
"QPushButton:hover{border-image: url(:/xPlay/Resources/open_hot.png);}"));
        playBtn = new QPushButton(xPlayClass);
        playBtn->setObjectName(QString::fromUtf8("playBtn"));
        playBtn->setGeometry(QRect(680, 570, 51, 28));
        playBtn->setStyleSheet(QString::fromUtf8("QPushButton:!hover{border-image: url(:/xPlay/Resources/play_normal.png);}\n"
"QPushButton:hover{border-image: url(:/xPlay/Resources/play_hot.png);}"));
        openGLWidget->raise();
        playTime->raise();
        totalTime->raise();
        playSlider->raise();
        totalTime->raise();
        openBtn->raise();
        playBtn->raise();

        retranslateUi(xPlayClass);
        QObject::connect(openBtn, SIGNAL(clicked()), xPlayClass, SLOT(open()));
        QObject::connect(playBtn, SIGNAL(clicked()), xPlayClass, SLOT(play()));
        QObject::connect(playSlider, SIGNAL(sliderPressed()), xPlayClass, SLOT(sliderPress()));
        QObject::connect(playSlider, SIGNAL(sliderReleased()), xPlayClass, SLOT(sliderRelease()));

        QMetaObject::connectSlotsByName(xPlayClass);
    } // setupUi

    void retranslateUi(QWidget *xPlayClass)
    {
        xPlayClass->setWindowTitle(QApplication::translate("xPlayClass", "xPlay", nullptr));
        playTime->setText(QApplication::translate("xPlayClass", "00:00:00/", nullptr));
        totalTime->setText(QApplication::translate("xPlayClass", "00:00:00", nullptr));
        openBtn->setText(QString());
        playBtn->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class xPlayClass: public Ui_xPlayClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_XPLAY_H
