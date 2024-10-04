/*
    Exoplismou source code. Tak172. 2024.

    @Name:			CEditUserDataWidget.h
    @Created:		01.10.2024
    @Programmer:	Timofey Kromachev

    Class of edit user data widget. */

#include <QWidget>

class QLineEdit;
class QLabel;
class QPushButton;
class CPasswordEditWidget;

enum EEditType
{
    CreateManager,
    EditUser
};

class CEditUserDataWidget : public QWidget
{
    Q_OBJECT

public:
    CEditUserDataWidget( QWidget * parent = nullptr );
    void UpdateUserData( EEditType type );

signals:
    void Exit();

private slots:
    void OnSaveClicked();
    void OnPhotoClicked();
    void OnExit();

private:
    void SetupWidgets();

    QLineEdit * _loginLE;
    CPasswordEditWidget * _passwordLE;
    QLineEdit * _phoneLE;
    QLineEdit * _fullNameLE;
    QLabel * _photoLabel;
    QPushButton * _saveBN;
    QPushButton * _photoBN;
    QPushButton * _exitBN;

    EEditType _type;
};