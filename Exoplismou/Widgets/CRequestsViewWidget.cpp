/*
    Exoplismou source code. Tak172. 2024.

    @Name:			CRequestsViewWidget.cpp
    @Created:		01.10.2024
    @Programmer:	Timofey Kromachev

    Implementations. */

#include "stdafx.h"

#include "CRequestsViewWidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QLabel>
#include <QTableView>
#include <QSqlQueryModel>
#include <QSortFilterProxyModel>
#include <QPushButton>
#include <QLineEdit>
#include <QDateEdit>
#include <QHeaderView>
#include <QMessageBox>
#include <QCheckBox>
#include <QMenu>
#include <QAction>

#include "../Helpful/QrCodeGenerator.h"
#include "../Database/CDatabaseManager.h"
#include "../Windows/CAddCommentWindow.h"
#include "../Windows/CEditRequestWindow.h"
#include "Global.h"

CRequestsViewWidget::CRequestsViewWidget( QWidget * parent )
    : QWidget( parent ), _totalRecords( 0 )
{
    SetupWidgets();

    connect( _exitBN, &QPushButton::clicked, this, &CRequestsViewWidget::OnExit );
    connect( _filterBN, &QPushButton::clicked, this, &CRequestsViewWidget::FilterData );
    connect( _clearFilterBN, &QPushButton::clicked, this, &CRequestsViewWidget::ResetFilters );
    connect( _useDateFilterCB, &QCheckBox::clicked, this, &CRequestsViewWidget::UseDataFilter );
    connect( _requestsView, &QTableView::customContextMenuRequested, this, &CRequestsViewWidget::OnCustomContextMenu );
}

void CRequestsViewWidget::ResetFilters()
{
    _masterFilterLE->clear();
    _customerFilterLE->clear();
    _modelFilterLE->clear();
    _startDE->setDate( QDate::currentDate() );
    _finishDE->setDate( QDate::currentDate() );
    _statusFilterLE->clear();
    _useDateFilterCB->setChecked( false );
    UseDataFilter( false );

    _filterModel->setFilterFixedString( QString() );

    for ( int row = 0; row < _requestsView->model()->rowCount(); ++row )
        _requestsView->setRowHidden( row, false );

    int filteredRecords = _queryModel->rowCount();
    _recordCountTL->setText( QString::fromStdWString( L"%1 из %2" ).arg( filteredRecords ).arg( _totalRecords ) );
}

void CRequestsViewWidget::FilterData()
{
    QString customer = _customerFilterLE->text().trimmed();
    QString master = _masterFilterLE->text().trimmed();
    QString model = _modelFilterLE->text().trimmed();
    QString status = _statusFilterLE->text().trimmed();

    QString startDate;
    if ( _startDE->isEnabled() )
        startDate = _startDE->date().toString( "yyyy-MM-dd" );

    QString finishDate;
    if ( _finishDE->isEnabled() )
        finishDate = _finishDE->date().toString( "yyyy-MM-dd" );

    for ( int row = 0; row < _requestsView->model()->rowCount(); ++row )
        _requestsView->setRowHidden( row, false );

    if ( !customer.isEmpty() )
    {
        for ( int row = 0; row < _requestsView->model()->rowCount(); ++row )
        {
            QModelIndex index = _requestsView->model()->index( row, 6 );
            QString rowCustomer = _requestsView->model()->data( index ).toString();

            if ( !rowCustomer.contains( customer, Qt::CaseInsensitive ) )
                _requestsView->setRowHidden( row, true );
        }
    }

    if ( !master.isEmpty() )
    {
        for ( int row = 0; row < _requestsView->model()->rowCount(); ++row )
        {
            QModelIndex index = _requestsView->model()->index( row, 7 );
            QString rowMaster = _requestsView->model()->data( index ).toString();

            if ( !rowMaster.contains( master, Qt::CaseInsensitive ) )
                _requestsView->setRowHidden( row, true );
        }
    }

    if ( !model.isEmpty() )
    {
        for ( int row = 0; row < _requestsView->model()->rowCount(); ++row )
        {
            QModelIndex index = _requestsView->model()->index( row, 4 );
            QString rowModel = _requestsView->model()->data( index ).toString();

            if ( !rowModel.contains( model, Qt::CaseInsensitive ) )
                _requestsView->setRowHidden( row, true );
        }
    }

    if ( !status.isEmpty() )
    {
        for ( int row = 0; row < _requestsView->model()->rowCount(); ++row )
        {
            QModelIndex index = _requestsView->model()->index( row, 5 );
            QString rowStatus = _requestsView->model()->data( index ).toString();

            if ( !rowStatus.contains( status, Qt::CaseInsensitive ) )
                _requestsView->setRowHidden( row, true );
        }
    }

    if ( !startDate.isEmpty() )
    {
        for ( int row = 0; row < _requestsView->model()->rowCount(); ++row )
        {
            if ( _requestsView->isRowHidden( row ) )
                continue;

            QModelIndex index = _requestsView->model()->index( row, 1 );
            QString rowDate = _requestsView->model()->data( index ).toString();

            if ( !rowDate.contains( startDate, Qt::CaseInsensitive ) )
                _requestsView->setRowHidden( row, true );
        }
    }

    if ( !finishDate.isEmpty() )
    {
        for ( int row = 0; row < _requestsView->model()->rowCount(); ++row )
        {
            if ( _requestsView->isRowHidden( row ) )
                continue;

            QModelIndex index = _requestsView->model()->index( row, 1 );
            QString rowDate = _requestsView->model()->data( index ).toString();

            if ( !rowDate.contains( finishDate, Qt::CaseInsensitive ) )
                _requestsView->setRowHidden( row, true );
        }
    }

    int visibleRecords = 0;
    for ( int row = 0; row < _requestsView->model()->rowCount(); ++row )
    {
        if ( !_requestsView->isRowHidden( row ) )
            ++visibleRecords;
    }

    _recordCountTL->setText( QString::fromStdWString( L"%1 из %2" ).arg( visibleRecords ).arg( _totalRecords ) );
}

