/*
    Exoplismou source code. Tak172. 2024.

    @Name:			CReportsWindow.h
    @Created:		03.10.2024
    @Programmer:	Timofey Kromachev

    Implementations. */

#include "stdafx.h"

#include "CReportsWindow.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSqlQueryModel>
#include <QTableView>
#include <QMessageBox>
#include <QHeaderView>
#include <QPushButton>

#include "../Database/CDatabaseManager.h"

CReportsWindow::CReportsWindow( int id, QWidget * parent )
    : QDialog( parent ), _id( id )
{
    SetupWidgets();
    LoadData();
}

void CReportsWindow::SetupWidgets()
{
    QVBoxLayout * mainVL = new QVBoxLayout;

    _reportsView = new QTableView( this );
    _reportsView->setSelectionBehavior( QAbstractItemView::SelectRows );
    _reportsView->verticalHeader()->setVisible( false );

    mainVL->addWidget( _reportsView );

    setLayout( mainVL );

    _queryModel = new QSqlQueryModel( this );
    _reportsView->setModel( _queryModel );
    _reportsView->setSortingEnabled( true );
    _reportsView->horizontalHeader()->setStretchLastSection( true );
}

void CReportsWindow::LoadData()
{
    DatabaseProcessing::CDatabaseManager * DBInstance = DatabaseProcessing::CDatabaseManager::Instance();
    QSqlQuery query = DBInstance->GetMasterReports( _id );
    if ( query.exec() == false )
    {
        QString message = QString::fromStdWString( L"Не удалось получить данные мастера." );
        QMessageBox messageBox;
        messageBox.setWindowTitle( QString::fromStdWString( L"Ошибка запроса" ) );
        messageBox.setText( message );
        messageBox.setIcon( QMessageBox::Warning );
        messageBox.setStandardButtons( QMessageBox::Ok );
        if ( QAbstractButton * okButton = messageBox.button( QMessageBox::Ok ) )
            okButton->setMinimumWidth( 100 );

        messageBox.exec();
        reject();
        return;
    }

    _queryModel->setQuery( query );
    setFixedWidth( 700 );
}