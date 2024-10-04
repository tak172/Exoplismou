/*
    Exoplismou source code. Tak172. 2024.

    @Name:			CAddCommentWindow.cpp
    @Created:		02.10.2024
    @Programmer:	Timofey Kromachev

    Implementations. */

#include "stdafx.h"

#include "CAddCommentWindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QMessageBox>

#include "../Database/CDatabaseManager.h"

CAddCommentWindow::CAddCommentWindow( int requestID, QWidget * parent )
    : QDialog( parent ), _requestID( requestID )
{
    SetupWidgets();

    connect( _okBN, &QPushButton::clicked, this, &CAddCommentWindow::OnOk );
    connect( _cancelBN, &QPushButton::clicked, this, &CAddCommentWindow::reject );
}

void CAddCommentWindow::OnOk()
{
    if ( _commentTE->toPlainText().isEmpty() )
    {
        QString message = QString::fromStdWString( L"Поле отчета не может быть пустым." );
        QMessageBox messageBox;
        messageBox.setWindowTitle( QString::fromStdWString( L"Ошибка создания отчета" ) );
        messageBox.setText( message );
        messageBox.setIcon( QMessageBox::Warning );
        messageBox.setStandardButtons( QMessageBox::Ok );
        if ( QAbstractButton * okButton = messageBox.button( QMessageBox::Ok ) )
            okButton->setMinimumWidth( 100 );

        messageBox.exec();
        return;
    }

    DatabaseProcessing::CDatabaseManager * DBInstance = DatabaseProcessing::CDatabaseManager::Instance();
    DBInstance->AddMasterComment( DBInstance->GetRequestData( _requestID ) );
    accept();
}

void CAddCommentWindow::SetupWidgets()
{
    DatabaseProcessing::CDatabaseManager * DBInstance = DatabaseProcessing::CDatabaseManager::Instance();
    std::shared_ptr<DatabaseProcessing::SRequestData> requestData( DBInstance->GetRequestData( _requestID ) );

    if ( requestData == nullptr )
    {
errorRequest:
        QString message = QString::fromStdWString( L"Ошибка получения данных." );
        QMessageBox messageBox;
        messageBox.setWindowTitle( QString::fromStdWString( L"Ошибка запроса" ) );
        messageBox.setText( message );
        messageBox.setIcon( QMessageBox::Warning );
        messageBox.setStandardButtons( QMessageBox::Ok );
        if ( QAbstractButton * okButton = messageBox.button( QMessageBox::Ok ) )
            okButton->setMinimumWidth( 100 );

        messageBox.exec();
        reject();
        return;
    }

    std::shared_ptr <DatabaseProcessing::SUserData> userData( DBInstance->GetUserData( requestData->_clientID ) );
    if ( userData == nullptr )
        goto errorRequest;

    std::shared_ptr <DatabaseProcessing::SUserData> masterData( DBInstance->GetUserData( requestData->_masterID ) );
    if ( masterData == nullptr )
        goto errorRequest;

    std::shared_ptr <DatabaseProcessing::SDeviceModel> modelData( DBInstance->GetDeviceModelData( requestData->_modelID ) );
    if ( modelData == nullptr )
        goto errorRequest;

    std::shared_ptr <DatabaseProcessing::SDeviceType> typeData( DBInstance->GetDeviceTypeData( modelData->_typeID ) );
    if ( typeData == nullptr )
        goto errorRequest;

    QVBoxLayout * mainVL = new QVBoxLayout;

    QHBoxLayout * clientHL = new QHBoxLayout;
    QLabel * clientTL = new QLabel( QString::fromStdWString( L"ФИО клиента:" ), this );
    _userNameLE = new QLineEdit( this );
    _userNameLE->setEnabled( false );
    _userNameLE->setText( userData->_fullName );
    clientHL->addWidget( clientTL );
    clientHL->addWidget( _userNameLE );

    QHBoxLayout * masterHL = new QHBoxLayout;
    QLabel * masterTL = new QLabel( QString::fromStdWString( L"ФИО мастера:" ), this );
    _masterNameLE = new QLineEdit( this );
    _masterNameLE->setEnabled( false );
    _masterNameLE->setText( masterData->_fullName );
    masterHL->addWidget( masterTL );
    masterHL->addWidget( _masterNameLE );

    QHBoxLayout * modelHL = new QHBoxLayout;
    QLabel * modelTL = new QLabel( QString::fromStdWString( L"Модель:" ), this );
    _modelLE = new QLineEdit( this );
    _modelLE->setEnabled( false );
    _modelLE->setText( modelData->_name );
    modelHL->addWidget( modelTL );
    modelHL->addWidget( _modelLE );

    QHBoxLayout * typeHL = new QHBoxLayout;
    QLabel * typeTL = new QLabel( QString::fromStdWString( L"Тип:" ), this );
    _typeLE = new QLineEdit( this );
    _typeLE->setEnabled( false );
    _typeLE->setText( typeData->_name );
    typeHL->addWidget( typeTL );
    typeHL->addWidget( _typeLE );

    QHBoxLayout * dateHL = new QHBoxLayout;
    QLabel * startDateTL = new QLabel( QString::fromStdWString( L"Начало заказа:" ), this );
    QLabel * finishDateTL = new QLabel( QString::fromStdWString( L"Конец заказа:" ), this );
    QLabel * startDateValueTL = new QLabel( requestData->_startDate.toString( "dd.MM.yyyy" ), this );
    QLabel * finishDateValueTL = new QLabel( QDate::currentDate().toString( "dd.MM.yyyy" ), this );
    dateHL->addWidget( startDateTL );
    dateHL->addWidget( startDateValueTL );
    dateHL->addSpacing( 20 );
    dateHL->addWidget( finishDateTL );
    dateHL->addWidget( finishDateValueTL );

    QLabel * commentTL = new QLabel( QString::fromStdWString( L"Отчет:" ), this );
    _commentTE = new QTextEdit( this );

    QHBoxLayout * buttonHL = new QHBoxLayout;
    _okBN = new QPushButton( QString::fromStdWString( L"OK" ), this );
    _cancelBN = new QPushButton( QString::fromStdWString( L"Отмена" ), this );
    buttonHL->addStretch();
    buttonHL->addWidget( _okBN );
    buttonHL->addWidget( _cancelBN );

    mainVL->addLayout( clientHL );
    mainVL->addLayout( masterHL );
    mainVL->addLayout( modelHL );
    mainVL->addLayout( typeHL );
    mainVL->addLayout( dateHL );
    mainVL->addWidget( commentTL );
    mainVL->addWidget( _commentTE );
    mainVL->addLayout( buttonHL );

    setLayout( mainVL );
}