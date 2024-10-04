#include "stdafx.h"

#include <QApplication>
#include <QMessageBox>
#include <QPushButton>

#include "Database/CDatabaseManager.h"
#include "Windows/CExoplismouMainWindow.h"
#include "Windows/CAuthentificationWindow.h"

int main( int argc, char * argv[ ] )
{
    QApplication app( argc, argv );

    /*QIcon appIcon(":/Plousios/logo");
    QApplication::setWindowIcon( appIcon );*/

    //QFile styleFile( ":/qss/obit" );
    //styleFile.open( QFile::ReadOnly );
    //QString styleSheet = QLatin1String( styleFile.readAll() );
    //qApp->setStyleSheet( styleSheet );

    DatabaseProcessing::CDatabaseManager * DBInstance = DatabaseProcessing::CDatabaseManager::Instance();
    if ( !DBInstance->IsOpen() )
    {
        QString message = QString::fromStdWString( L"Не удалось установить подключение к базе." );
        QMessageBox messageBox;
        messageBox.setWindowTitle( QString::fromStdWString( L"Ошибка подключения" ) );
        messageBox.setText( message );
        messageBox.setIcon( QMessageBox::Warning );
        messageBox.setStandardButtons( QMessageBox::Ok );
        if ( QAbstractButton * okButton = messageBox.button( QMessageBox::Ok ) )
            okButton->setMinimumWidth( 100 );

        messageBox.exec();
        app.quit();
        exit( 1 );
    }

    CExoplismouMainWindow mainWindow;
    mainWindow.Login();

    return app.exec();
}