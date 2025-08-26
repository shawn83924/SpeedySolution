#pragma once

//Sign Flag
const int FS_FLAG_CERT_NOATTACH = 0x00000100;
const int FS_FLAG_CERT_ATTACHALL = 0x00000200;

//Verify Flag
const int FS_FLAG_VERIFY_CONTENT_ONLY = 0x00000000;
const int FS_FLAG_VERIFY_CERTCHAIN = 0x00000001;
const int FS_FLAG_VERIFY_CRL = 0x00000002;
const int FS_FLAG_VERIFY_FULL = FS_FLAG_VERIFY_CERTCHAIN | FS_FLAG_VERIFY_CRL;

//Common Flag
const int FS_FLAG_BASE64_ENCODE = 0x00001000;
const int FS_FLAG_BASE64_DECODE = 0x00002000;
const int FS_FLAG_DETACHMSG = 0x00004000;
const int FS_FLAG_ENVELOPING = 0x00008000;
const int FS_FLAG_ENVELOPED = 0x00010000;
const int FS_FLAG_MANIFEST = 0x00020000;

const int FS_TYPE_CERTIFICATE = 1;
const int FS_TYPE_CERTREQUEST = 2;
const int FS_TYPE_CRL = 3;
const int FS_TYPE_PKCS7 = 4;
const int FS_TYPE_SIGNERINFO = 5;
const int FS_TYPE_PRIVATEKEY = 6;
const int FS_TYPE_PUBLICKEY = 7;
const int FS_TYPE_STORE = 8;
const int FS_TYPE_HASH = 9;
const int FS_TYPE_SYMKEY = 10;

const int FS_ALGOR_DES = 0x01;
const int FS_ALGOR_3DES = 0x02;
const int FS_ALGOR_IDEA = 0x03;
const int FS_ALGOR_RC4 = 0x04;

const int FS_ALGOR_MD5 = 0x01;
const int FS_ALGOR_SHA1 = 0x02;

const int FS_FLAG_MODE_ECB = 0x10;
const int FS_FLAG_MODE_CBC = 0x20;

const int FS_FLAG_SUBJECT_COMMON_NAME = 0x00010000;
const int FS_FLAG_SUBJECT_RSA_EMAILADDR = 0x00020000;
const int FS_FLAG_SUBJECT_ORGANIZATION_NAME = 0x00040000;
const int FS_FLAG_SUBJECT_ORGANIZATIONAL_UNIT_NAME = 0x00080000;
const int FS_FLAG_SUBJECT_COUNTRY_NAME = 0x00100000;
const int FS_FLAG_SUBJECT_DEVICE_SERIAL_NUMBER = 0x00200000;

//Pure Sign/Verify Flag
const int FS_FLAG_DOHASH = 0x00010000;
const int FS_FLAG_NOHASHOID = 0x00020000;

//Key Generation Flag
const int FS_FLAG_PBKDF1 = 1;
const int FS_FLAG_PBKDF2 = 2;

const int FS_KU_DIGITAL_SIGNATURE = 0x0080;
const int FS_KU_NON_REPUDIATION = 0x0040;
const int FS_KU_KEY_ENCIPHERMENT = 0x0020;
const int FS_KU_DATA_ENCIPHERMENT = 0x0010;
const int FS_KU_KEY_AGREEMENT = 0x0008;
const int FS_KU_KEY_CERT_SIGN = 0x0004;
const int FS_KU_CRL_SIGN = 0x0002;
const int FS_KU_ENCIPHER_ONLY = 0x0001;
const int FS_KU_DECIPHER_ONLY = 0x8000;

const int FS_XKU_SSL_SERVER_AUTH = 0x1;
const int FS_XKU_SSL_CLIENT_AUTH = 0x2;
const int FS_XKU_EMAIL_PROTECTION = 0x4;
const int FS_XKU_CODE_SIGN = 0x8;
const int FS_XKU_MS_SGC = 0x10;
const int FS_XKU_NS_SGC = 0x20;

//Sign Flag
const int FSCAPI_FLAG_SELCERT_MANUAL = 0x00000001;
const int FSCAPI_FLAG_SELCERT_AUTO = 0x00000002;
const int FSCAPI_FLAG_SELCERT_SELFAUTO = FSCAPI_FLAG_SELCERT_AUTO | FSCAPI_FLAG_SELCERT_MANUAL;
const int FSCAPI_FLAG_SELCERT_AFTER = 0x00000004;
const int FSCAPI_FLAG_SELCERT_OLDEST = 0x00000008;
const int FSCAPI_FLAG_SELCERT_CHECKVALID = 0x00000010;
const int FSCAPI_FLAG_DISABLE_KEYSPEC_ATSIGNATURE = 0x00000020;
const int FSCAPI_FLAG_ENABLE_EMPTY_PASSWORD = 0x00000100;
const int FSCAPI_FLAG_SELCERT_USEIE = 0x00000200;
const int FSCAPI_FLAG_SUBJECT_PARTIALMATCH = 0x00080000;
const int FSCAPI_FLAG_SELCERT_MEMORY = 0x00400000;

