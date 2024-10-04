/*
    Exoplismou source code. Tak172. 2024.

    @Name:			CCustomerMenuWidget.cpp
    @Created:		01.10.2024
    @Programmer:	Timofey Kromachev

    Implementations. */

#include "stdafx.h"

#include "CCustomerMenuWidget.h"

#include <QPushButton>
#include <QVBoxLayout>

#include "../Windows/CEditRequestWindow.h"

CCustomerMenuWidget::CCustomerMenuWidget( QWidget * parent )
    : QWidget( parent )
{
    SetupWidgets();

    connect( _addRequestBN, &QPushButton::clicked, this, &CCustomerMenuWidget::OnAddRequest );
    connect( _showActiveOrdersBN, &QPushButton::clicked, this, &CCustomerMenuWidget::OnShowActiveOrders );
    connect( _showOrdersHistoryBN, &QPushButton::clicked, this, &CCustomerMenuWidget::OnShowOrdersHistory );
    connect( _showRequestsBN, &QPushButton::clicked, this, &CCustomerMenuWidget::OnShowExt );
}

void CCustomerMenuWidget::OnAddRequest()
{
    CEditRequestWindow * editRequestDlg = new CEditRequestWindow( -1, Addition, this );
    editRequestDlg->exec();
}

void CCustomerMenuWidget::OnShowActiveOrders()
{
    emit ShowRequests( ERequestsViewType::CustomerActiveRequests );
}

void CCustomerMenuWidget::OnShowOrdersHistory()
{
    emit ShowRequests( ERequestsViewType::CustomerHistory );
}

void CCustomerMenuWidget::OnShowExt()
{
    emit ShowExt();
}

void CCustomerMenuWidget::SetupWidgets()
{
    QVBoxLayout * mainVL = new QVBoxLayout( this );

    _addRequestBN = new QPushButton( QString::fromStdWString( L"Создать заказ" ), this );
    _showActiveOrdersBN = new QPushButton( QString::fromStdWString( L"Показать не принятые заказы" ), this );
    _showOrdersHistoryBN = new QPushButton( QString::fromStdWString( L"Показать историю заказов" ), this );
    _showRequestsBN = new QPushButton( QString::fromStdWString( L"Показать запросы" ) );

    mainVL->addWidget( _addRequestBN );
    mainVL->addWidget( _showActiveOrdersBN );
    mainVL->addWidget( _showOrdersHistoryBN );
    mainVL->addWidget( _showRequestsBN );

    setLayout( mainVL );
}