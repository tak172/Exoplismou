/*
    Exoplismou source code. Tak172. 2024.

    @Name:			CCaptchaWindow.h
    @Created:		30.09.2024
    @Programmer:	Timofey Kromachev

    Class of captcha window. */

#pragma once

#include <QDialog>

#include "../Helpful/captcha.h"

class QPushButton;
class QLabel;
class QLineEdit;
class CCaptchaWindow : public QDialog
{
public:
    CCaptchaWindow( QWidget * parent = nullptr );

private slots:
    void OnOK();
    void UpdateCaptcha();

private:
    void SetupWidgets();

    QLabel * _captchaViewTL;
    QPushButton * _updateCaptchaBN;
    QPushButton * _okBN;
    QLineEdit * _captchaLE;

    Captcha _captchaGenerator;
};