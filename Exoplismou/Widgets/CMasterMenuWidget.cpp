/*
    Exoplismou source code. Tak172. 2024.

    @Name:			CMasterMenuWidget.cpp
    @Created:		01.10.2024
    @Programmer:	Timofey Kromachev

    Implementations. */

#include "stdafx.h"

#include "CMasterMenuWidget.h"

#include <QPushButton>
#include <QVBoxLayout>

CMasterMenuWidget::CMasterMenuWidget( QWidget * parent )
    : QWidget( parent )
{
    SetupWidgets();

    connect( _showActiveOrdersBN, &QPushButton::clicked, this, &CMasterMenuWidget::OnShowActiveOrders );
    connect( _showOrdersHistoryBN, &QPushButton::clicked, this, &CMasterMenuWidget::OnShowOrdersHistory );
}

void CMasterMenuWidget::OnShowActiveOrders()
{
    emit ShowRequests( ERequestsViewType::MasterActiveRequests );
}

void CMasterMenuWidget::OnShowOrdersHistory()
{
    emit ShowRequests( ERequestsViewType::MasterHistory );
}

void CMasterMenuWidget::SetupWidgets()
{
    QVBoxLayout * mainVL = new QVBoxLayout( this );

    _showActiveOrdersBN = new QPushButton( QString::fromStdWString( L"Показать активные заказы" ), this );
    _showOrdersHistoryBN = new QPushButton( QString::fromStdWString( L"Показать историю заказов" ), this );

    mainVL->addWidget( _showActiveOrdersBN );
    mainVL->addWidget( _showOrdersHistoryBN );

    setLayout( mainVL );
}