void CRequestsViewWidget::OnExit()
{
    emit Exit();
}

void CRequestsViewWidget::UseDataFilter( bool state )
{
    _startDE->setEnabled( state );
    _finishDE->setEnabled( state );
}

void CRequestsViewWidget::OnCustomContextMenu( QPoint pos )
{
    QModelIndex index = _requestsView->indexAt( pos );
    if ( index.isValid() )
    {
        int requestID = _requestsView->model()->data( _requestsView->model()->index( index.row(), 0 ) ).toInt();
        QMenu contextMenu( this );

        switch ( _curType )
        {
            case ManagerActiveRequests:
            {
                QAction * extendAction = new QAction( QString::fromStdWString( L"Продлить" ), this );
                connect( extendAction, &QAction::triggered, [ this, requestID ] ()
                {
                    OnExtend( requestID );
                } );

                contextMenu.addAction( extendAction );
            }
            break;
            case MasterActiveRequests:
            {
                QAction * completeAction = new QAction( QString::fromStdWString( L"Завершить" ), this );
                connect( completeAction, &QAction::triggered, [ this, requestID ] ()
                {
                    OnComplite( requestID );
                } );

                contextMenu.addAction( completeAction );
            }
            break;
            case CustomerHistory:
            {
                QModelIndex sixthColumnIndex = index.siblingAtColumn( 5 );
                QString value = _requestsView->model()->data( sixthColumnIndex ).toString();
                if ( _requestsView->model()->data( sixthColumnIndex ).toString() == QString::fromStdWString( L"Завершена" ) )
                {
                    QAction * commentAction = new QAction( QString::fromStdWString( L"Оставить отзыв" ), this );
                    connect( commentAction, &QAction::triggered, [ this, requestID ] ()
                    {
                        OnQR( requestID );
                    } );

                    contextMenu.addAction( commentAction );
                }
            }
            break;
            case CustomerActiveRequests:
            {
                QAction * editAction = new QAction( QString::fromStdWString( L"Редактировать" ), this );
                connect( editAction, &QAction::triggered, [ this, requestID ] ()
                {
                    OnEdit( requestID, UserEditing );
                } );

                contextMenu.addAction( editAction );
            }
            break;
            case OperatorActiveRequests:
            {
                QAction * acceptAction = new QAction( QString::fromStdWString( L"Принять" ), this );
                connect( acceptAction, &QAction::triggered, [ this, requestID ] ()
                {
                    OnAccept( requestID );
                } );

                contextMenu.addAction( acceptAction );

                QAction * editAction = new QAction( QString::fromStdWString( L"Редактировать" ), this );
                connect( editAction, &QAction::triggered, [ this, requestID ] ()
                {
                    OnEdit( requestID, OperatorEditing );
                } );

                contextMenu.addAction( editAction );

                QAction * removeAction = new QAction( QString::fromStdWString( L"Удалить" ), this );
                connect( removeAction, &QAction::triggered, [ this, requestID ] ()
                {
                    OnRemove( requestID );
                } );

                contextMenu.addAction( removeAction );
            }
            break;
        }

        contextMenu.exec( _requestsView->viewport()->mapToGlobal( pos ) );
    }
}

