/*
    Exoplismou source code. Tak172. 2024.

    @Name:			CMenuTemplateWidget.cpp
    @Created:		01.10.2024
    @Programmer:	Timofey Kromachev

    Implementations. */

#include "stdafx.h"

#include "CMenuTemplateWidget.h"

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QStackedWidget>

#include "Global.h"
#include "Database/CDatabaseManager.h"
#include "CCustomerMenuWidget.h"
#include "CManagerMenuWidget.h"
#include "CMasterMenuWidget.h"
#include "COperatorMenuWidget.h"

CMenuTemplateWidget::CMenuTemplateWidget( QWidget * parent )
    : QWidget( parent )
{
    SetupWidgets();

    connect( _exitPB, &QPushButton::clicked, this, &CMenuTemplateWidget::OnExit );
    connect( _loginHistoryPB, &QPushButton::clicked, this, &CMenuTemplateWidget::ShowLoginHistory );
    connect( _changeProfileDataBN, &QPushButton::clicked, this, &CMenuTemplateWidget::OnChangeProfile );

    connect( _managerMenuWidget, &CManagerMenuWidget::ShowRequests, this, &CMenuTemplateWidget::OnShowRequests );
    connect( _customerMenuWidget, &CCustomerMenuWidget::ShowRequests, this, &CMenuTemplateWidget::OnShowRequests );
    connect( _masterMenuWidget, &CMasterMenuWidget::ShowRequests, this, &CMenuTemplateWidget::OnShowRequests );
    connect( _operatorMenuWidget, &COperatorMenuWidget::ShowRequests, this, &CMenuTemplateWidget::OnShowRequests );

    connect( _managerMenuWidget, &CManagerMenuWidget::ShowMasters, this, &CMenuTemplateWidget::OnShowMasters );
    connect( _operatorMenuWidget, &COperatorMenuWidget::ShowMasters, this, &CMenuTemplateWidget::OnShowMasters );

    connect( _customerMenuWidget, &CCustomerMenuWidget::ShowExt, this, &CMenuTemplateWidget::OnShowExt );
    connect( _managerMenuWidget, &CManagerMenuWidget::AddMaster, this, &CMenuTemplateWidget::OnAddMaster );
}

