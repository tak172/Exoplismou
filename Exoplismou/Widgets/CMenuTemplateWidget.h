/*
    Exoplismou source code. Tak172. 2024.

    @Name:			CMenuTemplateWidget.h
    @Created:		01.10.2024
    @Programmer:	Timofey Kromachev

    Class of menu template widget. */

#include <QWidget>

#include "CRequestsViewWidget.h"
#include "CMastersWidget.h"

class QLabel;
class QPushButton;
class QGridLayout;
class QStackedWidget;
class CCustomerMenuWidget;
class CMasterMenuWidget;
class CManagerMenuWidget;
class COperatorMenuWidget;
class CMenuTemplateWidget : public QWidget
{
    Q_OBJECT

public:
    CMenuTemplateWidget( QWidget * parent = nullptr );
    void SetupUserData();

signals:
    void Exit();
    void LoginHistory();
    void ChangeProfile();
    void ShowRequests( ERequestsViewType );
    void ShowExt();
    void AddMaster();
    void ShowMasters( EMasterWidgetType type );

public slots:
    void OnExit();
    void ShowLoginHistory();
    void OnChangeProfile();
    void OnShowRequests( ERequestsViewType type );
    void OnShowExt();
    void OnAddMaster();
    void OnShowMasters( EMasterWidgetType type );

private:
    void SetupWidgets();

    QLabel * _photoTL;
    QLabel * _fullNameTL;
    QLabel * _phoneNumberTL;
    QLabel * _userRoleTL;

    QPushButton * _changeProfileDataBN;
    QPushButton * _exitPB;
    QPushButton * _loginHistoryPB;

    QStackedWidget * _stackedWidget;

    CCustomerMenuWidget * _customerMenuWidget;
    CMasterMenuWidget * _masterMenuWidget;
    CManagerMenuWidget * _managerMenuWidget;
    COperatorMenuWidget * _operatorMenuWidget;
};