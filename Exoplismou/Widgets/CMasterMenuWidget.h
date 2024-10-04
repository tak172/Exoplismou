/*
    Exoplismou source code. Tak172. 2024.

    @Name:			CMasterMenuWidget.h
    @Created:		01.10.2024
    @Programmer:	Timofey Kromachev

    Class of master menu widget. */

#include <QWidget>

#include "CRequestsViewWidget.h"

class QPushButton;
class CMasterMenuWidget : public QWidget
{
    Q_OBJECT

public:
    CMasterMenuWidget( QWidget * parent = nullptr );

signals:
    void ShowRequests( ERequestsViewType type );

private slots:
    void OnShowActiveOrders();
    void OnShowOrdersHistory();

private:
    void SetupWidgets();

    QPushButton * _showActiveOrdersBN;
    QPushButton * _showOrdersHistoryBN;
};