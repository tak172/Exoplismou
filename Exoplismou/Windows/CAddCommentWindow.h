/*
    Exoplismou source code. Tak172. 2024.

    @Name:			CAddCommentWindow.h
    @Created:		02.10.2024
    @Programmer:	Timofey Kromachev

    Class of add comment window. */

#pragma once

#include <QDialog>

class QLineEdit;
class QPushButton;
class QTextEdit;
class QLabel;
class CAddCommentWindow : public QDialog
{
public:
    CAddCommentWindow( int requestID, QWidget * parent = nullptr );

private slots:
    void OnOk();

private:
    void SetupWidgets();

    QLineEdit * _userNameLE;
    QLineEdit * _masterNameLE;
    QLineEdit * _modelLE;
    QLineEdit * _typeLE;
    QTextEdit * _commentTE;
    QPushButton * _okBN;
    QPushButton * _cancelBN;

    int _requestID;
};
