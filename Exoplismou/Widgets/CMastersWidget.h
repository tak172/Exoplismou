/*
    Exoplismou source code. Tak172. 2024.

    @Name:			CMastersWidget.h
    @Created:		03.10.2024
    @Programmer:	Timofey Kromachev

    Class of masters widget. */

#pragma once 

#include <QWidget>

class QSqlQueryModel;
class QSortFilterProxyModel;
class QTableView;
class QLineEdit;
class QDateEdit;
class QLabel;
class QPushButton;

enum EMasterWidgetType
{
    ManagerEdit,
    OperatorEdit
};

class CMastersWidget : public QWidget
{
    Q_OBJECT

public:
    CMastersWidget( QWidget * parent = nullptr );
    void LoadData( EMasterWidgetType type );

signals:
    void Exit();

private slots:
    void FilterData();
    void ResetFilters();
    void OnExit();
    void OnCustomContextMenu( QPoint pos );
    void OnReports( int masterID );

private:
    void SetupWidgets();

    QSqlQueryModel * _queryModel;
    QSortFilterProxyModel * _filterModel;
    QTableView * _mastersView;
    QLineEdit * _usernameFilterLE;
    QLabel * _recordCountTL;
    QPushButton * _filterBN;
    QPushButton * _clearFilterBN;
    QPushButton * _exitBN;

    int _totalRecords;
    EMasterWidgetType _type;
};