//Verify Flag
const int FSCAPI_FLAG_FETCH_URLOBJ = 0x00000010;
const int FSCAPI_FLAG_USING_CERTCHAIN = 0x00000020;
const int FSCAPI_FLAG_LOCALMACHINE = 0x00000040;

//Import P12 Flag
const int FSCAPI_FLAG_EXPORTABLE = 0x00000001;
const int FSCAPI_FLAG_PROTECTED = 0x00000002;
const int FSCAPI_FLAG_KEYSET_LOCALMACHINE = 0x00000004;
const int FSCAPI_FLAG_KEYSET_USER = 0x00000008;
const int FSCAPI_STORE_WRITABLE = 0x00000080;
const int FSCAPI_STORE_SYS = 0x00000001;
const int FSCAPI_STORE_LM = 0x00000010;
const int FSCAPI_STORE_CU = 0x00000100;

const int FSCAPI_STORE_SYS_ROOT = 0x00000001;
const int FSCAPI_STORE_SYS_CA = 0x00000002;
const int FSCAPI_STORE_SYS_MY = 0x00000004;
const int FSCAPI_STORE_LM_ROOT = 0x00000010;
const int FSCAPI_STORE_LM_CA = 0x00000020;
const int FSCAPI_STORE_LM_MY = 0x00000040;
const int FSCAPI_STORE_CU_ROOT = 0x00000100;
const int FSCAPI_STORE_CU_CA = 0x00000200;
const int FSCAPI_STORE_CU_MY = 0x00000400;
const int FSCAPI_STORE_CU_FILE = 0x00400000;

const int FSCAPI_STORE_ROOT = 0x00000001;
const int FSCAPI_STORE_CA = 0x00000002;
const int FSCAPI_STORE_MY = 0x00000004;
const int FSCAPI_STORE_OTHER = 0x00000008;
const int FSCAPI_STORE_ROOTS = 0x00000111;
const int FSCAPI_STORE_CAS = 0x00000222;
const int FSCAPI_STORE_MYS = 0x00000444;
const int FSCAPI_STORE_OTHERS = 0x00000888;

//X509 Cert Extersion Flag
const int FS_TYPE_X509EXT_CRLDP = 1;
const int FS_TYPE_X509EXT_CERTPOLICIES = 2;
const int FS_TYPE_X509EXT_AIA = 3;
const int FS_TYPE_X509EXT_AKI = 4;
const int FS_TYPE_X509EXT_SKI = 5;
const int FS_TYPE_X509EXT_BASICCONTRAINTS = 6;
const int FS_TYPE_X509EXT_KEYUSAGE = 7;

//FSCAPI Error Code
const int FS_RTN_SUCCESS = 0; 
const int FS_RTN_ERROR = 5001; 
const int FS_RTN_MEMALLOC_ERROR = 5002; 
const int FS_RTN_BUFFER_TOO_SMALL = 5003; 
const int FS_RTN_FUNCTION_UNSUPPORT = 5004; 
const int FS_RTN_INVALID_PARAM = 5005; 
const int FS_RTN_INVALID_HANDLE = 5006; 
const int FS_RTN_LIB_EXPIRE = 5007; 
const int FS_RTN_BASE64_ERROR = 5008; 
const int FS_RTN_CERT_NOT_FOUND = 5010; 
const int FS_RTN_CERT_EXPIRED = 5011; 
const int FS_RTN_CERT_NOT_YET_VALID = 5012; 
const int FS_RTN_CERT_EXPIRE_OR_NOT_YET_USE = 5013; 
const int FS_RTN_CERT_DENIED = 5014; 
const int FS_RTN_CERT_NOISSUER = 5015; 
const int FS_RTN_CERT_BAD_SIGNATURE = 5016; 
const int FS_RTN_CERT_INVALID_KEYUSAGE = 5017; 
const int FS_RTN_CERT_REVOKED = 5020; 
const int FS_RTN_CERT_KEY_COMPROMISED = 5021; 
const int FS_RTN_CERT_CA_COMPROMISED = 5022; 
const int FS_RTN_CERT_AFFILIATION_CHANGED = 5023; 
const int FS_RTN_CERT_SUPERSEDED = 5024; 