void CRequestsViewWidget::OnExtend( int requestID )
{
    DatabaseProcessing::CDatabaseManager * DBInstance = DatabaseProcessing::CDatabaseManager::Instance();
    if ( DBInstance->AddExtension( requestID ) == false )
    {
        QString message = QString::fromStdWString( L"Запрос на продление этого заказа уже существует." );
        QMessageBox messageBox;
        messageBox.setWindowTitle( QString::fromStdWString( L"Ошибка создания запроса" ) );
        messageBox.setText( message );
        messageBox.setIcon( QMessageBox::Warning );
        messageBox.setStandardButtons( QMessageBox::Ok );
        if ( QAbstractButton * okButton = messageBox.button( QMessageBox::Ok ) )
            okButton->setMinimumWidth( 100 );

        messageBox.exec();
    }
}

void CRequestsViewWidget::OnComplite( int requestID )
{
    CAddCommentWindow * addCommentDlg = new CAddCommentWindow( requestID, this );
    if ( addCommentDlg->exec() == QDialog::Accepted )
    {
        DatabaseProcessing::CDatabaseManager * DBInstance = DatabaseProcessing::CDatabaseManager::Instance();
        DBInstance->EditRequestStatus( requestID, DatabaseProcessing::ERequestStatus::COMPLETED );
        LoadData();
    }
}

void CRequestsViewWidget::OnEdit( int requestID, ERequestWindowType type )
{
    CEditRequestWindow * editRequestDlg = new CEditRequestWindow( requestID, type, this );
    if ( editRequestDlg->exec() == QDialog::Accepted )
        LoadData();
}

void CRequestsViewWidget::OnQR( int requestID )
{
    QrCodeGenerator generator;
    QString data = QUrl( QString("http://192.168.168.207/?requestID=") + QString::number( requestID ) ).toString();
    QImage qrCodeImage = generator.generateQr( data );
    qrCodeImage = qrCodeImage.scaled( qrCodeImage.size() / 2, Qt::KeepAspectRatio, Qt::SmoothTransformation );
    
    QDialog qrDlg( this );
    QLabel * qrTL = new QLabel( &qrDlg );
    qrTL->setPixmap( QPixmap::fromImage( qrCodeImage ) );
    qrDlg.resize( qrCodeImage.size() );
    QVBoxLayout * qrGL = new QVBoxLayout( &qrDlg );
    qrGL->addWidget( qrTL );
    qrDlg.setLayout( qrGL );

    qrDlg.exec();
}

void CRequestsViewWidget::OnAccept( int requestID )
{
    DatabaseProcessing::CDatabaseManager * DBInstance = DatabaseProcessing::CDatabaseManager::Instance();
    DBInstance->EditRequestStatus( requestID, DatabaseProcessing::ERequestStatus::ATWORK );
    LoadData();
}

void CRequestsViewWidget::OnRemove( int requestID )
{
    DatabaseProcessing::CDatabaseManager * DBInstance = DatabaseProcessing::CDatabaseManager::Instance();
    DBInstance->EditRequestStatus( requestID, DatabaseProcessing::ERequestStatus::CANCELED );
    LoadData();
}

void CRequestsViewWidget::SetCurrentType( ERequestsViewType type )
{
    _curType = type;
    LoadData();
}

