
#ifndef MYRESULTCODE_HPP
#define MYRESULTCODE_HPP

#ifdef __cplusplus
extern "C" {
#endif

class CMy_BSDR_Exception
{
public:
  CMy_BSDR_Exception(const std::string & info, int g_rescode)
    : m_info{info}, m_global_rescode{g_rescode} {};
  virtual ~CMy_BSDR_Exception() {};

public:
  virtual std::string
  getInfo(void)
  {
    return m_info;
  }

  virtual int
  getResultCode(void)
  {
    return m_global_rescode;
  }

  virtual void
  printToConsole(void)
  {
  }

private:
  std::string m_info;
  int m_global_rescode;
};

//ok
const static int __GLOBAL_RESULT_OK = 1;

//algo
//no exception here

//input message
const static int __GLOBAL_INPUT_MESSAGE_PARSE_ERR = 19001;

//curl
const static int __GLOBAL_RESULT_CURL_INIT_ERR = 10411;
const static int __GLOBAL_RESULT_CURL_INST_ERR = 10412;
const static int __GLOBAL_RESULT_CURL_POST_ERR = 10413;
const static int __GLOBAL_RESULT_CURL_PERFORM_ERR = 10414;
const static int __GLOBAL_RESULT_CURL_RESPONSE_ERR = 10415;

//market groupid
const static int __GLOBAL_NO_MARKET_ERR = 10500;

const static int __GLOBAL_BLOCKLABEL_ERR = 10501;
const static int __GLOBAL_NOT_RIGHT_CHAINDATA_ERR = 10502;
const static int __GLOBAL_MARKET_ERR = 10503;

const static int __GLOBAL_CONFIG_ERR = 10600;
const static int __GLOBAL_SENDFXSERVER_ERR = 10601;
const static int __GLOBAL_NOT_ALLOC_ERR = 10602;



const static int __GLOBAL_USERDATA_BROKEN = 10700;


const static int __GLOBAL_EXIST_NO_CHAIN_RECORD_ERR = 10701;



//json
const static int __GLOBAL_RESULT_JSON_STRING_IS_NOT_NUMBER = 17001;
const static int __GLOBAL_RESULT_JSON_STRING_IS_NOT_BEGINEND = 17002;
const static int __GLOBAL_RESULT_JSON_CHAR_OUT_OF_HEAD = 17003;
const static int __GLOBAL_RESULT_JSON_CHAR_OUT_OF_TAIL = 17004;
const static int __GLOBAL_RESULT_JSON_CHAR_OUT_OF_HEADTAIL = 17005;

const static int __GLOBAL_RESULT_JSON_FUNCTION_PRECONDITION_EMPTY = 17100;
const static int __GLOBAL_RESULT_JSON_JSONCPP_READER_PARSE_ERR = 17101;
const static int __GLOBAL_RESULT_JSON_JSONCPP_READ_VALUE_EMPTY = 17102;

//mysql
const static int __GLOBAL_MY_MYSQL_UNKNOWN_ERROR = 12000;
const static int __GLOBAL_MY_MYSQL_SOCKET_CREATE_ERROR = 12001;
const static int __GLOBAL_MY_MYSQL_CONNECTION_ERROR = 12002;
const static int __GLOBAL_MY_MYSQL_CONN_HOST_ERROR = 12003;
const static int __GLOBAL_MY_MYSQL_IPSOCK_ERROR = 12004;
const static int __GLOBAL_MY_MYSQL_UNKNOWN_HOST = 12005;
const static int __GLOBAL_MY_MYSQL_SERVER_GONE_ERROR = 12006;
const static int __GLOBAL_MY_MYSQL_VERSION_ERROR = 12007;
const static int __GLOBAL_MY_MYSQL_OUT_OF_MEMORY = 12008;
const static int __GLOBAL_MY_MYSQL_WRONG_HOST_INFO = 12009;
const static int __GLOBAL_MY_MYSQL_LOCALHOST_CONNECTION = 12010;
const static int __GLOBAL_MY_MYSQL_TCP_CONNECTION = 12011;
const static int __GLOBAL_MY_MYSQL_SERVER_HANDSHAKE_ERR = 12012;
const static int __GLOBAL_MY_MYSQL_SERVER_LOST = 12013;
const static int __GLOBAL_MY_MYSQL_COMMANDS_OUT_OF_SYNC = 12014;
const static int __GLOBAL_MY_MYSQL_NAMEDPIPE_CONNECTION = 12015;
const static int __GLOBAL_MY_MYSQL_NAMEDPIPEWAIT_ERROR = 12016;
const static int __GLOBAL_MY_MYSQL_NAMEDPIPEOPEN_ERROR = 12017;
const static int __GLOBAL_MY_MYSQL_NAMEDPIPESETSTATE_ERROR = 12018;
const static int __GLOBAL_MY_MYSQL_CANT_READ_CHARSET = 12019;
const static int __GLOBAL_MY_MYSQL_NET_PACKET_TOO_LARGE = 12020;
const static int __GLOBAL_MY_MYSQL_EMBEDDED_CONNECTION = 12021;
const static int __GLOBAL_MY_MYSQL_PROBE_SLAVE_STATUS = 12022;
const static int __GLOBAL_MY_MYSQL_PROBE_SLAVE_HOSTS = 12023;
const static int __GLOBAL_MY_MYSQL_PROBE_SLAVE_CONNECT = 12024;
const static int __GLOBAL_MY_MYSQL_PROBE_MASTER_CONNECT = 12025;
const static int __GLOBAL_MY_MYSQL_SSL_CONNECTION_ERROR = 12026;
const static int __GLOBAL_MY_MYSQL_MALFORMED_PACKET = 12027;
const static int __GLOBAL_MY_MYSQL_WRONG_LICENSE = 12028;
const static int __GLOBAL_MY_MYSQL_NULL_POINTER = 12029;
const static int __GLOBAL_MY_MYSQL_NO_PREPARE_STMT = 12030;
const static int __GLOBAL_MY_MYSQL_PARAMS_NOT_BOUND = 12031;
const static int __GLOBAL_MY_MYSQL_DATA_TRUNCATED = 12032;
const static int __GLOBAL_MY_MYSQL_NO_PARAMETERS_EXISTS = 12033;
const static int __GLOBAL_MY_MYSQL_INVALID_PARAMETER_NO = 12034;
const static int __GLOBAL_MY_MYSQL_INVALID_BUFFER_USE = 12035;
const static int __GLOBAL_MY_MYSQL_UNSUPPORTED_PARAM_TYPE = 12036;
const static int __GLOBAL_MY_MYSQL_SHARED_MEMORY_CONNECTION = 12037;
const static int __GLOBAL_MY_MYSQL_SHARED_MEMORY_CONNECT_REQUEST_ERROR = 12038;
const static int __GLOBAL_MY_MYSQL_SHARED_MEMORY_CONNECT_ANSWER_ERROR = 12039;
const static int __GLOBAL_MY_MYSQL_SHARED_MEMORY_CONNECT_FILE_MAP_ERROR = 12040;
const static int __GLOBAL_MY_MYSQL_SHARED_MEMORY_CONNECT_MAP_ERROR = 12041;
const static int __GLOBAL_MY_MYSQL_SHARED_MEMORY_FILE_MAP_ERROR = 12042;
const static int __GLOBAL_MY_MYSQL_SHARED_MEMORY_MAP_ERROR = 12043;
const static int __GLOBAL_MY_MYSQL_SHARED_MEMORY_EVENT_ERROR = 12044;
const static int __GLOBAL_MY_MYSQL_SHARED_MEMORY_CONNECT_ABANDONED_ERROR = 12045;
const static int __GLOBAL_MY_MYSQL_SHARED_MEMORY_CONNECT_SET_ERROR = 12046;
const static int __GLOBAL_MY_MYSQL_CONN_UNKNOW_PROTOCOL = 12047;
const static int __GLOBAL_MY_MYSQL_INVALID_CONN_HANDLE = 12048;
const static int __GLOBAL_MY_MYSQL_SECURE_AUTH = 12049;
const static int __GLOBAL_MY_MYSQL_UNUSED_1 = 12049;
const static int __GLOBAL_MY_MYSQL_FETCH_CANCELED = 12050;
const static int __GLOBAL_MY_MYSQL_NO_DATA = 12051;
const static int __GLOBAL_MY_MYSQL_NO_STMT_METADATA = 12052;
const static int __GLOBAL_MY_MYSQL_NO_RESULT_SET = 12053;
const static int __GLOBAL_MY_MYSQL_NOT_IMPLEMENTED = 12054;
const static int __GLOBAL_MY_MYSQL_SERVER_LOST_EXTENDED = 12055;
const static int __GLOBAL_MY_MYSQL_STMT_CLOSED = 12056;
const static int __GLOBAL_MY_MYSQL_NEW_STMT_METADATA = 12057;
const static int __GLOBAL_MY_MYSQL_ALREADY_CONNECTED = 12058;
const static int __GLOBAL_MY_MYSQL_AUTH_PLUGIN_CANNOT_LOAD = 12059;
const static int __GLOBAL_MY_MYSQL_DUPLICATE_CONNECTION_ATTR = 12060;
const static int __GLOBAL_MY_MYSQL_AUTH_PLUGIN_ERR = 12061;
const static int __GLOBAL_MY_MYSQL_INSECURE_API_ERR = 12062;

const static int __GLOBAL_DB_EXIST_ITEM_ERR = 11062;

//ssl
//no exception here

//s3
const static int __GLOBAL_RESULT_S3_INVALID_ARGUMENT = 7201;
const static int __GLOBAL_RESULT_S3_REQUEST_FAILED = 7202;
const static int __GLOBAL_RESULT_S3_RESPONSE_FAILED = 7203;
const static int __GLOBAL_RESULT_S3_XML_PARSE_FAILURE = 7204;
const static int __GLOBAL_RESULT_S3_FILE_ABTAIN_FAILED = 7205;
const static int __GLOBAL_RESULT_S3_OUT_OF_MEMORY = 7206;
const static int __GLOBAL_RESULT_S3_WRITE_JSON_FAILED = 7207;
const static int __GLOBAL_RESULT_S3_FILE_PUT = 7208;
const static int __GLOBAL_RESULT_S3_FILE_UPDATE = 7209;
const static int __GLOBAL_RESULT_S3_FILE_DELETE = 7210;
//xml
//no exception here

//jwt
const static int __GLOBAL_RESULT_JWT_USER_ALREADY_REG = 9101;
const static int __GLOBAL_RESULT_JWT_USER_PUBKEY_FMT_ERR = 9102;
const static int __GLOBAL_RESULT_JWT_USER_NOT_EXIST = 9201;
const static int __GLOBAL_RESULT_JWT_USER_SIG_FAIL = 9301;
const static int __GLOBAL_RESULT_JWT_USER_CHALLENGE_NUMBER_ERR = 9302;
const static int __GLOBAL_RESULT_JWT_USER_AUTH_VERIFY_FAIL = 9401;
const static int __GLOBAL_RESULT_JWT_USER_AUTH_EXPIRED = 9402;
const static int __GLOBAL_RESULT_JWT_USER_AUTH_NO_PERMISSIONS = 9403;


#ifdef __cplusplus
}
#endif

#endif
