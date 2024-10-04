/*
    Exoplismou source code. Tak172. 2024.

    @Name:			CEditRequestWindow.h
    @Created:		03.10.2024
    @Programmer:	Timofey Kromachev

    Class of edit requests window. */

#pragma once

#include <QDialog>

enum ERequestWindowType
{
    Addition,
    UserEditing,
    OperatorEditing
};

class QLineEdit;
class QPushButton;
class QComboBox;
class QTextEdit;
class QLabel;
class QDateEdit;
class CEditRequestWindow : public QDialog
{
public:
    CEditRequestWindow( int requestID, ERequestWindowType type, QWidget * parent = nullptr );

private slots:
    void OnOk();

private:
    void SetupWidgets();

    QLineEdit * _userLoginLE;
    QComboBox * _masterNameCB;
    QComboBox * _modelCB;
    QComboBox * _typeCB;
    QTextEdit * _commentTE;
    QPushButton * _okBN;
    QPushButton * _cancelBN;
    QDateEdit * _startDateDE;
    QDateEdit * _finishDateDE;

    int _requestID;
    ERequestWindowType _type;
};
