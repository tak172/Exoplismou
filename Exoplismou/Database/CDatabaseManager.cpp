/*
    Exoplismou source code. Tak172. 2024.

    @Name:			CDatabaseManager.cpp
    @Created:		29.09.2024
    @Programmer:	Timofey Kromachev

    Implementations. */

#include "stdafx.h"

#include "CDatabaseManager.h"

#include <exception>

static QString host = "tak172-VirtualBox";
static QString dbName = "exoplismou";
static QString user = "tak172";
static QString password = "Exoplismou1!";

namespace DBP = DatabaseProcessing;

static bool DatabaseProcessing::Transaction( QSqlQuery & query )
{
    DatabaseProcessing::CDatabaseManager * DBInstance = DatabaseProcessing::CDatabaseManager::Instance();
    if ( DBInstance->_db.driver()->hasFeature( QSqlDriver::Transactions ) == false )
        return query.exec( "START TRANSACTION" );
    else
        return DBInstance->_db.transaction();
}

static bool DatabaseProcessing::Rollback( QSqlQuery & query )
{
    DatabaseProcessing::CDatabaseManager * DBInstance = DatabaseProcessing::CDatabaseManager::Instance();
    if ( DBInstance->_db.driver()->hasFeature( QSqlDriver::Transactions ) == false )
        return query.exec( "ROLLBACK" );
    else
        return DBInstance->_db.rollback();
}

static bool DatabaseProcessing::Commit( QSqlQuery & query )
{
    DatabaseProcessing::CDatabaseManager * DBInstance = DatabaseProcessing::CDatabaseManager::Instance();
    if ( DBInstance->_db.driver()->hasFeature( QSqlDriver::Transactions ) == false )
        return query.exec( "COMMIT" );
    else
        return DBInstance->_db.commit();
}

#define CHECK_QUERY_AND_ROLLBACK if (!(query).exec()) { if (!Rollback(query)) { \
            throw std::runtime_error("Failed to rollback transaction!"); } return false; }

