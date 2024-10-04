/*
    Exoplismou source code. Tak172. 2024.

    @Name:			CMastersWidget.h
    @Created:		03.10.2024
    @Programmer:	Timofey Kromachev

    Implementations. */

#include "stdafx.h"

#include "CMastersWidget.h"

#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSortFilterProxyModel>
#include <QSqlQueryModel>
#include <QTableView>
#include <QMessageBox>
#include <QHeaderView>
#include <QAction>
#include <QMenu>

#include "../Database/CDatabaseManager.h"
#include "../Windows/CReportsWindow.h"

CMastersWidget::CMastersWidget( QWidget * parent )
    : QWidget( parent ), _totalRecords( 0 )
{
    SetupWidgets();

    connect( _filterBN, &QPushButton::clicked, this, &CMastersWidget::FilterData );
    connect( _clearFilterBN, &QPushButton::clicked, this, &CMastersWidget::ResetFilters );
    connect( _exitBN, &QPushButton::clicked, this, &CMastersWidget::OnExit );
    connect( _mastersView, &QTableView::customContextMenuRequested, this, &CMastersWidget::OnCustomContextMenu );
}

void CMastersWidget::OnExit()
{
    emit Exit();
}

void CMastersWidget::OnCustomContextMenu( QPoint pos )
{
    if ( _type == EMasterWidgetType::ManagerEdit )
        return;

    QModelIndex index = _mastersView->indexAt( pos );
    if ( index.isValid() )
    {
        int masterID = _mastersView->model()->data( _mastersView->model()->index( index.row(), 0 ) ).toInt();
        QMenu contextMenu( this );
        QAction * reportsAN = new QAction( QString::fromStdWString( L"Отчеты мастера" ), this );
        connect( reportsAN, &QAction::triggered, [ this, masterID ] ()
        {
            OnReports( masterID );
        } );

        contextMenu.addAction( reportsAN );
        contextMenu.exec( _mastersView->viewport()->mapToGlobal( pos ) );
    }
}

void CMastersWidget::OnReports( int masterID )
{
    CReportsWindow reportsDlg( masterID, this );
    reportsDlg.exec();
}

void CMastersWidget::SetupWidgets()
{
    QVBoxLayout * mainVL = new QVBoxLayout;

    {
        QHBoxLayout * exitHL = new QHBoxLayout;

        QSpacerItem * leftHS = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
        _exitBN = new QPushButton( QString::fromStdWString( L"Меню" ), this );
        _exitBN->setFixedWidth( 100 );

        exitHL->addItem( leftHS );
        exitHL->addWidget( _exitBN );

        mainVL->addLayout( exitHL );
    }

    {
        QHBoxLayout * filterHL = new QHBoxLayout;

        _usernameFilterLE = new QLineEdit( this );

        _filterBN = new QPushButton( QString::fromStdWString( L"Отфильтровать" ), this );
        _filterBN->setFixedWidth( 120 );

        _clearFilterBN = new QPushButton( QString::fromStdWString( L"Сбросить фильтры" ), this );
        _clearFilterBN->setFixedWidth( 120 );

        filterHL->addWidget( new QLabel( QString::fromStdWString( L"ФИО пользоватея: " ), this ) );
        filterHL->addWidget( _usernameFilterLE );
        filterHL->addWidget( _filterBN );
        filterHL->addWidget( _clearFilterBN );

        mainVL->addLayout( filterHL );
    }

    {
        QHBoxLayout * exitHL = new QHBoxLayout;

        QSpacerItem * leftHS = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
        _recordCountTL = new QLabel( this );
        
        exitHL->addItem( leftHS );
        exitHL->addWidget( _recordCountTL );

        mainVL->addLayout( exitHL );
    }

    _mastersView = new QTableView( this );
    _mastersView->setSelectionBehavior( QAbstractItemView::SelectRows );
    _mastersView->verticalHeader()->setVisible( false );

    mainVL->addWidget( _mastersView );

    setLayout( mainVL );

    _queryModel = new QSqlQueryModel( this );
    _filterModel = new QSortFilterProxyModel( this );
    _filterModel->setSourceModel( _queryModel );
    _mastersView->setModel( _filterModel );
    _mastersView->setSortingEnabled( true );

    _mastersView->setColumnWidth( 0, 200 );
    _mastersView->setColumnWidth( 1, 200 );
    _mastersView->setSortingEnabled( true );
    _mastersView->setContextMenuPolicy( Qt::ContextMenuPolicy::CustomContextMenu );
    _mastersView->horizontalHeader()->setStretchLastSection( true );
}

void CMastersWidget::LoadData( EMasterWidgetType type )
{
    _type = type;
    DatabaseProcessing::CDatabaseManager * DBInstance = DatabaseProcessing::CDatabaseManager::Instance();
    QSqlQuery query = DBInstance->GetMasters();
    if ( query.exec() == false )
    {
        QString message = QString::fromStdWString( L"Не удалось получить данные мастеров." );
        QMessageBox messageBox;
        messageBox.setWindowTitle( QString::fromStdWString( L"Ошибка запроса" ) );
        messageBox.setText( message );
        messageBox.setIcon( QMessageBox::Warning );
        messageBox.setStandardButtons( QMessageBox::Ok );
        if ( QAbstractButton * okButton = messageBox.button( QMessageBox::Ok ) )
            okButton->setMinimumWidth( 100 );

        messageBox.exec();
        OnExit();
    }

    _queryModel->setQuery( query );
    _totalRecords = _queryModel->rowCount();
    _filterModel->setSourceModel( _queryModel );
    _recordCountTL->setText( QString::fromStdWString( L"%1 из %2" ).arg( _totalRecords ).arg( _totalRecords ) );
    _mastersView->setColumnHidden( 0, true );
}

void CMastersWidget::ResetFilters()
{
    _usernameFilterLE->clear();
    _filterModel->setFilterFixedString( QString() );

    for ( int row = 0; row < _mastersView->model()->rowCount(); ++row )
        _mastersView->setRowHidden( row, false );

    int filteredRecords = _queryModel->rowCount();
    _recordCountTL->setText( QString::fromStdWString( L"%1 из %2" ).arg( filteredRecords ).arg( _totalRecords ) );
}

void CMastersWidget::FilterData()
{
    QString username = _usernameFilterLE->text().trimmed();

    for ( int row = 0; row < _mastersView->model()->rowCount(); ++row )
        _mastersView->setRowHidden( row, false );

    if ( !username.isEmpty() )
    {
        for ( int row = 0; row < _mastersView->model()->rowCount(); ++row )
        {
            QModelIndex index = _mastersView->model()->index( row, 1 );
            QString rowUsername = _mastersView->model()->data( index ).toString();

            if ( !rowUsername.contains( username, Qt::CaseInsensitive ) )
            {
                _mastersView->setRowHidden( row, true );
            }
        }
    }

    int visibleRecords = 0;
    for ( int row = 0; row < _mastersView->model()->rowCount(); ++row )
    {
        if ( !_mastersView->isRowHidden( row ) )
            ++visibleRecords;
    }

    _recordCountTL->setText( QString::fromStdWString( L"%1 из %2" ).arg( visibleRecords ).arg( _totalRecords ) );
}