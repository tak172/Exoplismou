/*
    Exoplismou source code. Tak172. 2024.

    @Name:			COperatorMenuWidget.h
    @Created:		01.10.2024
    @Programmer:	Timofey Kromachev

    Class of operator menu widget. */

#include <QWidget>

#include "CRequestsViewWidget.h"
#include "CMastersWidget.h"

class QPushButton;
class COperatorMenuWidget : public QWidget
{
    Q_OBJECT

public:
    COperatorMenuWidget( QWidget * parent = nullptr );

signals:
    void ShowRequests( ERequestsViewType type );
    void ShowMasters( EMasterWidgetType type );

private slots:
    void OnShowOrders();
    void OnShowMasters();
    void OnShowReports();

private:
    void SetupWidgets();

    QPushButton * _showOrdersBN;
    QPushButton * _showMastersBN;
    QPushButton * _showReportsBN;
};