/*
    Exoplismou source code. Tak172. 2024.

    @Name:			CManagerMenuWidget.h
    @Created:		01.10.2024
    @Programmer:	Timofey Kromachev

    Class of manager menu widget. */

#include <QWidget>

#include "CRequestsViewWidget.h"
#include "CMastersWidget.h"

class QPushButton;
class CManagerMenuWidget : public QWidget
{
    Q_OBJECT

public:
    CManagerMenuWidget( QWidget * parent = nullptr );

signals:
    void ShowRequests( ERequestsViewType type );
    void AddMaster();
    void ShowMasters( EMasterWidgetType type );

private slots:
    void OnShowActiveOrders();
    void OnShowMasters();
    void OnAddMaster();

private:
    void SetupWidgets();

    QPushButton * _showActiveOrdersBN;
    QPushButton * _showMastersBN;
    QPushButton * _addMasterBN;
};