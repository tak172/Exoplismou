/*
    Exoplismou source code. Tak172. 2024.

    @Name:			CRequestsViewWidget.h
    @Created:		01.10.2024
    @Programmer:	Timofey Kromachev

    Class of requests view widget. */

#pragma once

#include <QWidget>

class QSqlQueryModel;
class QSortFilterProxyModel;
class QTableView;
class QLineEdit;
class QDateEdit;
class QLabel;
class QPushButton;
class QCheckBox;

enum ERequestsViewType
{
    ManagerActiveRequests,
    MasterActiveRequests,
    MasterHistory,
    CustomerActiveRequests,
    CustomerHistory,
    OperatorActiveRequests
};

enum ERequestWindowType;

class CRequestsViewWidget : public QWidget
{
    Q_OBJECT

public:
    CRequestsViewWidget( QWidget * parent = nullptr );
    void SetCurrentType( ERequestsViewType type );

signals:
    void Exit();

private slots:
    void FilterData();
    void ResetFilters();
    void OnExit();
    void UseDataFilter( bool state );
    void OnCustomContextMenu( QPoint pos );

    void OnExtend( int requestID );
    void OnComplite( int requestID );
    void OnEdit( int requestID, ERequestWindowType );
    void OnQR( int requestID );
    void OnAccept( int requestID );
    void OnRemove( int requestID );

private:
    void SetupWidgets();
    void LoadData();

    QSqlQueryModel * _queryModel;
    QSortFilterProxyModel * _filterModel;
    QTableView * _requestsView;

    QLineEdit * _masterFilterLE;
    QLineEdit * _customerFilterLE;
    QLineEdit * _modelFilterLE;
    QDateEdit * _startDE;
    QDateEdit * _finishDE;
    QLineEdit * _statusFilterLE;
    QLabel * _recordCountTL;
    QPushButton * _filterBN;
    QPushButton * _clearFilterBN;
    QPushButton * _exitBN;
    QCheckBox * _useDateFilterCB;

    int _totalRecords;
    ERequestsViewType _curType;
};