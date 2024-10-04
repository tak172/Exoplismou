/*
    Exoplismou source code. Tak172. 2024.

    @Name:			CExtensionsWidget.h
    @Created:		03.10.2024
    @Programmer:	Timofey Kromachev

    Class of extensions widget. */

#pragma once

#include <QWidget>

class QSqlQueryModel;
class QTableView;
class QPushButton;
class CExtensionsWidget : public QWidget
{
    Q_OBJECT

public:
    CExtensionsWidget( QWidget * parent = nullptr );
    void LoadData();

signals:
    void Exit();

private slots:
    void OnExit();
    void OnCustomContextMenu( QPoint pos );
    void OnAccept( int extID );
    void OnReject( int extID );

private:
    void SetupWidgets();

    QSqlQueryModel * _queryModel;
    QTableView * _extView;

    QPushButton * _exitBN;
};