void CRequestsViewWidget::SetupWidgets()
{
    QVBoxLayout * mainVL = new QVBoxLayout( this );

    _queryModel = new QSqlQueryModel( this );
    _filterModel = new QSortFilterProxyModel( this );
    _requestsView = new QTableView( this );
    _requestsView->setSelectionBehavior( QAbstractItemView::SelectRows );
    _requestsView->verticalHeader()->setVisible( false );

    _exitBN = new QPushButton( QString::fromStdWString( L"Выход" ), this );
    _exitBN->setMaximumWidth( 100 );

    mainVL->addWidget( _exitBN, 0, Qt::AlignRight );

    {
        QHBoxLayout * filterHL = new QHBoxLayout;

        _customerFilterLE = new QLineEdit( this );
        _masterFilterLE = new QLineEdit( this );
        _modelFilterLE = new QLineEdit( this );

        filterHL->addWidget( new QLabel( QString::fromStdWString( L"ФИО пользоватея: " ), this ) );
        filterHL->addWidget( _customerFilterLE );
        filterHL->addWidget( new QLabel( QString::fromStdWString( L"ФИО мастер: " ), this ) );
        filterHL->addWidget( _masterFilterLE );
        filterHL->addWidget( new QLabel( QString::fromStdWString( L"Модель: " ), this ) );
        filterHL->addWidget( _modelFilterLE );

        mainVL->addLayout( filterHL );
    }

    {
        QHBoxLayout * filterHL = new QHBoxLayout;

        _useDateFilterCB = new QCheckBox( QString::fromStdWString( L"Использвать дату" ), this );
        _useDateFilterCB->setChecked( false );

        _startDE = new QDateEdit( this );
        _startDE->setMinimumWidth( 100 );
        _startDE->setCalendarPopup( true );
        _startDE->setDate( QDate::currentDate() );
        _startDE->setDisplayFormat( "yyyy-MM-dd" );
        _startDE->setEnabled( false );

        _finishDE = new QDateEdit( this );
        _finishDE->setMinimumWidth( 100 );
        _finishDE->setCalendarPopup( true );
        _finishDE->setDate( QDate::currentDate() );
        _finishDE->setDisplayFormat( "yyyy-MM-dd" );
        _finishDE->setEnabled( false );

        _statusFilterLE = new QLineEdit( this );

        filterHL->addWidget( _useDateFilterCB );
        filterHL->addWidget( new QLabel( QString::fromStdWString( L"Начало: " ), this ) );
        filterHL->addWidget( _startDE );
        filterHL->addWidget( new QLabel( QString::fromStdWString( L"Конец: " ), this ) );
        filterHL->addWidget( _finishDE );
        filterHL->addWidget( new QLabel( QString::fromStdWString( L"Статус: " ), this ) );
        filterHL->addWidget( _statusFilterLE );

        mainVL->addLayout( filterHL );
    }

    {
        QHBoxLayout * filterHL = new QHBoxLayout;

        _filterBN = new QPushButton( QString::fromStdWString( L"Отфильтровать" ), this );
        _filterBN->setFixedWidth( 120 );

        _clearFilterBN = new QPushButton( QString::fromStdWString( L"Сбросить фильтры" ), this );
        _clearFilterBN->setFixedWidth( 120 );

        QSpacerItem * leftHS = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
        _recordCountTL = new QLabel( this );
        _recordCountTL->setText( QString::fromStdWString( L"0 из 0" ) );

        filterHL->addWidget( _filterBN );
        filterHL->addWidget( _clearFilterBN );
        filterHL->addItem( leftHS );
        filterHL->addWidget( _recordCountTL );

        mainVL->addLayout( filterHL );
    }

    mainVL->addWidget( _requestsView );
    
    setLayout( mainVL );

    _filterModel->setSourceModel( _queryModel );
    _requestsView->setModel( _filterModel );
    _requestsView->setSortingEnabled( true );
    _requestsView->setContextMenuPolicy( Qt::ContextMenuPolicy::CustomContextMenu );
    _requestsView->horizontalHeader()->setStretchLastSection( true );
}

void CRequestsViewWidget::LoadData()
{
    DatabaseProcessing::CDatabaseManager * DBInstance = DatabaseProcessing::CDatabaseManager::Instance();
    QSqlQuery query;
    switch ( _curType )
    {
        case ManagerActiveRequests:
        {
            query = DBInstance->GetAllRequests( DatabaseProcessing::ERequestStatus::ATWORK );
        }
        break;
        case MasterActiveRequests:
        {
            query = DBInstance->GetMasterRequests( _currentUser.get() ,DatabaseProcessing::ERequestStatus::ATWORK );
        }
        break;
        case MasterHistory:
        {
            query = DBInstance->GetMasterRequests( _currentUser.get(), DatabaseProcessing::ERequestStatus::COMPLETED );
        }
        break;
        case CustomerActiveRequests:
        {
            query = DBInstance->GetCustomerRequests( _currentUser.get(), DatabaseProcessing::ERequestStatus::NEW );
        }
        break;
        case CustomerHistory:
        {
            query = DBInstance->GetCustomerRequests( _currentUser.get(), DatabaseProcessing::ERequestStatus::ALL );
        }
        break;
        case OperatorActiveRequests:
        {
            query = DBInstance->GetAllRequests( DatabaseProcessing::ERequestStatus::NEW );
        }
        break;
    }

    if ( query.exec() == false )
    {
        QString message = QString::fromStdWString( L"Не удалось получить данные." );
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
    _requestsView->setColumnHidden( 0, true );
    _totalRecords = _queryModel->rowCount();
    _filterModel->setSourceModel( _queryModel );
    _recordCountTL->setText( QString::fromStdWString( L"%1 из %2" ).arg( _totalRecords ).arg( _totalRecords ) );
}
