/*
    Exoplismou source code. Tak172. 2024.

    @Name:			CExtensionsWidget.cpp
    @Created:		03.10.2024
    @Programmer:	Timofey Kromachev

    Implementations. */

#include "stdafx.h"

#include "CExtensionsWidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QLabel>
#include <QTableView>
#include <QSqlQueryModel>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>
#include <QMenu>
#include <QAction>

#include "../Database/CDatabaseManager.h"
#include "Global.h"

CExtensionsWidget::CExtensionsWidget( QWidget * parent )
    : QWidget( parent )
{
    SetupWidgets();

    connect( _exitBN, &QPushButton::clicked, this, &CExtensionsWidget::OnExit );
    connect( _extView, &QTableView::customContextMenuRequested, this, &CExtensionsWidget::OnCustomContextMenu );
}

void CExtensionsWidget::OnCustomContextMenu( QPoint pos )
{
    QModelIndex index = _extView->indexAt( pos );
    if ( index.isValid() )
    {
        int extID = _extView->model()->data( _extView->model()->index( index.row(), 0 ) ).toInt();
        QMenu contextMenu( this );
        QAction * acceptAN = new QAction( QString::fromStdWString( L"Принять" ), this );
        QAction * rejectAN = new QAction( QString::fromStdWString( L"Отклонить" ), this );

        connect( acceptAN, &QAction::triggered, this, [ this, extID ] ()
        {
            OnAccept( extID );
        } );
        connect( rejectAN, &QAction::triggered, this, [ this, extID ] ()
        {
            OnReject( extID );
        } );

        contextMenu.addAction( acceptAN );
        contextMenu.addAction( rejectAN );

        contextMenu.exec( _extView->viewport()->mapToGlobal( pos ) );
    }
}

void CExtensionsWidget::OnAccept( int extID )
{
    DatabaseProcessing::CDatabaseManager * DBInstance = DatabaseProcessing::CDatabaseManager::Instance();
    DBInstance->RemoveExtension( true, extID );
    LoadData();
}

void CExtensionsWidget::OnReject( int extID )
{
    DatabaseProcessing::CDatabaseManager * DBInstance = DatabaseProcessing::CDatabaseManager::Instance();
    DBInstance->RemoveExtension( false, extID );
    LoadData();
}

void CExtensionsWidget::OnExit()
{
    emit Exit();
}

void CExtensionsWidget::SetupWidgets()
{
    QVBoxLayout * mainVL = new QVBoxLayout( this );

    _queryModel = new QSqlQueryModel( this );
    _extView = new QTableView( this );

    _extView->setSelectionBehavior( QAbstractItemView::SelectRows );
    _extView->verticalHeader()->setVisible( false );

    _exitBN = new QPushButton( QString::fromStdWString( L"Выход" ), this );
    _exitBN->setMaximumWidth( 100 );

    mainVL->addWidget( _exitBN, 0, Qt::AlignRight );
    mainVL->addWidget( _extView );

    setLayout( mainVL );

    _extView->setModel( _queryModel );
    _extView->setSortingEnabled( true );
    _extView->setContextMenuPolicy( Qt::ContextMenuPolicy::CustomContextMenu );
    _extView->horizontalHeader()->setStretchLastSection( true );
}

void CExtensionsWidget::LoadData()
{
    DatabaseProcessing::CDatabaseManager * DBInstance = DatabaseProcessing::CDatabaseManager::Instance();
    QSqlQuery query = DBInstance->GetUserExtensions( _currentUser->_id );
    if ( query.exec() == false )
    {
        QString message = QString::fromStdWString( L"Не удалось получить запросы на продление." );
        QMessageBox messageBox;
        messageBox.setWindowTitle( QString::fromStdWString( L"Ошибка запроса" ) );
        messageBox.setText( message );
        messageBox.setIcon( QMessageBox::Warning );
        messageBox.setStandardButtons( QMessageBox::Ok );
        if ( QAbstractButton * okButton = messageBox.button( QMessageBox::Ok ) )

        messageBox.exec();
        OnExit();
    }

    _queryModel->setQuery( query );
    _extView->setColumnHidden( 0, true );
}
