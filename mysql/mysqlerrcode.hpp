
#ifndef MYSQLERRCODE_HPP
#define MYSQLERRCODE_HPP

#ifdef __cplusplus
extern "C" {
#endif

enum class MYSQL_ERR_CODE : int
{
  //Error: 2000 (CR_UNKNOWN_ERROR)
  //Message: Unknown MySQL error
  MY_MYSQL_UNKNOWN_ERROR = 2000,

  //Error: 2001 (CR_SOCKET_CREATE_ERROR)
  //Message: Can't create UNIX socket (%d)
  MY_MYSQL_SOCKET_CREATE_ERROR,

  //Error: 2002 (CR_CONNECTION_ERROR)
  //Message: Can't connect to local MySQL server through socket '%s' (%d)
  MY_MYSQL_CONNECTION_ERROR,

  //Error: 2003 (CR_CONN_HOST_ERROR)
  //Message: Can't connect to MySQL server on '%s' (%d)
  MY_MYSQL_CONN_HOST_ERROR,

  //Error: 2004 (CR_IPSOCK_ERROR)
  //Message: Can't create TCP/IP socket (%d)
  MY_MYSQL_IPSOCK_ERROR,

  //Error: 2005 (CR_UNKNOWN_HOST)
  //Message: Unknown MySQL server host '%s' (%d)
  MY_MYSQL_UNKNOWN_HOST,

  //Error: 2006 (CR_SERVER_GONE_ERROR)
  //Message: MySQL server has gone away
  MY_MYSQL_SERVER_GONE_ERROR,

  //Error: 2007 (CR_VERSION_ERROR)
  //Message: Protocol mismatch; server version = %d, client version = %d
  MY_MYSQL_VERSION_ERROR,

  //Error: 2008 (CR_OUT_OF_MEMORY)
  //Message: MySQL client ran out of memory
  MY_MYSQL_OUT_OF_MEMORY,

  //Error: 2009 (CR_WRONG_HOST_INFO)
  //Message: Wrong host info
  MY_MYSQL_WRONG_HOST_INFO,

  //Error: 2010 (CR_LOCALHOST_CONNECTION)
  //Message: Localhost via UNIX socket
  MY_MYSQL_LOCALHOST_CONNECTION,

  //Error: 2011 (CR_TCP_CONNECTION)
  //Message: %s via TCP/IP
  MY_MYSQL_TCP_CONNECTION,

  //Error: 2012 (CR_SERVER_HANDSHAKE_ERR)
  //Message: Error in server handshake
  MY_MYSQL_SERVER_HANDSHAKE_ERR,

  //Error: 2013 (CR_SERVER_LOST)
  //Message: Lost connection to MySQL server during query
  MY_MYSQL_SERVER_LOST,

  //Error: 2014 (CR_COMMANDS_OUT_OF_SYNC)
  //Message: Commands out of sync; you can't run this command now
  MY_MYSQL_COMMANDS_OUT_OF_SYNC,

  //Error: 2015 (CR_NAMEDPIPE_CONNECTION)
  //Message: Named pipe: %s
  MY_MYSQL_NAMEDPIPE_CONNECTION,

  //Error: 2016 (CR_NAMEDPIPEWAIT_ERROR)
  //Message: Can't wait for named pipe to host: %s pipe: %s (%lu)
  MY_MYSQL_NAMEDPIPEWAIT_ERROR,

  //Error: 2017 (CR_NAMEDPIPEOPEN_ERROR)
  //Message: Can't open named pipe to host: %s pipe: %s (%lu)
  MY_MYSQL_NAMEDPIPEOPEN_ERROR,

  //Error: 2018 (CR_NAMEDPIPESETSTATE_ERROR)
  //Message: Can't set state of named pipe to host: %s pipe: %s (%lu)
  MY_MYSQL_NAMEDPIPESETSTATE_ERROR,

  //Error: 2019 (CR_CANT_READ_CHARSET)
  //Message: Can't initialize character set %s (path: %s)
  MY_MYSQL_CANT_READ_CHARSET,

  //Error: 2020 (CR_NET_PACKET_TOO_LARGE)
  //Message: Got packet bigger than 'max_allowed_packet' bytes
  MY_MYSQL_NET_PACKET_TOO_LARGE,

  //Error: 2021 (CR_EMBEDDED_CONNECTION)
  //Message: Embedded server
  MY_MYSQL_EMBEDDED_CONNECTION,

  //Error: 2022 (CR_PROBE_SLAVE_STATUS)
  //Message: Error on SHOW SLAVE STATUS:
  MY_MYSQL_PROBE_SLAVE_STATUS,

  //Error: 2023 (CR_PROBE_SLAVE_HOSTS)
  //Message: Error on SHOW SLAVE HOSTS:
  MY_MYSQL_PROBE_SLAVE_HOSTS,

  //Error: 2024 (CR_PROBE_SLAVE_CONNECT)
  //Message: Error connecting to slave:
  MY_MYSQL_PROBE_SLAVE_CONNECT,

  //Error: 2025 (CR_PROBE_MASTER_CONNECT)
  //Message: Error connecting to master:
  MY_MYSQL_PROBE_MASTER_CONNECT,

  //Error: 2026 (CR_SSL_CONNECTION_ERROR)
  //Message: SSL connection error: %s
  MY_MYSQL_SSL_CONNECTION_ERROR,

  //Error: 2027 (CR_MALFORMED_PACKET)
  //Message: Malformed packet
  MY_MYSQL_MALFORMED_PACKET,

  //Error: 2028 (CR_WRONG_LICENSE)
  //Message: This client library is licensed only for use with MySQL servers having '%s' license
  MY_MYSQL_WRONG_LICENSE,

  //Error: 2029 (CR_NULL_POINTER)
  //Message: Invalid use of null pointer
  MY_MYSQL_NULL_POINTER,

  //Error: 2030 (CR_NO_PREPARE_STMT)
  //Message: Statement not prepared
  MY_MYSQL_NO_PREPARE_STMT,

  //Error: 2031 (CR_PARAMS_NOT_BOUND)
  //Message: No data supplied for parameters in prepared statement
  MY_MYSQL_PARAMS_NOT_BOUND,

  //Error: 2032 (CR_DATA_TRUNCATED)
  //Message: Data truncated
  MY_MYSQL_DATA_TRUNCATED,

  //Error: 2033 (CR_NO_PARAMETERS_EXISTS)
  //Message: No parameters exist in the statement
  MY_MYSQL_NO_PARAMETERS_EXISTS,

  //Error: 2034 (CR_INVALID_PARAMETER_NO)
  //Message: Invalid parameter number
  MY_MYSQL_INVALID_PARAMETER_NO,

  //Error: 2035 (CR_INVALID_BUFFER_USE)
  //Message: Can't send long data for non-string/non-binary data types (parameter: %d)
  MY_MYSQL_INVALID_BUFFER_USE,

  //Error: 2036 (CR_UNSUPPORTED_PARAM_TYPE)
  //Message: Using unsupported buffer type: %d (parameter: %d)
  MY_MYSQL_UNSUPPORTED_PARAM_TYPE,

  //Error: 2037 (CR_SHARED_MEMORY_CONNECTION)
  //Message: Shared memory: %s
  MY_MYSQL_SHARED_MEMORY_CONNECTION,

  //Error: 2038 (CR_SHARED_MEMORY_CONNECT_REQUEST_ERROR)
  //Message: Can't open shared memory; client could not create request event (%lu)
  MY_MYSQL_SHARED_MEMORY_CONNECT_REQUEST_ERROR,

  //Error: 2039 (CR_SHARED_MEMORY_CONNECT_ANSWER_ERROR)
  //Message: Can't open shared memory; no answer event received from server (%lu)
  MY_MYSQL_SHARED_MEMORY_CONNECT_ANSWER_ERROR,

  //Error: 2040 (CR_SHARED_MEMORY_CONNECT_FILE_MAP_ERROR)
  //Message: Can't open shared memory; server could not allocate file mapping (%lu)
  MY_MYSQL_SHARED_MEMORY_CONNECT_FILE_MAP_ERROR,

  //Error: 2041 (CR_SHARED_MEMORY_CONNECT_MAP_ERROR)
  //Message: Can't open shared memory; server could not get pointer to file mapping (%lu)
  MY_MYSQL_SHARED_MEMORY_CONNECT_MAP_ERROR,

  //Error: 2042 (CR_SHARED_MEMORY_FILE_MAP_ERROR)
  //Message: Can't open shared memory; client could not allocate file mapping (%lu)
  MY_MYSQL_SHARED_MEMORY_FILE_MAP_ERROR,

  //Error: 2043 (CR_SHARED_MEMORY_MAP_ERROR)
  //Message: Can't open shared memory; client could not get pointer to file mapping (%lu)
  MY_MYSQL_SHARED_MEMORY_MAP_ERROR,

  //Error: 2044 (CR_SHARED_MEMORY_EVENT_ERROR)
  //Message: Can't open shared memory; client could not create %s event (%lu)
  MY_MYSQL_SHARED_MEMORY_EVENT_ERROR,

  //Error: 2045 (CR_SHARED_MEMORY_CONNECT_ABANDONED_ERROR)
  //Message: Can't open shared memory; no answer from server (%lu)
  MY_MYSQL_SHARED_MEMORY_CONNECT_ABANDONED_ERROR,

  //Error: 2046 (CR_SHARED_MEMORY_CONNECT_SET_ERROR)
  //Message: Can't open shared memory; cannot send request event to server (%lu)
  MY_MYSQL_SHARED_MEMORY_CONNECT_SET_ERROR,

  //Error: 2047 (CR_CONN_UNKNOW_PROTOCOL)
  //Message: Wrong or unknown protocol
  MY_MYSQL_CONN_UNKNOW_PROTOCOL,

  //Error: 2048 (CR_INVALID_CONN_HANDLE)
  //Message: Invalid connection handle
  MY_MYSQL_INVALID_CONN_HANDLE,

  //Error: 2049 (CR_SECURE_AUTH)
  //Message: Connection using old (pre-4.1.1) authentication protocol refused (client option 'secure_auth' enabled)
  MY_MYSQL_SECURE_AUTH,

  //Error: 2049 (CR_UNUSED_1)
  //Message: Connection using old (pre-4.1.1) authentication protocol refused (client option 'secure_auth' enabled)
  MY_MYSQL_UNUSED_1,

  //Error: 2050 (CR_FETCH_CANCELED)
  //Message: Row retrieval was canceled by mysql_stmt_close() call
  MY_MYSQL_FETCH_CANCELED,

  //Error: 2051 (CR_NO_DATA)
  //Message: Attempt to read column without prior row fetch
  MY_MYSQL_NO_DATA,

  //Error: 2052 (CR_NO_STMT_METADATA)
  //Message: Prepared statement contains no metadata
  MY_MYSQL_NO_STMT_METADATA,

  //Error: 2053 (CR_NO_RESULT_SET)
  //Message: Attempt to read a row while there is no result set associated with the statement
  MY_MYSQL_NO_RESULT_SET,

  //Error: 2054 (CR_NOT_IMPLEMENTED)
  //Message: This feature is not implemented yet
  MY_MYSQL_NOT_IMPLEMENTED,

  //Error: 2055 (CR_SERVER_LOST_EXTENDED)
  //Message: Lost connection to MySQL server at '%s', system error: %d
  MY_MYSQL_SERVER_LOST_EXTENDED,

  //Error: 2056 (CR_STMT_CLOSED)
  //Message: Statement closed indirectly because of a preceding %s() call
  MY_MYSQL_STMT_CLOSED,

  //Error: 2057 (CR_NEW_STMT_METADATA)
  //Message: The number of columns in the result set differs from the number of bound buffers. You must reset the statement, rebind the result set columns, and execute the statement again
  MY_MYSQL_NEW_STMT_METADATA,

  //Error: 2058 (CR_ALREADY_CONNECTED)
  //Message: This handle is already connected. Use a separate handle for each connection.
  MY_MYSQL_ALREADY_CONNECTED,

  //Error: 2059 (CR_AUTH_PLUGIN_CANNOT_LOAD)
  //Message: Authentication plugin '%s' cannot be loaded: %s
  MY_MYSQL_AUTH_PLUGIN_CANNOT_LOAD,

  //Error: 2060 (CR_DUPLICATE_CONNECTION_ATTR)
  //Message: There is an attribute with the same name already
  MY_MYSQL_DUPLICATE_CONNECTION_ATTR,

  //Error: 2061 (CR_AUTH_PLUGIN_ERR)
  //Message: Authentication plugin '%s' reported error: %s
  MY_MYSQL_AUTH_PLUGIN_ERR,

  //Error: 2062 (CR_INSECURE_API_ERR)
  //Message: Insecure API function call: '%s' Use instead: '%s'
  MY_MYSQL_INSECURE_API_ERR

};


#ifdef __cplusplus
}
#endif

#endif

