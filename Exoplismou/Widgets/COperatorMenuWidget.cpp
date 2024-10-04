/*
    Exoplismou source code. Tak172. 2024.

    @Name:			COperatorMenuWidget.cpp
    @Created:		01.10.2024
    @Programmer:	Timofey Kromachev

    Implementations. */

#include "stdafx.h"

#include "COperatorMenuWidget.h"

#include <QPushButton>
#include <QVBoxLayout>

#include "../Windows/CCommentsWindow.h"

COperatorMenuWidget::COperatorMenuWidget( QWidget * parent )
    : QWidget( parent )
{
    SetupWidgets();

    connect( _showOrdersBN, &QPushButton::clicked, this, &COperatorMenuWidget::OnShowOrders );
    connect( _showMastersBN, &QPushButton::clicked, this, &COperatorMenuWidget::OnShowMasters );
    connect( _showReportsBN, &QPushButton::clicked, this, &COperatorMenuWidget::OnShowReports );
}

void COperatorMenuWidget::OnShowOrders()
{
    ShowRequests( ERequestsViewType::OperatorActiveRequests );
}

void COperatorMenuWidget::OnShowMasters()
{
    emit ShowMasters( EMasterWidgetType::OperatorEdit );
}

void COperatorMenuWidget::OnShowReports()
{
    CCommentsWindow commentsDlg( this );
    commentsDlg.exec();
}

void COperatorMenuWidget::SetupWidgets()
{
    QVBoxLayout * mainVL = new QVBoxLayout( this );

    _showOrdersBN = new QPushButton( QString::fromStdWString( L"Показать заказы" ), this );
    _showMastersBN = new QPushButton( QString::fromStdWString( L"Показать мастеров" ), this );
    _showReportsBN = new QPushButton( QString::fromStdWString( L"Показать отчеты" ), this );

    mainVL->addWidget( _showOrdersBN );
    mainVL->addWidget( _showMastersBN );
    mainVL->addWidget( _showReportsBN );

    setLayout( mainVL );
}