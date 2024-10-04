/*
    Exoplismou source code. Tak172. 2024.

    @Name:			CAuthentificationWindow.h
    @Created:		30.09.2024
    @Programmer:	Timofey Kromachev

    Class of authentification window. */

#pragma once

#include <QDialog>
#include <QTime>

namespace DatabaseProcessing
{
    struct SUserData;
}

class QPushButton;
class QLineEdit;
class QLabel;
class QTimer;
class CPasswordEditWidget;
class CAuthentificationWindow : public QDialog
{
public:
    CAuthentificationWindow( QWidget * parent = nullptr );

private slots:
    void OnOK();
    void OnExit();
    void UpdateTimer();

private:
    void SetupWidgets();

    QPushButton * _okBN;
    QPushButton * _exitBN;
    QLineEdit * _loginLE;
    CPasswordEditWidget * _passwordLE;
    QLabel * _warningTL;

    QTimer * _timer;
    QTime _remainingTime;

    unsigned _incorrectPasswordCount;
};