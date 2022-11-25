/*
 * Connection.cpp
 * 
 *  Autor: Wagner Ary Mota
 *  Setembro de 2022
 * 
 *  Gerenciamento das tratativas com o SGBD.
 *
 */

#include "MySQLConnect.h"

WiFiClient client;
MySQL_Connection _connection((Client *)&client);

MySQLConnect::MySQLConnect(IPAddress __server, char* __user, char* __pswd)
{
    // Configuracao de conexao do SGBD
    this->_serverAddress = __server;
    this->_userName = __user;
    this->_userPswd = __pswd;
}

// Realiza a conexao com o sgbd.
bool MySQLConnect::connect()
{
    if(!_connection.connected()) {
        // Conectando ao sgbd.
        return _connection.connect(_serverAddress, 3306, this->_userName, this->_userPswd);
    }

    return true;
}

// Fecha a conexao com o sgbd.
void MySQLConnect::disconnect()
{
    if(_connection.connected()) {
        _connection.close();
    }
}

// Recebe os dados do paciente.
int MySQLConnect::receiveData(int __deviceID)
{
    char __SELECT_SQL[200];
    row_values* __row = NULL;
    int __headCount = 0;

    if(this->connect()) {
        sprintf( // String da query para o select.
            __SELECT_SQL,
            "SELECT `idPaciente` FROM `unip_tcc`.`paciente` WHERE `idEquipamento` = '%d' AND `isAtivo` = '1';",
            __deviceID
        );

        // Instanciando o cursor.
        MySQL_Cursor* __sql_cursor = new MySQL_Cursor(&_connection);

        // Executando a query.
        bool __status = __sql_cursor->execute(__SELECT_SQL);

        // Fetch the columns (required) but we don't use them.
        column_names* columns = __sql_cursor->get_columns();

        // Read the row (we are only expecting the one)
        do {
            __row = __sql_cursor->get_next_row();
            
            if (__row != NULL){
                __headCount = atol(__row->values[0]);
            }
        } while(__row != NULL);

        // Deleting the cursor also frees up memory used
        delete(__sql_cursor);
        this->disconnect();

        return __headCount;
    }

    return false;
}


// Enviando os dados para o SGBD.
bool MySQLConnect::sendData(int __patientID, int __saturationO2, int __beatPerMinute, float __temperature)
{
    char __INSERT_SQL[200];

    if(this->connect()) {
        sprintf( // String da query para o insert.
            __INSERT_SQL,
            "INSERT INTO `unip_tcc`.`medicoes` (`idPaciente`, `numOxi`, `numBPM`, `numTemp`) VALUES ('%d','%d', '%d', '%f');",
            __patientID, __saturationO2, __beatPerMinute, __temperature
        );

        // Instanciando o cursor.
        MySQL_Cursor* __sql_cursor = new MySQL_Cursor(&_connection);

        // Executando a query.
        bool __status = __sql_cursor->execute(__INSERT_SQL);

        // Apaga o cursor e desconecta do sgbd.
        delete(__sql_cursor);
        this->disconnect();

        return __status;
    }

    return false;
}

// Desabilita o dispositivo no SGBD.
bool MySQLConnect::disableDevice(int __deviceID)
{
    char __UPDATE_SQL[200];

    if(this->connect()) {
        sprintf( // String da query para o update.
            __UPDATE_SQL,
            "UPDATE `unip_tcc`.`paciente` SET `isAtivo` = '0' WHERE `idEquipamento` = '%d' AND `isAtivo` = '1';",
            __deviceID
        );

        // Instanciando o cursor.
        MySQL_Cursor* __sql_cursor = new MySQL_Cursor(&_connection);

        // Executando a query.
        bool __status = __sql_cursor->execute(__UPDATE_SQL);

        // Apaga o cursor e desconecta do sgbd.
        delete(__sql_cursor);
        this->disconnect();

        return __status;
    }

    return false;
}