void CMenuTemplateWidget::SetupWidgets()
{
    QGridLayout * mainGL = new QGridLayout;

    {
        QGridLayout * topGL = new QGridLayout;
        QGroupBox * profileGB = new QGroupBox( this );
        QGridLayout * profileGL = new QGridLayout;

        _photoTL = new QLabel( profileGB );
        _photoTL->setFixedSize( 70, 70 );
        _photoTL->setStyleSheet( "border: 1px solid gray;" );

        _fullNameTL = new QLabel( profileGB );
        _phoneNumberTL = new QLabel( profileGB );
        _userRoleTL = new QLabel( profileGB );

        _changeProfileDataBN = new QPushButton( profileGB );
        _changeProfileDataBN->setText( QString::fromStdWString( L"Изменить" ) );

        profileGL->addWidget( _photoTL, 0, 0, 3, 1 );
        profileGL->addWidget( _fullNameTL, 0, 1, 1, 1 );
        profileGL->addWidget( _phoneNumberTL, 1, 1, 1, 1 );
        profileGL->addWidget( _userRoleTL, 2, 1, 1, 1 );
        profileGL->addWidget( _changeProfileDataBN, 3, 0, 1, 2 );

        profileGB->setLayout( profileGL );

        QSpacerItem * topHS = new QSpacerItem( 458, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );

        _exitPB = new QPushButton( this );
        _exitPB->setText( QString::fromStdWString( L"Выйти" ) );

        QSpacerItem * topVS = new QSpacerItem( 20, 58, QSizePolicy::Minimum, QSizePolicy::Maximum );

        topGL->addWidget( profileGB, 0, 0, 2, 1 );
        topGL->addItem( topHS, 0, 1, 1, 1 );
        topGL->addWidget( _exitPB, 0, 2, 1, 1 );
        topGL->addItem( topVS, 1, 2, 1, 1 );

        mainGL->addLayout( topGL, 0, 0, 1, 1 );
    }

    {
        QGridLayout * centerGL = new QGridLayout;
        QSpacerItem * centerTopVS = new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding );
        QSpacerItem * centerLeftHS = new QSpacerItem( 118, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
        _stackedWidget = new QStackedWidget( this );
        QSpacerItem * centerRightHS = new QSpacerItem( 128, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
        QSpacerItem * centerBottomVS = new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding );

        centerGL->addItem( centerTopVS, 0, 1, 1, 1 );
        centerGL->addItem( centerLeftHS, 1, 0, 1, 1 );
        centerGL->addWidget( _stackedWidget, 1, 1, 1, 1 );
        centerGL->addItem( centerRightHS, 1, 2, 1, 1 );
        centerGL->addItem( centerBottomVS, 2, 1, 1, 1 );

        mainGL->addLayout( centerGL, 1, 0, 1, 1 );
    }

    {
        QHBoxLayout * bottomHL = new QHBoxLayout;

        _loginHistoryPB = new QPushButton( this );
        _loginHistoryPB->setText( QString::fromStdWString( L"История входов" ) );

        QSpacerItem * bottomHS = new QSpacerItem( 368, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );

        bottomHL->addWidget( _loginHistoryPB );
        bottomHL->addItem( bottomHS );

        mainGL->addLayout( bottomHL, 2, 0, 1, 1 );
    }

    setLayout( mainGL );

    _masterMenuWidget = new CMasterMenuWidget( this );
    _managerMenuWidget = new CManagerMenuWidget( this );
    _customerMenuWidget = new CCustomerMenuWidget( this );
    _operatorMenuWidget = new COperatorMenuWidget( this );

    _stackedWidget->addWidget( _operatorMenuWidget );
    _stackedWidget->addWidget( _managerMenuWidget );
    _stackedWidget->addWidget( _masterMenuWidget );
    _stackedWidget->addWidget( _customerMenuWidget );
}

void CMenuTemplateWidget::SetupUserData()
{
    _fullNameTL->setText( _currentUser->_fullName );
    _phoneNumberTL->setText( _currentUser->_phone );
    _userRoleTL->setText( QString::fromStdWString( DatabaseProcessing::StringFromUserStatus( _currentUser->_status ) ) );

    {
        QImage image;
        if ( !image.loadFromData( _currentUser->_photo, "PNG" ) )
        {
            image.load( ":/Icons/no_photo" );
        }

        _photoTL->setPixmap( QPixmap::fromImage( image ).scaled( _photoTL->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
    }

    _loginHistoryPB->setVisible( _currentUser->_status == DatabaseProcessing::EUserStatus::OPERATOR );

    switch ( _currentUser->_status )
    {
        case DatabaseProcessing::EUserStatus::OPERATOR:
        {
            _stackedWidget->setCurrentWidget( _operatorMenuWidget );
        }
        break;
        case DatabaseProcessing::EUserStatus::MANAGER:
        {
            _stackedWidget->setCurrentWidget( _managerMenuWidget );
        }
        break;
        case DatabaseProcessing::EUserStatus::MASTER:
        {
            _stackedWidget->setCurrentWidget( _masterMenuWidget );
        }
        break;
        case DatabaseProcessing::EUserStatus::CUSTOMER:
        {
            _stackedWidget->setCurrentWidget( _customerMenuWidget );
        }
    }
}

void CMenuTemplateWidget::ShowLoginHistory()
{
    emit LoginHistory();
}

void CMenuTemplateWidget::OnChangeProfile()
{
    emit ChangeProfile();
}

void CMenuTemplateWidget::OnShowRequests( ERequestsViewType type )
{
    emit ShowRequests( type );
}

void CMenuTemplateWidget::OnShowExt()
{
    emit ShowExt();
}

void CMenuTemplateWidget::OnAddMaster()
{
    emit AddMaster();
}

void CMenuTemplateWidget::OnShowMasters( EMasterWidgetType type )
{
    emit ShowMasters( type );
}

void CMenuTemplateWidget::OnExit()
{
    emit Exit();
}