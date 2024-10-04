/*
    Exoplismou source code. Tak172. 2024.

    @Name:			CExoplismouMainWindow.h
    @Created:		30.09.2024
    @Programmer:	Timofey Kromachev

    Class of exoplismou main window. */

#pragma once

#include <QMainWindow>

#include "../Widgets/CRequestsViewWidget.h"
#include "../Widgets/CMastersWidget.h"

class CMenuTemplateWidget;
class CAuthHistoryWidget;
class QStackedWidget;
class CEditUserDataWidget;
class CExtensionsWidget;
class CExoplismouMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    CExoplismouMainWindow( QWidget * parent = nullptr );

public slots:
    void Login();
    void LoginHistory();
    void ExitToMenu();
    void ChangeProfile();
    void ShowRequests( ERequestsViewType );
    void ShowExt();
    void AddMaster();
    void ShowMasters( EMasterWidgetType );

private:
    void SetupWidgets();
    void LoginSuccessful();

    CMenuTemplateWidget * _menuTemplateWidget;
    CAuthHistoryWidget * _authHistoryWidget;
    CEditUserDataWidget * _editUserDataWidget;
    CRequestsViewWidget * _requestsViewWidget;
    CExtensionsWidget * _extensionsWidget;
    CMastersWidget * _masterWidget;

    QStackedWidget * _stackedWidget;
};