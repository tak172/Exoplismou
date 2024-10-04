/*
    Exoplismou source code. Tak172. 2024.

    @Name:			CPasswordEditWidget.cpp
    @Created:		01.10.2024
    @Programmer:	Timofey Kromachev

    Implementations. */

#include "stdafx.h"

#include "CPasswordEditWidget.h"

#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>

CPasswordEditWidget::CPasswordEditWidget( QWidget * parent )
    : QWidget( parent )
{
    _passwordLE = new QLineEdit( this );
    _passwordLE->setEchoMode( QLineEdit::Password );

    _toggleBN = new QPushButton( this );
    _toggleBN->setIcon( QIcon( ":/Icons/closed_eye" ) );

    QHBoxLayout * mainHL = new QHBoxLayout( this );
    mainHL->setContentsMargins( 0, 0, 0, 0 ); 
    mainHL->setSpacing( 0 ); 
    mainHL->addWidget( _passwordLE );
    mainHL->addWidget( _toggleBN );
    setLayout( mainHL );

    connect( _toggleBN, &QPushButton::clicked, this, &CPasswordEditWidget::TogglePasswordVisibility );
}

void CPasswordEditWidget::SetText( const QString & text )
{
    _passwordLE->setText( text );
}

QString CPasswordEditWidget::GetPassword() const
{
    return _passwordLE->text();
}

void CPasswordEditWidget::TogglePasswordVisibility() 
{
    if ( _passwordLE->echoMode() == QLineEdit::Password )
    {
        _passwordLE->setEchoMode( QLineEdit::Normal ); 
        _toggleBN->setIcon( QIcon( ":/Icons/opened_eye" ) );
    }
    else
    {
        _passwordLE->setEchoMode( QLineEdit::Password );
        _toggleBN->setIcon( QIcon( ":/Icons/closed_eye" ) );
    }
}