/*
    Exoplismou source code. Tak172. 2024.

    @Name:			CReportsWindow.h
    @Created:		03.10.2024
    @Programmer:	Timofey Kromachev

    Class of reports dialog. */

#pragma once 

#include <QDialog>

class QSqlQueryModel;
class QTableView;

class CReportsWindow : public QDialog
{
    Q_OBJECT

public:
    CReportsWindow( int id, QWidget * parent = nullptr );
    void LoadData();

private:
    void SetupWidgets();

    QSqlQueryModel * _queryModel;
    QTableView * _reportsView;

    int _id;
};