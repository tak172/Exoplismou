/*
    Exoplismou source code. Tak172. 2024.

    @Name:			CAuthentificationWindow.cpp
    @Created:		30.09.2024
    @Programmer:	Timofey Kromachev

    Implementations. */

#include "stdafx.h"

#include "CAuthentificationWindow.h"

#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QGridLayout>
#include <QEvent>

#include "../Database/CDatabaseManager.h"
#include "../Widgets/CPasswordEditWidget.h"
#include "../Global.h"
#include "CCaptchaWindow.h"

CAuthentificationWindow::CAuthentificationWindow( QWidget * parent )
    : QDialog( parent ), _incorrectPasswordCount( 0 )
{
    SetupWidgets();

    connect( _okBN, &QPushButton::clicked, this, &CAuthentificationWindow::OnOK );
    connect( _exitBN, &QPushButton::clicked, this, &CAuthentificationWindow::OnExit );
    connect( _timer, &QTimer::timeout, this, &CAuthentificationWindow::UpdateTimer );
}

void CAuthentificationWindow::SetupWidgets()
{
    setWindowTitle( QString::fromStdWString( L"Авторизация" ) );
    setFixedSize( 360, 140 );

    _timer = new QTimer( this );
    QLabel * loginTL = new QLabel( QString::fromStdWString( L"Введите логин:" ), this );
    QLabel * passwordTL = new QLabel( QString::fromStdWString( L"Введите пароль:" ), this );

    _loginLE = new QLineEdit( this );
    _passwordLE = new CPasswordEditWidget( this );

    _okBN = new QPushButton( this );
    _okBN->setFixedWidth( 110 );
    _okBN->setText( QString::fromStdWString( L"OK" ) );

    _exitBN = new QPushButton( this );
    _exitBN->setMinimumWidth( 110 );
    _exitBN->setText( QString::fromStdWString( L"Отмена" ) );

    _warningTL = new QLabel( this );

    QGridLayout * centralGL = new QGridLayout;
    centralGL->addWidget( loginTL, 0, 0, 1, 1 );
    centralGL->addWidget( passwordTL, 1, 0, 1, 1 );
    centralGL->addWidget( _loginLE, 0, 1, 1, 3 );
    centralGL->addWidget( _passwordLE, 1, 1, 1, 3 );
    centralGL->addWidget( _okBN, 3, 0, 1, 1 );
    QSpacerItem * tempSI = new QSpacerItem( 40, 20, QSizePolicy::Expanding );
    centralGL->addItem( tempSI, 3, 2, 1, 1 );
    centralGL->addWidget( _exitBN, 3, 3, 1, 1 );
    centralGL->addWidget( _warningTL, 2, 0, 1, 4 );
    setLayout( centralGL );
}

void CAuthentificationWindow::OnOK()
{
    if ( _loginLE->text().size() < 3 || _loginLE->text().size() > 15 )
    {
        _warningTL->setText( QString::fromStdWString( L"Логин должен содержать от 3 до 15 символов." ) );
        return;
    }

    if ( _passwordLE->GetPassword().size() < 3 || _passwordLE->GetPassword().size() > 15 )
    {
        _warningTL->setText( QString::fromStdWString( L"Пароль должен содержать от 3 до 15 символов." ) );
        return;
    }

    auto DBInstance = DatabaseProcessing::CDatabaseManager::Instance();
    DatabaseProcessing::SUserData * userData = DBInstance->GetUserData( _loginLE->text() );
    if ( userData == nullptr )
    {
        _warningTL->setText( QString::fromStdWString( L"Пользователь \'%1\' не найден." ).arg( _loginLE->text() ) );
        return;
    }

    _warningTL->clear();
    if ( _incorrectPasswordCount > 0 )
    {
        CCaptchaWindow captchaWindow;
        if ( captchaWindow.exec() == QDialog::Rejected )
            return;
    }

    if ( userData->_password != _passwordLE->GetPassword() )
    {
        ++_incorrectPasswordCount;
        _warningTL->setText( QString::fromStdWString( L"Неверный пароль для пользователя \'%1\'." ).arg( _loginLE->text() ) );
        DBInstance->AddAuthentification( userData, false );
        if ( _incorrectPasswordCount == 2 )
        {
            _remainingTime = QTime( 0, 3, 0 );
            _warningTL->setText( QString::fromStdWString( L"Неверный пароль. До попыдки осталось: 3:00" ) );
            _timer->start( 1000 );
            _okBN->setEnabled( false );
        }
        else if ( _incorrectPasswordCount > 2 )
        {
            _warningTL->setText( QString::fromStdWString( L"Неверный пароль. Вход заблокирован." ) );
            _okBN->setEnabled( false );
        }

        return;
    }

    _currentUser.reset( userData );
    DBInstance->AddAuthentification( _currentUser.get(), true );
    done( QDialog::Accepted );
}

void CAuthentificationWindow::OnExit()
{
    done( QDialog::Rejected );
}

void CAuthentificationWindow::UpdateTimer()
{
    _remainingTime = _remainingTime.addSecs( -1 );
    QString timeString = _remainingTime.toString( "m:ss" );
    _warningTL->setText( QString::fromStdWString( L"Неверный пароль. До попытки осталось: " ) + timeString );

    if ( _remainingTime <= QTime( 0, 0 ) )
    {
        _timer->stop();
        _warningTL->setText( "" );
        _okBN->setEnabled( true );
    }
}