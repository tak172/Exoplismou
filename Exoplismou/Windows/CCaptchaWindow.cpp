/*
    Exoplismou source code. Tak172. 2024.

    @Name:			CCaptchaWindow.cpp
    @Created:		30.09.2024
    @Programmer:	Timofey Kromachev

    Implementations. */

#include "stdafx.h"

#include "CCaptchaWindow.h"

#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QGridLayout>
#include <QMouseEvent>

CCaptchaWindow::CCaptchaWindow( QWidget * parent )
    : QDialog( parent )
{
    SetupWidgets();
    UpdateCaptcha();

    connect( _okBN, &QPushButton::clicked, this, &CCaptchaWindow::OnOK );
    connect(_updateCaptchaBN, &QPushButton::clicked, this, &CCaptchaWindow::UpdateCaptcha );
}

void CCaptchaWindow::OnOK()
{
    if ( _captchaLE->text().toUpper() == _captchaGenerator.captchaText() )
        accept();
    else
    {
        _captchaLE->clear();
        _captchaGenerator.generateText( 4 );
        _captchaViewTL->setPixmap( QPixmap::fromImage( _captchaGenerator.captchaImage() ) );
    }
}

void CCaptchaWindow::UpdateCaptcha()
{
    _captchaGenerator.generateText( 4 );
    _captchaViewTL->setPixmap( QPixmap::fromImage( _captchaGenerator.captchaImage() ) );
}

void CCaptchaWindow::SetupWidgets()
{
    setWindowTitle( QString::fromStdWString( L"Капча" ) );
    setFixedSize( 240, 178 );

    _captchaGenerator.setDifficulty( 3 );

    QGridLayout * mainGL = new QGridLayout;

    _captchaViewTL = new QLabel( this );
    _captchaViewTL->setStyleSheet( "background-color: white;" );
    _captchaViewTL->setScaledContents( true );

    _updateCaptchaBN = new QPushButton( QString::fromStdWString( L"Обновить" ), this );
    _updateCaptchaBN->setFixedWidth( 100 );

    _okBN = new QPushButton( QString::fromStdWString( L"OK" ), this );
    _okBN->setFixedWidth( 100 );

    _captchaLE = new QLineEdit( this );
    _captchaLE->setPlaceholderText( QString::fromStdWString( L"Введите капчу" ) );

    mainGL->addWidget( _captchaViewTL, 0, 0, 1, 2 );
    mainGL->addWidget( _captchaLE, 1, 0, 1, 2 ); 
    mainGL->addWidget( _updateCaptchaBN, 2, 0, 1, 1 ); 
    mainGL->addWidget( _okBN, 2, 1, 1, 1 ); 

    setLayout( mainGL );
}