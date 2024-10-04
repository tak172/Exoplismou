/*
    Exoplismou source code. Tak172. 2024.

    @Name:			CAuthHistoryWidget.h
    @Created:		30.09.2024
    @Programmer:	Timofey Kromachev

    Implementations. */

#include "stdafx.h"

#include "CAuthHistoryWidget.h"

#include <QLineEdit>
#include <QLabel>
#include <QDateEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSortFilterProxyModel>
#include <QSqlQueryModel>
#include <QTableView>
#include <QMessageBox>
#include <QHeaderView>

#include "../Database/CDatabaseManager.h"

CAuthHistoryWidget::CAuthHistoryWidget( QWidget * parent )
    : QWidget( parent ), _totalRecords( 0 )
{
    SetupWidgets();

    connect( _filterBN, &QPushButton::clicked, this, &CAuthHistoryWidget::FilterData );
    connect( _clearFilterBN, &QPushButton::clicked, this, &CAuthHistoryWidget::ResetFilters );
    connect( _exitBN, &QPushButton::clicked, this, &CAuthHistoryWidget::OnExit );
}

void CAuthHistoryWidget::ResetFilters()
{
    _usernameFilterLE->clear();
    _dateFilterDE->setDate( QDate::currentDate() );

    _filterModel->setFilterFixedString( QString() );

    for ( int row = 0; row < _historyView->model()->rowCount(); ++row )
        _historyView->setRowHidden( row, false );

    int filteredRecords = _queryModel->rowCount();
    _recordCountTL->setText( QString::fromStdWString( L"%1 из %2" ).arg( filteredRecords ).arg( _totalRecords ) );
}

void CAuthHistoryWidget::OnExit()
{
    emit Exit();
}

void CAuthHistoryWidget::SetupWidgets()
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

        _dateFilterDE = new QDateEdit( this );
        _dateFilterDE->setMinimumWidth( 100 );
        _dateFilterDE->setCalendarPopup( true );
        _dateFilterDE->setDate( QDate::currentDate() );
        _dateFilterDE->setDisplayFormat( "yyyy-MM-dd" );

        filterHL->addWidget( new QLabel( QString::fromStdWString( L"ФИО пользоватея: " ), this ) );
        filterHL->addWidget( _usernameFilterLE );
        filterHL->addWidget( new QLabel( QString::fromStdWString( L"Дата входа: " ), this ) );
        filterHL->addWidget( _dateFilterDE );

        mainVL->addLayout( filterHL );
    }

    {
        QHBoxLayout * countHL = new QHBoxLayout;

        _filterBN = new QPushButton( QString::fromStdWString( L"Отфильтровать" ), this );
        _filterBN->setFixedWidth( 120 );

        _clearFilterBN = new QPushButton( QString::fromStdWString( L"Сбросить фильтры" ), this );
        _clearFilterBN->setFixedWidth( 120 );

        QSpacerItem * leftHS = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
        _recordCountTL = new QLabel( this );
        _recordCountTL->setText( QString::fromStdWString( L"0 из 0" ) );

        countHL->addWidget( _filterBN );
        countHL->addWidget( _clearFilterBN );
        countHL->addItem( leftHS );
        countHL->addWidget( _recordCountTL );

        mainVL->addLayout( countHL );
    }

    _historyView = new QTableView( this );
    _historyView->setSelectionBehavior( QAbstractItemView::SelectRows );
    _historyView->verticalHeader()->setVisible( false );

    mainVL->addWidget( _historyView );

    setLayout( mainVL );

    _queryModel = new QSqlQueryModel( this );
    _filterModel = new QSortFilterProxyModel( this );

    LoadLoginHistory();

    _filterModel->setSourceModel( _queryModel );
    _historyView->setModel( _filterModel );
    _historyView->setSortingEnabled( true );

    _historyView->setColumnWidth( 0, 200 );
    _historyView->setColumnWidth( 1, 200 );
    _historyView->horizontalHeader()->setStretchLastSection( true );
}

void CAuthHistoryWidget::LoadLoginHistory()
{
    DatabaseProcessing::CDatabaseManager * DBInstance = DatabaseProcessing::CDatabaseManager::Instance();
    QSqlQuery query = DBInstance->GetAuthHistory();
    if ( query.exec() == false )
    {
        QString message = QString::fromStdWString( L"Не удалось получить историю входов." );
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
}

void CAuthHistoryWidget::FilterData() 
{
    QString username = _usernameFilterLE->text().trimmed();
    QString date = _dateFilterDE->date().toString( "yyyy-MM-dd" );

    for ( int row = 0; row < _historyView->model()->rowCount(); ++row )
        _historyView->setRowHidden( row, false );

    if ( !username.isEmpty() )
    {
        for ( int row = 0; row < _historyView->model()->rowCount(); ++row )
        {
            QModelIndex index = _historyView->model()->index( row, 0 );
            QString rowUsername = _historyView->model()->data( index ).toString();

            if ( !rowUsername.contains( username, Qt::CaseInsensitive ) )
            {
                _historyView->setRowHidden( row, true );
            }
        }
    }

    if ( !date.isEmpty() )
    {
        for ( int row = 0; row < _historyView->model()->rowCount(); ++row )
        {
            if ( _historyView->isRowHidden( row ) )
                continue;

            QModelIndex index = _historyView->model()->index( row, 1 );
            QString rowDate = _historyView->model()->data( index ).toString();

            if ( !rowDate.contains( date, Qt::CaseInsensitive ) )
            {
                _historyView->setRowHidden( row, true );
            }
        }
    }

    int visibleRecords = 0;
    for ( int row = 0; row < _historyView->model()->rowCount(); ++row )
    {
        if ( !_historyView->isRowHidden( row ) )
        {
            ++visibleRecords;
        }
    }

    _recordCountTL->setText( QString::fromStdWString( L"%1 из %2" ).arg( visibleRecords ).arg( _totalRecords ) );
}