#define UPDATE_FIELD_USERS(field, value) \
    query.prepare("UPDATE Users SET " #field " = :newValue WHERE userID = :id"); \
    query.bindValue(":newValue", value); query.bindValue(":id", userData->_id); \
    CHECK_QUERY_AND_ROLLBACK;

DBP::CDatabaseManager * DBP::CDatabaseManager::_instance = nullptr;

DBP::CDatabaseManager * DBP::CDatabaseManager::Instance()
{
    if ( _instance == nullptr )
        _instance = new CDatabaseManager();

    return _instance;
}

DBP::CDatabaseManager::~CDatabaseManager()
{
    if ( _db.isOpen() )
        _db.close();
}

DBP::CDatabaseManager::CDatabaseManager()
{
    _db = QSqlDatabase::addDatabase( "QMYSQL" );
    _db.setHostName( host );
    _db.setDatabaseName( dbName );
    _db.setUserName( user );
    _db.setPassword( password );
    _db.open();
}

DBP::SUserData * DBP::CDatabaseManager::GetUserData( const QString & login ) const
{
    QSqlQuery query( _db );
    query.prepare( "SELECT * FROM Users WHERE login = :login" );
    query.bindValue( ":login", login );

    SUserData * result = nullptr;
    if ( query.exec() && query.next() )
    {
        unsigned id = query.value( "userID" ).toUInt();
        QString fullName = query.value( "fio" ).toString();
        QString phone = query.value( "phone" ).toString();
        QString password = query.value( "password" ).toString();
        QByteArray photo = query.value( "photo" ).toByteArray();
        int userStatusID = query.value( "userStatusID" ).toInt();
        result = new SUserData { id, login, password, phone, fullName, EUserStatus( userStatusID - 1), photo };
    }

    return result;
}

DBP::SUserData * DBP::CDatabaseManager::GetUserData( unsigned userID ) const
{
    QSqlQuery query( _db );
    query.prepare( "SELECT * FROM Users WHERE userID = :userID" );
    query.bindValue( ":userID", userID );

    SUserData * result = nullptr;
    if ( query.exec() && query.next() )
    {
        unsigned id = query.value( "userID" ).toUInt();
        QString login = query.value( "login" ).toString();
        QString fullName = query.value( "fio" ).toString();
        QString phone = query.value( "phone" ).toString();
        QString password = query.value( "password" ).toString();
        QByteArray photo = query.value( "photo" ).toByteArray();
        int userStatusID = query.value( "userStatusID" ).toInt();
        result = new SUserData { userID, login, password, phone, fullName, EUserStatus( userStatusID - 1 ), photo };
    }

    return result;
}

bool DBP::CDatabaseManager::SetUserData( SUserData * userData, const SUserData * newData )
{
    if ( userData == nullptr || newData == nullptr )
        return false;

    QSqlQuery query( _db );
    if ( Transaction( query ) == false )
        return false;

    UPDATE_FIELD_USERS( login, newData->_login );
    UPDATE_FIELD_USERS( password, newData->_password );
    UPDATE_FIELD_USERS( fio, newData->_fullName );
    UPDATE_FIELD_USERS( phone, newData->_phone );
    UPDATE_FIELD_USERS( photo, newData->_photo );
    UPDATE_FIELD_USERS( userStatusID, ( ( int ) newData->_status  + 1) );

    if ( Commit( query ) == false )
        throw std::runtime_error( "Failed to commit transaction!" );

    userData->_login = newData->_login;
    userData->_fullName = newData->_fullName;
    userData->_phone = newData->_phone;
    userData->_password = newData->_password;
    userData->_photo = newData->_photo;
    return true;
}

bool DatabaseProcessing::CDatabaseManager::IsLoginUnique( const QString & login )
{
    QSqlQuery query( _db );
    query.prepare( "SELECT COUNT(*) FROM Users WHERE login = :login" );
    query.bindValue( ":login", login );

    if ( !query.exec() )
        return false;

    if ( query.next() )
    {
        int count = query.value( 0 ).toInt();
        return count == 0;
    }

    return false;
}

int DatabaseProcessing::CDatabaseManager::GetMidTime()
{
    QSqlQuery query( _db );
    query.prepare( QString::fromStdWString( L"SELECT AVG(TIMESTAMPDIFF(DAY, startDate, completionDate)) "
        L"AS average_completion_time FROM Requests WHERE statusID = 3;" ) );
    query.exec();

    if ( query.exec() )
        if ( query.next() )
            return query.value( 0 ).toInt(); 

    return 0;
}

QSqlQuery DatabaseProcessing::CDatabaseManager::GetReviews() const
{
    QSqlQuery query( _db );
    query.prepare( QString::fromStdWString( L"SELECT reviewText AS 'Отзыв', rating AS 'Рейтинг' "
        L"FROM Review;" ) );

    return query;
}

bool DatabaseProcessing::CDatabaseManager::AddAuthentification( const SUserData * userData, bool isActive )
{
    if ( userData == nullptr )
        return false;

    QSqlQuery query( _db );
    query.prepare( "INSERT INTO AuthHistory (userID, authentificationDate, isActive) "
        "VALUES (:id, CURDATE(), :active);" );
    query.bindValue( ":id", userData->_id );
    query.bindValue( ":active", (int) isActive );
    if ( !( query ).exec() )
        return false;

    return true;
}

QSqlQuery DatabaseProcessing::CDatabaseManager::GetAuthHistory() const
{
    QSqlQuery query( _db );
    query.prepare( QString::fromStdWString(
        L"SELECT U.fio AS ФИО, A.authentificationDate AS 'Дата входа', "
        L"CASE WHEN A.isActive = 1 THEN 'Успешно' ELSE 'Неудачно' END AS 'Удачность входа' "
        L"FROM Users U JOIN AuthHistory A ON U.userID = A.userID;" ) );

    return query;
}

QSqlQuery DatabaseProcessing::CDatabaseManager::GetMasters() const
{
    QSqlQuery query( _db );
    query.prepare( QString::fromStdWString(
        L"SELECT userID, fio AS 'ФИО мастера', phone AS 'Телефон' "
        L"FROM Users WHERE userStatusID = 2;" ) );

    return query;
}

QSqlQuery DatabaseProcessing::CDatabaseManager::GetMasterReports( unsigned masterID ) const
{
    QSqlQuery query( _db );
    query.prepare( QString::fromStdWString(
        L"SELECT u.fio AS 'ФИО клиента', dm.modelName AS 'Модель', r.problemDescription AS 'Описание', "
        L" c.message AS Отчет FROM Requests r JOIN Users u ON r.clientID = u.userID JOIN DeviceModels dm ON r.modelID"
        L" = dm.modelID JOIN Comments c ON r.requestID = c.requestID WHERE c.masterID = :masterId;" ) );
    query.bindValue( ":masterId", masterID );

    return query;
}

QSqlQuery DatabaseProcessing::CDatabaseManager::GetAllRequests( ERequestStatus status ) const
{
    QSqlQuery query( _db );
    QString queryStr = QString::fromStdWString(
        L"SELECT r.requestID AS 'ID запроса', r.startDate AS 'Дата начала', r.completionDate AS 'Дата конца', "
        L"r.problemDescription AS 'Описание', dm.modelName AS 'Модель', rs.statusName AS 'Статус', u.fio AS 'Клиент', "
        L"um.fio AS 'Мастер' FROM Requests r "
        L"JOIN DeviceModels dm ON r.modelID = dm.modelID "
        L"JOIN RequestStatuses rs ON r.statusID = rs.statusID "
        L"JOIN Users u ON r.clientID = u.userID "
        L"JOIN Users um ON r.masterID = um.userID " );

    if ( status != ERequestStatus::ALL )
        queryStr += "WHERE r.statusID = :statusID";

    queryStr += ';';
    query.prepare( queryStr );

    if ( status != ERequestStatus::ALL )
        query.bindValue( ":statusID", ( int ) status );

    return query;
}

QSqlQuery DatabaseProcessing::CDatabaseManager::GetMasterRequests( const SUserData * userData, ERequestStatus status )
{
    QSqlQuery query( _db );
    QString queryStr = QString::fromStdWString(
        L"SELECT r.requestID AS \'ID запроса\', r.startDate AS \'Дата начала\', r.completionDate AS \'Дата конца\', r.problemDescription AS \'Описание\', "
        L"dm.modelName AS \'Модель\', rs.statusName AS \'Статус\', u.fio AS \'Клиент\', um.fio AS \'Мастер\' "
        L"FROM Requests r JOIN DeviceModels dm ON r.modelID = dm.modelID JOIN RequestStatuses rs ON r.statusID = rs.statusID "
        L"JOIN Users u ON r.clientID = u.userID JOIN Users um ON r.masterID = um.userID WHERE r.masterID = :masterId " );

    if ( status != ERequestStatus::ALL )
        queryStr += "AND r.statusID = :statusID";

    queryStr += ';';
    query.prepare( queryStr );
    query.bindValue( ":masterId", userData->_id );
    if ( status != ERequestStatus::ALL )
        query.bindValue( ":statusID", ( int ) status );

    return query;
}

QSqlQuery DatabaseProcessing::CDatabaseManager::GetCustomerRequests( const SUserData * userData, ERequestStatus status )
{
    QSqlQuery query( _db );
    QString queryStr = QString::fromStdWString(
        L"SELECT r.requestID AS \'ID запроса\', r.startDate AS \'Дата начала\', r.completionDate AS \'Дата конца\', r.problemDescription AS \'Описание\', "
        L"dm.modelName AS \'Модель\', rs.statusName AS \'Статус\', u.fio AS \'Клиент\', um.fio AS \'Мастер\' "
        L"FROM Requests r JOIN DeviceModels dm ON r.modelID = dm.modelID JOIN RequestStatuses rs ON r.statusID = rs.statusID "
        L"JOIN Users u ON r.clientID = u.userID JOIN Users um ON r.masterID = um.userID WHERE r.clientID = :clientId " );
    
    if ( status != ERequestStatus::ALL )
        queryStr += "AND r.statusID = :statusID";
    
    queryStr += ';';
    query.prepare( queryStr );
    query.bindValue( ":clientId", userData->_id );
    if ( status != ERequestStatus::ALL )
        query.bindValue( ":statusID", ( int ) status );

    return query;
}

bool DatabaseProcessing::CDatabaseManager::AddExtension( int requestID )
{
    QSqlQuery query( _db );

    query.prepare( "SELECT COUNT(*) FROM Extensions WHERE requestID = :requestID" );
    query.bindValue( ":requestID", requestID );

    if ( !query.exec() || !query.next() )
        return false;

    if ( query.value( 0 ).toInt() > 0 )
        return false;

    QDate currentDate = QDate::currentDate();
    QDate newCompletionDate = currentDate.addDays( 7 );

    query.prepare( "INSERT INTO Extensions (requestID, newCompletion) VALUES (:requestID, :newCompletion)" );
    query.bindValue( ":requestID", requestID );
    query.bindValue( ":newCompletion", newCompletionDate );
    if ( !query.exec() )
        return false;

    return true;
}

bool DatabaseProcessing::CDatabaseManager::RemoveExtension( bool isAccepted, int extID )
{
    QSqlQuery query( _db );
    if ( isAccepted == true )
    {
        query.prepare( QString::fromStdWString( L"UPDATE Requests SET completionDate = "
            L"( SELECT newCompletion FROM Extensions WHERE extensionID = :id ) "
            L"WHERE requestID = ( SELECT requestID FROM Extensions WHERE extensionID = :id );" ) );
        query.bindValue( ":id", extID );
        if ( query.exec() == false )
            return false;
    }

    query.prepare( QString::fromStdWString( L"DELETE FROM Extensions WHERE extensionID = :id;" ) );
    query.bindValue( ":id", extID );
    if (query.exec() == false )
        return false;

    return true;
}

QSqlQuery DatabaseProcessing::CDatabaseManager::GetUserExtensions( int userID )
{
    QSqlQuery query( _db );
    query.prepare( QString::fromStdWString( L"SELECT e.extensionID, r.problemDescription AS 'Описание', e.newCompletion AS 'Новая дата окончания' "
        L"FROM Extensions e JOIN Requests r ON e.requestID = r.requestID WHERE r.clientID = :id;" ) );
    query.bindValue( ":id", userID );

    return query;
}

bool DatabaseProcessing::CDatabaseManager::EditRequestStatus( int requestID, ERequestStatus status )
{
    if ( status == ERequestStatus::ALL )
        return false;

    QSqlQuery query( _db );
    int statusID = static_cast<int>( status );

    query.prepare( "UPDATE Requests SET statusID = :statusID WHERE requestID = :requestID" );
    query.bindValue( ":statusID", statusID );
    query.bindValue( ":requestID", requestID );

    if ( !query.exec() )
        return false;

    return true;
}

DatabaseProcessing::SDeviceModel * DatabaseProcessing::CDatabaseManager::GetDeviceModelData( int modelID ) const
{
    QSqlQuery query( _db );
    query.prepare( "SELECT modelID, modelName, typeID FROM DeviceModels WHERE modelID = :modelID" );
    query.bindValue( ":modelID", modelID );

    if ( !query.exec() )
        return nullptr;

    if ( !query.next() )
        return nullptr;

    SDeviceModel * deviceModel = new SDeviceModel;
    deviceModel->_id = query.value( 0 ).toUInt(); 
    deviceModel->_name = query.value( 1 ).toString();  
    deviceModel->_typeID = query.value( 2 ).toUInt();  
    return deviceModel;
}

DatabaseProcessing::SDeviceType * DatabaseProcessing::CDatabaseManager::GetDeviceTypeData( int typeID ) const
{
    QSqlQuery query( _db );
    query.prepare( "SELECT typeID, typeName FROM DeviceTypes WHERE typeID = :typeID" );
    query.bindValue( ":typeID", typeID );

    if ( !query.exec() )
        return nullptr;

    if ( !query.next() )
        return nullptr;

    SDeviceType * deviceType = new SDeviceType;
    deviceType->_id = query.value( 0 ).toUInt();
    deviceType->_name = query.value( 1 ).toString();
    return deviceType;
}

DatabaseProcessing::SRequestData * DatabaseProcessing::CDatabaseManager::GetRequestData( int requestID ) const
{
    QSqlQuery query( _db );

    query.prepare( "SELECT requestID, startDate, completionDate, problemDescription, masterID, clientID, modelID, statusID FROM Requests WHERE requestID = :requestID" );
    query.bindValue( ":requestID", requestID );

    if ( !query.exec() )
        return nullptr;

    if ( !query.next() )
        return nullptr;

    SRequestData * requestData = new SRequestData;
    requestData->_id = query.value( 0 ).toUInt();
    requestData->_startDate = query.value( 1 ).toDate();
    requestData->_finishDate = query.value( 2 ).toDate(); 
    requestData->_descr = query.value( 3 ).toString(); 
    requestData->_masterID = query.value( 4 ).toUInt(); 
    requestData->_clientID = query.value( 5 ).toUInt();
    requestData->_modelID = query.value( 6 ).toUInt();
    requestData->_status = ( ERequestStatus ) query.value( 7 ).toUInt();

    return requestData;
}

bool DatabaseProcessing::CDatabaseManager::AddMasterComment( SRequestData * requestData )
{
    if ( !requestData )
        return false;

    QSqlQuery query( _db );

    if ( !Transaction( query ) )
        return false;

    query.prepare( "UPDATE Requests SET completionDate = :completionDate WHERE requestID = :requestID" );
    query.bindValue( ":completionDate", QDate::currentDate() );
    query.bindValue( ":requestID", requestData->_id );

    if ( !query.exec() )
    {
        Rollback( query );
        return false;
    }

    query.prepare( "INSERT INTO Comments (message, masterID, requestID) VALUES (:message, :masterID, :requestID)" );
    query.bindValue( ":message", requestData->_descr );
    query.bindValue( ":masterID", requestData->_masterID ); 
    query.bindValue( ":requestID", requestData->_id ); 

    if ( !query.exec() )
    {
        Rollback( query );
        return false;
    }

    if ( !Commit( query ) )
    {
        Rollback( query );
        return false;
    }

    return true;
}

QList<QString> DatabaseProcessing::CDatabaseManager::GetMasterNames() const
{
    QList<QString> masterNames;
    QSqlQuery query( _db );
    query.prepare( "SELECT fio FROM Users WHERE userStatusID = 2" );
    if ( query.exec() )
    {
        while ( query.next() )
            masterNames.append( query.value( 0 ).toString() );
    }

    return masterNames;
}

QList<QString> DatabaseProcessing::CDatabaseManager::GetAllDeviceTypes() const
{
    QList<QString> deviceTypes;
    QSqlQuery query( _db );
    query.prepare( "SELECT * FROM DeviceTypes" );
    if ( query.exec() )
    {
        while ( query.next() )
            deviceTypes.append( query.value( 1 ).toString() );
    }

    return deviceTypes;
}

QList<QString> DatabaseProcessing::CDatabaseManager::GetAllModels( int typeID ) const
{
    QList<QString> deviceModels;
    QSqlQuery query( _db );

    query.prepare( "SELECT modelID, modelName FROM DeviceModels WHERE typeID = :typeID" );
    query.bindValue( ":typeID", typeID );
    if ( query.exec() )
    {
        while ( query.next() )
            deviceModels.append( query.value( 1 ).toString() );
    }

    return deviceModels;
}

QList<QString> DatabaseProcessing::CDatabaseManager::GetDeviceModelsByTypeName( const QString & typeName ) const
{
    QList<QString> deviceModels;
    QSqlQuery query( _db );

    query.prepare( "SELECT typeID FROM DeviceTypes WHERE typeName = :typeName" );
    query.bindValue( ":typeName", typeName );

    int typeID = -1;
    if ( query.exec() && query.next() )
        typeID = query.value( 0 ).toInt();
    else
        return deviceModels;

    query.prepare( "SELECT modelID, modelName FROM DeviceModels WHERE typeID = :typeID" );
    query.bindValue( ":typeID", typeID );
    if ( query.exec() )
    {
        while ( query.next() )
            deviceModels.append( query.value( 1 ).toString() );
    }

    return deviceModels;
}

bool DatabaseProcessing::CDatabaseManager::AddRequest( unsigned modelID, const QString & problemDescription, unsigned clientID, const QDate & finishDate )
{
    unsigned masterID = GetMasterWithFewestOrders();
    if ( masterID == 0 )
        return false;

    QSqlQuery query( _db );
    query.prepare( "INSERT INTO Requests (startDate, problemDescription, completionDate, masterID, clientID, modelID, statusID) "
        "VALUES (:startDate, :problemDescription, :completionDate, :masterID, :clientID, :modelID, :statusID)" );

    query.bindValue( ":startDate", QDate::currentDate() );
    query.bindValue( ":problemDescription", problemDescription );
    query.bindValue( ":completionDate", finishDate );
    query.bindValue( ":masterID", masterID );
    query.bindValue( ":clientID", clientID );
    query.bindValue( ":modelID", modelID );
    query.bindValue( ":statusID", 1 );

    if ( !query.exec() )
        return false;

    return true;
}

unsigned DatabaseProcessing::CDatabaseManager::GetMasterWithFewestOrders() const
{
    QSqlQuery query( _db );
    query.prepare( "SELECT masterID FROM Requests GROUP BY masterID ORDER BY COUNT(*) ASC LIMIT 1" );

    if ( query.exec() && query.next() )
        return query.value( 0 ).toUInt();

    return 0;
}

unsigned DatabaseProcessing::CDatabaseManager::GetDeviceModelIDByName( const QString & modelName ) const
{
    QSqlQuery query( _db );
    query.prepare( "SELECT modelID, modelName, typeID FROM DeviceModels WHERE modelName = :modelName" );
    query.bindValue( ":modelName", modelName );

    if ( query.exec() && query.next() )
        return query.value( "modelID" ).toUInt(); 

    return 0;
}

unsigned DatabaseProcessing::CDatabaseManager::GetMasterIDByName( const QString & masterName ) const
{
    QSqlQuery query( _db );
    query.prepare( "SELECT userID, login FROM Users WHERE fio = :masterName AND userStatusID = 2" );
    query.bindValue( ":masterName", masterName );
    if ( query.exec() && query.next() )
        return query.value( "userID" ).toUInt();

    return 0;
}

bool DatabaseProcessing::CDatabaseManager::UpdateRequest( int requestID, int newModelID, int newMasterID, const QString & newComment, const QDate & newCompletionDate )
{
    QSqlQuery query( _db );
    query.prepare( "UPDATE Requests SET modelID = :modelID, masterID = :masterID, completionDate = :completionDate, problemDescription = :descr "
        "WHERE requestID = :requestID" );

    query.bindValue( ":modelID", newModelID );
    query.bindValue( ":masterID", newMasterID );
    query.bindValue( ":completionDate", newCompletionDate );
    query.bindValue( ":descr", newComment );
    query.bindValue( ":requestID", requestID );

    if ( !query.exec() )
        return false;

    return true;
}

bool DatabaseProcessing::CDatabaseManager::AddMaster( SUserData * userData )
{
    QSqlQuery query( _db );

    query.prepare(
        "INSERT INTO Users (fio, phone, login, password, userStatusID, photo) "
        "VALUES (:fio, :phone, :login, :password, :userStatusID, :photo)"
    );

    query.bindValue( ":fio", userData->_fullName );
    query.bindValue( ":phone", userData->_phone );
    query.bindValue( ":login", userData->_login );
    query.bindValue( ":password", userData->_password );
    query.bindValue( ":userStatusID", 2 );
    query.bindValue( ":photo", userData->_photo );

    if ( !query.exec() )
        return false;

    return true;
}