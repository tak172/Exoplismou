/*
    Exoplismou source code. Tak172. 2024.

    @Name:			CManagerMenuWidget.cpp
    @Created:		01.10.2024
    @Programmer:	Timofey Kromachev

    Implementations. */

#include "stdafx.h"

#include "CManagerMenuWidget.h"

#include <QPushButton>
#include <QVBoxLayout>

CManagerMenuWidget::CManagerMenuWidget( QWidget * parent )
    : QWidget( parent )
{
    SetupWidgets();

    connect( _showActiveOrdersBN, &QPushButton::clicked, this, &CManagerMenuWidget::OnShowActiveOrders );
    connect( _showMastersBN, &QPushButton::clicked, this, &CManagerMenuWidget::OnShowMasters );
    connect( _addMasterBN, &QPushButton::clicked, this, &CManagerMenuWidget::OnAddMaster );
}

void CManagerMenuWidget::OnShowMasters()
{
    emit ShowMasters( EMasterWidgetType::ManagerEdit );
}

void CManagerMenuWidget::OnAddMaster()
{
    emit AddMaster();
}

void CManagerMenuWidget::SetupWidgets()
{
    QVBoxLayout * mainVL = new QVBoxLayout( this );

    _addMasterBN = new QPushButton( QString::fromStdWString( L"Добавить сотрудника" ), this );
    _showActiveOrdersBN = new QPushButton( QString::fromStdWString( L"Показать активные заказы" ), this );
    _showMastersBN = new QPushButton( QString::fromStdWString( L"Показать мастеров" ), this );

    mainVL->addWidget( _addMasterBN );
    mainVL->addWidget( _showActiveOrdersBN );
    mainVL->addWidget( _showMastersBN );

    setLayout( mainVL );
}

void CManagerMenuWidget::OnShowActiveOrders()
{
    emit ShowRequests( ERequestsViewType::ManagerActiveRequests );
}