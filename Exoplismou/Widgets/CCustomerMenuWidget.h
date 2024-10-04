/*
    Exoplismou source code. Tak172. 2024.

    @Name:			CCustomerMenuWidget.h
    @Created:		01.10.2024
    @Programmer:	Timofey Kromachev

    Class of customer menu widget. */

#include <QWidget>

#include "CRequestsViewWidget.h"

class QPushButton;
class CCustomerMenuWidget : public QWidget
{
    Q_OBJECT

public:
    CCustomerMenuWidget( QWidget * parent = nullptr );

signals:
    void ShowRequests( ERequestsViewType type );
    void ShowExt();

private slots:
    void OnAddRequest();
    void OnShowActiveOrders();
    void OnShowOrdersHistory();
    void OnShowExt();

private:
    void SetupWidgets();

    QPushButton * _addRequestBN;
    QPushButton * _showActiveOrdersBN;
    QPushButton * _showOrdersHistoryBN;
    QPushButton * _showRequestsBN;
};