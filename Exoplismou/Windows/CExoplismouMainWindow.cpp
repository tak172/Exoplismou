/*
    Exoplismou source code. Tak172. 2024.

    @Name:			CExoplismouMainWindow.cpp
    @Created:		29.09.2024
    @Programmer:	Timofey Kromachev

    Implementations. */

#include "stdafx.h"

#include "CExoplismouMainWindow.h"

#include <QApplication>
#include <QStackedWidget>

#include "Global.h"
#include "Database/CDatabaseManager.h"
#include "Windows/CAuthentificationWindow.h"
#include "Widgets/CMenuTemplateWidget.h"
#include "Widgets/CAuthHistoryWidget.h"
#include "Widgets/CEditUserDataWidget.h"
#include "Widgets/CRequestsViewWidget.h"
#include "Widgets/CExtensionsWidget.h"

CExoplismouMainWindow::CExoplismouMainWindow( QWidget * parent )
    : QMainWindow( parent )
{
    SetupWidgets();

    connect( _menuTemplateWidget, &CMenuTemplateWidget::Exit, this, &CExoplismouMainWindow::Login );
    connect( _authHistoryWidget, &CAuthHistoryWidget::Exit, this, &CExoplismouMainWindow::ExitToMenu );
    connect( _menuTemplateWidget, &CMenuTemplateWidget::LoginHistory, this, &CExoplismouMainWindow::LoginHistory );
    connect( _menuTemplateWidget, &CMenuTemplateWidget::ChangeProfile, this, &CExoplismouMainWindow::ChangeProfile );
    connect( _editUserDataWidget, &CEditUserDataWidget::Exit, this, &CExoplismouMainWindow::ExitToMenu );
    connect( _requestsViewWidget, &CRequestsViewWidget::Exit, this, &CExoplismouMainWindow::ExitToMenu );
    connect( _extensionsWidget, &CExtensionsWidget::Exit, this, &CExoplismouMainWindow::ExitToMenu );
    connect( _masterWidget, &CMastersWidget::Exit, this, &CExoplismouMainWindow::ExitToMenu );

    connect( _menuTemplateWidget, &CMenuTemplateWidget::ShowRequests, this, &CExoplismouMainWindow::ShowRequests );
    connect( _menuTemplateWidget, &CMenuTemplateWidget::ShowExt, this, &CExoplismouMainWindow::ShowExt );
    connect( _menuTemplateWidget, &CMenuTemplateWidget::AddMaster, this, &CExoplismouMainWindow::AddMaster );
    connect( _menuTemplateWidget, &CMenuTemplateWidget::ShowMasters, this, &CExoplismouMainWindow::ShowMasters );
}

void CExoplismouMainWindow::LoginHistory()
{
    _authHistoryWidget->LoadLoginHistory();
    _stackedWidget->setCurrentWidget( _authHistoryWidget );
}

void CExoplismouMainWindow::ExitToMenu()
{
    _menuTemplateWidget->SetupUserData();
    _stackedWidget->setCurrentWidget( _menuTemplateWidget );
}

void CExoplismouMainWindow::ChangeProfile()
{
    _editUserDataWidget->UpdateUserData( EEditType::EditUser );
    _stackedWidget->setCurrentWidget( _editUserDataWidget );
}

void CExoplismouMainWindow::ShowRequests( ERequestsViewType type )
{
    _requestsViewWidget->SetCurrentType( type );
    _stackedWidget->setCurrentWidget( _requestsViewWidget );
}

void CExoplismouMainWindow::ShowExt()
{
    _extensionsWidget->LoadData();
    _stackedWidget->setCurrentWidget( _extensionsWidget );
}

void CExoplismouMainWindow::AddMaster()
{
    _editUserDataWidget->UpdateUserData( EEditType::CreateManager );
    _stackedWidget->setCurrentWidget( _editUserDataWidget );
}

void CExoplismouMainWindow::ShowMasters( EMasterWidgetType type )
{
    _masterWidget->LoadData( type );
    _stackedWidget->setCurrentWidget( _masterWidget );
}

void CExoplismouMainWindow::SetupWidgets()
{
    setWindowTitle( "Exoplismou" );
    resize( 900, 600 );

    _stackedWidget = new QStackedWidget( this );
    setCentralWidget( _stackedWidget );

    _menuTemplateWidget = new CMenuTemplateWidget( this );
    _authHistoryWidget = new CAuthHistoryWidget( this );
    _editUserDataWidget = new CEditUserDataWidget( this );
    _requestsViewWidget = new CRequestsViewWidget( this );
    _extensionsWidget = new CExtensionsWidget( this );
    _masterWidget = new CMastersWidget( this );

    _stackedWidget->addWidget( _extensionsWidget );
    _stackedWidget->addWidget( _masterWidget );
    _stackedWidget->addWidget( _menuTemplateWidget );
    _stackedWidget->addWidget( _authHistoryWidget );
    _stackedWidget->addWidget( _editUserDataWidget );
    _stackedWidget->addWidget( _requestsViewWidget );
}

void CExoplismouMainWindow::LoginSuccessful()
{
    ExitToMenu();
    show();
}

void CExoplismouMainWindow::Login()
{
    _currentUser.reset();
    hide();

    CAuthentificationWindow * authentification = new CAuthentificationWindow( this );
    if ( authentification->exec() == QDialog::Rejected )
    {
        QApplication::quit();
        exit( 1 );
    }
    else
        LoginSuccessful();
}