const int FS_RTN_CERT_CESSATION = 5025; 
const int FS_RTN_CERT_HOLD = 5026; 
const int FS_RTN_CERT_REMOVEFROMCRL = 5028; 
const int FS_RTN_CRL_EXPIRED = 5030; 
const int FS_RTN_CRL_NOT_YET_VALID = 5031; 
const int FS_RTN_CRL_NOT_FOUND = 5032; 
const int FS_RTN_CRL_BAD_SIGNATURE = 5034; 
const int FS_RTN_GET_DIGEST_ERROR = 5035; 
const int FS_RTN_BAD_SIGNATURE = 5036; 
const int FS_RTN_BAD_CONTENT = 5037; 
const int FS_RTN_INCORRECT_CAPTCHA = 5038; 
const int FS_RTN_INVALID_CERT = 5040; 
const int FS_RTN_INVALID_CRL = 5041; 
const int FS_RTN_INVALID_PKCS7 = 5042; 
const int FS_RTN_INVALID_KEY = 5043; 
const int FS_RTN_INVALID_CERTREQ = 5044; 
const int FS_RTN_INVALID_FORMAT = 5045; 
const int FS_RTN_INVALID_PKCS12 = 5046; 
const int FS_RTN_OBJ_NOT_FOUND = 5050; 
const int FS_RTN_PKCS7_NO_CONTENT = 5051; 
const int FS_RTN_PKCS7_NO_CERTIFICATE = 5052; 
const int FS_RTN_PKCS7_NO_SIGNERINFO = 5053; 
const int FS_RTN_UNMATCH_CERT_KEY = 5060; 
const int FS_RTN_SIGN_ERROR = 5061; 
const int FS_RTN_VERIFY_ERROR = 5062; 
const int FS_RTN_ENCRYPT_ERROR = 5063; 
const int FS_RTN_DECRYPT_ERROR = 5064; 
const int FS_RTN_GENKEY_ERROR = 5065; 
const int FS_RTN_DELETE_USR_CERT_ERROR = 5066; 
const int FS_RTN_OPERATION_CANCELED = 5070; 
const int FS_RTN_PASSWD_INVALID = 5071; 
const int FSXML_RTN_PARSE_ERROR = 5080; 
const int FSXML_RTN_TAG_NOTFOUND = 5081; 
const int FSCAPI_RTN_OPENSTORE_ERROR = 5201; 
const int FSCAPI_RTN_CREATECHAIN_ERROR = 5202; 
const int FSCAPI_RTN_CRYPTCONTEXT_ERROR = 5203; 
const int FSCAPI_RTN_NO_PRIVATE_KEY = 5204; 
const int FSCAPI_RTN_UNEXPORTABLE = 5205; 
const int FSCAPI_RTN_STORE_ACCESSDENY = 5206; 
const int FS_RTN_UNICODE_ERROR = 5901; 
const int FS_RTN_FILE_NOT_FOUND = 5902; // File Not Found
const int FS_RTN_PATH_NOT_FOUND = 5903; // Path Not Found
const int FS_RTN_BAD_NETPATH = 5904; // Network path was not found
const int FS_RTN_LOGON_FAILURE = 5905; // Unknown logon user name or bad password
const int FS_RTN_ACCESS_DENIED = 5906; // Access Denied, no right
const int FS_RTN_OBJECT_NOT_EXIST = 9100;
const int FS_RTN_OBJECT_EXIST = 9101;
const int FS_RTN_OBJECT_HAS_PROBLEM = 9102;
const int FS_RTN_LIBRARY_NOT_LOAD0 = 9110;
const int FS_RTN_LIBRARY_NOT_LOAD1 = 9111;
const int FS_RTN_SLOT_NOT_FOUND = 9112;


/* 全景安控元件 FSCAPI ACTIVEX API Bridge */
struct IFSCATLAPI
{
	virtual void AddRef() = 0;
	virtual void Release() = 0;
	virtual long XGetUserCertificate(const char* provider, const char* subject, int flag, int key_usage, char** pp_cert) = 0;
	virtual long CertGetSubject(const char* cert, int flag, char** pp_subject) = 0;
	virtual long CertGetNotAfter(const char* cert, int flag, char** pp_notafter) = 0;
	virtual long CertGetNotBefore(const char* cert, int flag, char** pp_notbefore) = 0;
	virtual long CertGetSerialNumber(const char* cert, int flag, char** pp_sno) = 0;
	virtual long SignEx(const char* data, const char* subject, const char* provider, const char* pin, int flag, int key_usage, char** pp_sign) = 0;
	virtual void FreeAllocString(char* data) = 0;
	virtual const char* GetErrorMsg() = 0;
};