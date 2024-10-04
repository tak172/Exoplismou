/*
    Exoplismou source code. Tak172. 2024.

    @Name:			CDatabaseManager.h
    @Created:		29.09.2024
    @Programmer:	Timofey Kromachev

    Class of database manager. */

#pragma once

#include "DatabaseHelper.h"

#include <QtSql> 
#include <QtCore>

namespace DatabaseProcessing
{
    // Class of database manager
    class CDatabaseManager
    {
        friend bool Transaction( QSqlQuery & query );
        friend bool Rollback( QSqlQuery & query );
        friend bool Commit( QSqlQuery & query );

    public:
        static CDatabaseManager * Instance();
        ~CDatabaseManager();

        inline bool IsOpen() { return _db.isOpen(); }

        SUserData * GetUserData( const QString & login ) const;
        SUserData * GetUserData( unsigned userID ) const;
        bool SetUserData( SUserData * userData, const SUserData * newData );
        bool IsLoginUnique( const QString & login );
        bool AddMaster( SUserData * userData );

        int GetMidTime();
        QSqlQuery GetReviews() const;

        bool AddAuthentification( const SUserData * userData, bool isActive );
        QSqlQuery GetAuthHistory() const;

        QSqlQuery GetMasters() const;
        QSqlQuery GetMasterReports( unsigned masterID ) const;

        QSqlQuery GetAllRequests( ERequestStatus status = ERequestStatus::ALL ) const;
        QSqlQuery GetMasterRequests( const SUserData * userData, ERequestStatus status = ERequestStatus::ALL );
        QSqlQuery GetCustomerRequests( const SUserData * userData, ERequestStatus status = ERequestStatus::ALL );

        bool AddExtension( int requestID );
        bool RemoveExtension( bool isAccepted, int extID );
        QSqlQuery GetUserExtensions( int userID );
        bool EditRequestStatus( int requestID, ERequestStatus status );

        SDeviceModel * GetDeviceModelData( int modelID ) const;
        SDeviceType * GetDeviceTypeData( int typeID ) const;

        SRequestData * GetRequestData( int requestID ) const;

        bool AddMasterComment( SRequestData * requestData );
        QList<QString> GetMasterNames() const;
        QList<QString> GetAllDeviceTypes() const;
        QList<QString> GetAllModels( int typeID ) const;
        QList<QString> GetDeviceModelsByTypeName( const QString & typeName ) const;
        unsigned GetDeviceModelIDByName( const QString & modelName ) const;

        unsigned GetMasterWithFewestOrders() const;
        bool AddRequest( unsigned modelID, const QString & problemDescription, unsigned clientID, const QDate & finishDate );
        bool UpdateRequest( int requestID, int newModelID, int newMasterID, const QString & newComment, const QDate & newCompletionDate );

        unsigned GetMasterIDByName( const QString & masterName ) const;

    private:
        CDatabaseManager();
        CDatabaseManager( const CDatabaseManager & ) = delete;
        CDatabaseManager & operator=( const CDatabaseManager & ) = delete;

        QSqlDatabase _db;
        static CDatabaseManager * _instance;
    };
}