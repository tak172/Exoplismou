/*
    Exoplismou source code. Tak172. 2024.

    @Name:			CEditRequestWindow.cpp
    @Created:		03.10.2024
    @Programmer:	Timofey Kromachev

    Implementations. */

#include "stdafx.h"

#include "CEditRequestWindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QDateEdit>
#include <QMessageBox>

#include "../Global.h"
#include "../Database/CDatabaseManager.h"

CEditRequestWindow::CEditRequestWindow( int requestID, ERequestWindowType type, QWidget * parent )
    : QDialog( parent ), _type( type ), _requestID( requestID )
{
    SetupWidgets();

    connect( _okBN, &QPushButton::clicked, this, &CEditRequestWindow::OnOk );
    connect( _cancelBN, &QPushButton::clicked, this, &CEditRequestWindow::reject );
    connect( _typeCB, QOverload<int>::of( &QComboBox::currentIndexChanged ), [ this ] ( int index )
    {
        DatabaseProcessing::CDatabaseManager * DBInstance = DatabaseProcessing::CDatabaseManager::Instance();
        _modelCB->clear();
        _modelCB->addItems( DBInstance->GetDeviceModelsByTypeName( _typeCB->currentText() ) );
    } );
}

void CEditRequestWindow::OnOk()
{
    DatabaseProcessing::CDatabaseManager * DBInstance = DatabaseProcessing::CDatabaseManager::Instance();
    
    if ( _commentTE->toPlainText().isEmpty() || _typeCB->currentIndex() == -1 
        || _modelCB->currentIndex() == -1 )
    {
        QString message = QString::fromStdWString( L"Не все данные введены." );
        QMessageBox messageBox;
        messageBox.setWindowTitle( QString::fromStdWString( L"Ошибка создания" ) );
        messageBox.setText( message );
        messageBox.setIcon( QMessageBox::Warning );
        messageBox.setStandardButtons( QMessageBox::Ok );
        if ( QAbstractButton * okButton = messageBox.button( QMessageBox::Ok ) )
            okButton->setMinimumWidth( 100 );

        messageBox.exec();
        return;
    }

    switch ( _type )
    {
        case Addition:
        {
            DBInstance->AddRequest( DBInstance->GetDeviceModelIDByName( _modelCB->currentText() ), _commentTE->toPlainText(),
                _currentUser->_id, _finishDateDE->date() );
        }
        break;
        case UserEditing:
        {
            DBInstance->UpdateRequest( _requestID, DBInstance->GetDeviceModelIDByName( _modelCB->currentText() ),
                DBInstance->GetRequestData( _requestID )->_masterID, _commentTE->toPlainText(), _finishDateDE->date() );
        }
        break;
        case OperatorEditing:
        {
            DBInstance->UpdateRequest( _requestID, DBInstance->GetDeviceModelIDByName( _modelCB->currentText() ),
                DBInstance->GetMasterIDByName( _masterNameCB->currentText() ), _commentTE->toPlainText(), _finishDateDE->date() );
        }
        break;
    }

    accept();
}

void CEditRequestWindow::SetupWidgets()
{
    DatabaseProcessing::CDatabaseManager * DBInstance = DatabaseProcessing::CDatabaseManager::Instance();
    std::shared_ptr<DatabaseProcessing::SRequestData> requestData( DBInstance->GetRequestData( _requestID ) );

    QVBoxLayout * mainVL = new QVBoxLayout;

    QLabel * startDateTL = new QLabel( QString::fromStdWString( L"Дата начала:" ) );
    _startDateDE = new QDateEdit( this );
    if ( _type == ERequestWindowType::Addition )
        _startDateDE->setDate( QDate::currentDate() );
    else
        _startDateDE->setDate( requestData->_startDate );

    _startDateDE->setEnabled( false );
    mainVL->addWidget( startDateTL );
    mainVL->addWidget( _startDateDE );

    QLabel * finishDateTL = new QLabel( QString::fromStdWString( L"Дата завершения:" ) );
    _finishDateDE = new QDateEdit( this );
    if ( _type != ERequestWindowType::Addition )
        _finishDateDE->setDate( requestData->_finishDate );

    _finishDateDE->setEnabled( _type == ERequestWindowType::Addition || _type == ERequestWindowType::UserEditing );
    mainVL->addWidget( finishDateTL );
    mainVL->addWidget( _finishDateDE );

    if ( _type == ERequestWindowType::OperatorEditing )
    {
        std::shared_ptr<DatabaseProcessing::SUserData> userData( DBInstance->GetUserData( requestData->_clientID ) );
        QLabel * userLoginTL = new QLabel( QString::fromStdWString( L"Логин клиента:" ) );
        _userLoginLE = new QLineEdit( this );
        _userLoginLE->setText( userData->_login );
        _userLoginLE->setEnabled( false );
        mainVL->addWidget( userLoginTL );
        mainVL->addWidget( _userLoginLE );
    }

    if ( _type == ERequestWindowType::OperatorEditing )
    {
        std::shared_ptr<DatabaseProcessing::SUserData> masterData( DBInstance->GetUserData( requestData->_masterID ) );
        QLabel * masterNameLabel = new QLabel( QString::fromStdWString( L"Мастер:" ) );
        _masterNameCB = new QComboBox( this );
        _masterNameCB->addItems( DBInstance->GetMasterNames() );
        _masterNameCB->setCurrentText( masterData->_fullName );
        mainVL->addWidget( masterNameLabel );
        mainVL->addWidget( _masterNameCB );
    }

    QLabel * typeTL = new QLabel( QString::fromStdWString( L"Тип устройства:" ) );
    _typeCB = new QComboBox( this );
    _typeCB->addItems( DBInstance->GetAllDeviceTypes() );
    if ( _type != ERequestWindowType::Addition )
    {
        std::shared_ptr<DatabaseProcessing::SDeviceModel> modelData( DBInstance->GetDeviceModelData( requestData->_modelID ) );
        std::shared_ptr<DatabaseProcessing::SDeviceType> typeData( DBInstance->GetDeviceTypeData( modelData->_typeID ) );
        _typeCB->setCurrentText( typeData->_name );
    }

    mainVL->addWidget( typeTL );
    mainVL->addWidget( _typeCB );

    QLabel * modeTL = new QLabel( QString::fromStdWString( L"Модель устройства:" ) );
    _modelCB = new QComboBox( this );
    if ( _type != ERequestWindowType::Addition )
    {
        std::shared_ptr<DatabaseProcessing::SDeviceModel> modelData( DBInstance->GetDeviceModelData( requestData->_modelID ) );
        _modelCB->addItems( DBInstance->GetAllModels( modelData->_typeID ) );
        _modelCB->setCurrentText( modelData->_name );
    }

    mainVL->addWidget( modeTL );
    mainVL->addWidget( _modelCB );

    QLabel * descTL = new QLabel( QString::fromStdWString( L"Описание проблемы:" ) );
    _commentTE = new QTextEdit( this );
    if ( _type != ERequestWindowType::Addition )
    {
        _commentTE->setText( requestData->_descr );
    }

    mainVL->addWidget( descTL );
    mainVL->addWidget( _commentTE );

    QHBoxLayout * buttonHL = new QHBoxLayout();
    _okBN = new QPushButton( QString::fromStdWString( L"ОК" ) );
    _cancelBN = new QPushButton( QString::fromStdWString( L"Отмена" ) );
    buttonHL->addWidget( _okBN );
    buttonHL->addWidget( _cancelBN );
    mainVL->addLayout( buttonHL );

    setLayout( mainVL );
}
