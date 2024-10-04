/*
    Exoplismou source code. Tak172. 2024.

    @Name:			CEditUserDataWidget.cpp
    @Created:		01.10.2024
    @Programmer:	Timofey Kromachev

    Implementations. */

#include "stdafx.h"

#include "CEditUserDataWidget.h"

#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QFormLayout>
#include <QMessageBox>
#include <QFileDialog>

#include "../Global.h"
#include "../Database/CDatabaseManager.h"
#include "CPasswordEditWidget.h"

CEditUserDataWidget::CEditUserDataWidget( QWidget * parent )
    : QWidget( parent )
{
    SetupWidgets();

    connect( _saveBN, &QPushButton::clicked, this, &CEditUserDataWidget::OnSaveClicked );
    connect( _photoBN, &QPushButton::clicked, this, &CEditUserDataWidget::OnPhotoClicked );
    connect( _exitBN, &QPushButton::clicked, this, &CEditUserDataWidget::OnExit );
}

void CEditUserDataWidget::SetupWidgets()
{
    QFormLayout * mainFL = new QFormLayout;

    _loginLE = new QLineEdit( this );
    _passwordLE = new CPasswordEditWidget( this );
    _phoneLE = new QLineEdit( this );
    _phoneLE->setInputMask( "8(999)999-99-99;_" );

    _fullNameLE = new QLineEdit( this );
    _photoLabel = new QLabel( this );
    _photoLabel->setFixedSize( 100, 100 );
    _photoBN = new QPushButton( QString::fromStdWString( L"Выбрать фото..." ), this );
    _saveBN = new QPushButton( QString::fromStdWString( L"Сохранить" ), this );

    {
        QHBoxLayout * exitHL = new QHBoxLayout;

        _exitBN = new QPushButton( QString::fromStdWString( L"Меню" ) );
        _exitBN->setFixedWidth( 100 );

        QSpacerItem * leftHS = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );

        exitHL->addItem( leftHS );
        exitHL->addWidget( _exitBN );

        mainFL->addItem( exitHL );
    }

    mainFL->addRow( QString::fromStdWString( L"Логин:" ), _loginLE );
    mainFL->addRow( QString::fromStdWString( L"Пароль:" ), _passwordLE );
    mainFL->addRow( QString::fromStdWString( L"Номер телефона:" ), _phoneLE );
    mainFL->addRow( QString::fromStdWString( L"ФИО:" ), _fullNameLE );
    mainFL->addRow( QString::fromStdWString( L"Фото профиля:" ), _photoLabel );
    mainFL->addRow( "", _photoBN );
    mainFL->addWidget( _saveBN );

    setLayout( mainFL );
}

void CEditUserDataWidget::UpdateUserData( EEditType type )
{
    _type = type;

    QImage image;
    if ( _type == EEditType::EditUser )
    {
        _loginLE->setText( _currentUser->_login );
        _passwordLE->SetText( _currentUser->_password );
        _phoneLE->setText( _currentUser->_phone );
        _fullNameLE->setText( _currentUser->_fullName );
        if ( !image.loadFromData( _currentUser->_photo, "PNG" ) )
            image.load( ":/Icons/no_photo" );
    }
    else
    {
        _loginLE->setText( "" );
        _passwordLE->SetText( "" );
        _phoneLE->setText( "" );
        _fullNameLE->setText( "" );
        image.load( ":/Icons/no_photo" );
    }

    _photoLabel->setPixmap( QPixmap::fromImage( image ).scaled( _photoLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
}

void CEditUserDataWidget::OnSaveClicked()
{
    DatabaseProcessing::CDatabaseManager * DBInstance = DatabaseProcessing::CDatabaseManager::Instance();
    if ( _loginLE->text() != _currentUser->_login && DBInstance->IsLoginUnique( _loginLE->text() ) == false )
    {
        QString message = QString::fromStdWString( L"Пользователь с таким логином уже существует." );
        QMessageBox messageBox;
        messageBox.setWindowTitle( QString::fromStdWString( L"Ошибка сохранения" ) );
        messageBox.setText( message );
        messageBox.setIcon( QMessageBox::Warning );
        messageBox.setStandardButtons( QMessageBox::Ok );
        if ( QAbstractButton * okButton = messageBox.button( QMessageBox::Ok ) )
            okButton->setMinimumWidth( 100 );

        messageBox.exec();
        return;
    }

    if ( _loginLE->text().isEmpty() || _passwordLE->GetPassword().isEmpty() || 
        _phoneLE->text().isEmpty() || _fullNameLE->text().isEmpty() )
    {
        QString message = QString::fromStdWString( L"Заполните все поля ввода." );
        QMessageBox messageBox;
        messageBox.setWindowTitle( QString::fromStdWString( L"Ошибка сохранения" ) );
        messageBox.setText( message );
        messageBox.setIcon( QMessageBox::Warning );
        messageBox.setStandardButtons( QMessageBox::Ok );
        if ( QAbstractButton * okButton = messageBox.button( QMessageBox::Ok ) )
            okButton->setMinimumWidth( 100 );

        messageBox.exec();
        return;
    }

    DatabaseProcessing::SUserData tempData;
    if ( _type == EEditType::EditUser )
    {
        tempData = *_currentUser;
    }

    tempData._login = _loginLE->text();
    tempData._password = _passwordLE->GetPassword();
    tempData._phone = _phoneLE->text();
    tempData._fullName = _fullNameLE->text();

    tempData._photo.clear();
    QImage image = _photoLabel->pixmap( Qt::ReturnByValue ).toImage();
    QBuffer buffer( &tempData._photo );
    buffer.open( QIODevice::WriteOnly );
    image.save( &buffer, "PNG" );
    buffer.close();

    bool result = false;
    if ( _type == EEditType::EditUser )
    {
        result = DBInstance->SetUserData( _currentUser.get(), &tempData );
    }
    else
        result = DBInstance->AddMaster( &tempData );

    if ( result )
    {
        QMessageBox::information( this, QString::fromStdWString( L"Сохранение" ),
            QString::fromStdWString( L"Сохранение данных пользователя прошло успешно!" ) );

        emit Exit();
    }
    else
    {
        QString message = QString::fromStdWString( L"Не удалось сохранить данные пользователя." );
        QMessageBox messageBox;
        messageBox.setWindowTitle( QString::fromStdWString( L"Ошибка сохранения" ) );
        messageBox.setText( message );
        messageBox.setIcon( QMessageBox::Warning );
        messageBox.setStandardButtons( QMessageBox::Ok );
        if ( QAbstractButton * okButton = messageBox.button( QMessageBox::Ok ) )
            okButton->setMinimumWidth( 100 );

        messageBox.exec();
        return;
    }
}

void CEditUserDataWidget::OnPhotoClicked()
{
    QString fileName = QFileDialog::getOpenFileName( this, QString::fromStdWString(L"Открыть изображение"), "", "Image Files (*.png)");
    if ( !fileName.isEmpty() )
    {
        QImage image( fileName );
        if ( !image.isNull() )
            _photoLabel->setPixmap( QPixmap::fromImage( image ).scaled( _photoLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
    }
}

void CEditUserDataWidget::OnExit()
{
    emit Exit();
}
