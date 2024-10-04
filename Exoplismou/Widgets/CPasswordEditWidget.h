/*
    Exoplismou source code. Tak172. 2024.

    @Name:			CPasswordEditWidget.h
    @Created:		01.10.2024
    @Programmer:	Timofey Kromachev

    Class of password edit widget. */

#include <QWidget>

class QLineEdit;
class QPushButton;
class CPasswordEditWidget : public QWidget
{
    Q_OBJECT

public:
    CPasswordEditWidget( QWidget * parent = nullptr );
    void SetText( const QString & text );
    QString GetPassword() const;

private slots:
    void TogglePasswordVisibility();

private:
    QLineEdit * _passwordLE;
    QPushButton * _toggleBN;
};