/*
    Exoplismou source code. Tak172. 2024.

    @Name:			CCommentsWindow.h
    @Created:		03.10.2024
    @Programmer:	Timofey Kromachev

    Class of reports dialog. */

#pragma once 

#include <QDialog>

class QSqlQueryModel;
class QTableView;
class QLabel;

class CCommentsWindow : public QDialog
{
    Q_OBJECT

public:
    CCommentsWindow( QWidget * parent = nullptr );
    void LoadData();

private:
    void SetupWidgets();

    QSqlQueryModel * _queryModel;
    QTableView * _reviewView;
    QLabel * _midTimeTL;
};