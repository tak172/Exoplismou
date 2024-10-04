/*
    Exoplismou source code. Tak172. 2024.

    @Name:			CAuthHistoryWidget.h
    @Created:		30.09.2024
    @Programmer:	Timofey Kromachev

    Class of authentification history widget. */

#include <QWidget>

class QSqlQueryModel;
class QSortFilterProxyModel;
class QTableView;
class QLineEdit;
class QDateEdit;
class QLabel;
class QPushButton;
class CAuthHistoryWidget : public QWidget
{
    Q_OBJECT

public:
    CAuthHistoryWidget( QWidget * parent = nullptr );
    void LoadLoginHistory();
    
signals:
    void Exit();

private slots:
    void FilterData();
    void ResetFilters();
    void OnExit();

private:
    void SetupWidgets();

    QSqlQueryModel * _queryModel;
    QSortFilterProxyModel * _filterModel;
    QTableView * _historyView;
    QLineEdit * _usernameFilterLE;
    QDateEdit * _dateFilterDE;
    QLabel * _recordCountTL;
    QPushButton * _filterBN;
    QPushButton * _clearFilterBN;
    QPushButton * _exitBN;

    int _totalRecords;
};