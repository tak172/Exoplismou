/*
    Exoplismou source code. Tak172. 2024.

    @Name:			CCommentsWindow.h
    @Created:		03.10.2024
    @Programmer:	Timofey Kromachev

    Implementations. */

#include "stdafx.h"

#include "CCommentsWindow.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSqlQueryModel>
#include <QTableView>
#include <QMessageBox>
#include <QHeaderView>
#include <QPushButton>
#include <QLabel>

#include "../Database/CDatabaseManager.h"

CCommentsWindow::CCommentsWindow( QWidget * parent )
    : QDialog( parent )
{
    SetupWidgets();
    LoadData();
}

void CCommentsWindow::SetupWidgets()
{
    QVBoxLayout * mainVL = new QVBoxLayout;

    _reviewView = new QTableView( this );
    _reviewView->setSelectionBehavior( QAbstractItemView::SelectRows );
    _reviewView->verticalHeader()->setVisible( false );

    _midTimeTL = new QLabel( this );

    mainVL->addWidget( _reviewView );
    mainVL->addWidget( _midTimeTL, Qt::AlignLeft );

    setLayout( mainVL );

    _queryModel = new QSqlQueryModel( this );
    _reviewView->setModel( _queryModel );
    _reviewView->setSortingEnabled( true );
    _reviewView->horizontalHeader()->setStretchLastSection( true );
}

void CCommentsWindow::LoadData()
{
    DatabaseProcessing::CDatabaseManager * DBInstance = DatabaseProcessing::CDatabaseManager::Instance();
    QSqlQuery query = DBInstance->GetReviews();
    if ( query.exec() == false )
    {
        QString message = QString::fromStdWString( L"Не удалось получить данные отзывов." );
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

    _midTimeTL->setText( QString::fromStdWString( L"Среднее время выполнения: " ) +
        QString::number( DBInstance->GetMidTime() + 1 ) );

    setFixedWidth( 700 );
}