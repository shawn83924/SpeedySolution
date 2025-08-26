#pragma once

/* return code */
#define CG_RTN_SUCCESS						0		/* success */

#define CG_RTN_ERROR						5001	/* general error */
#define CG_RTN_MEMALLOC_ERROR				5002	/* Memory Allocation Error */
#define CG_RTN_BUFFER_TOO_SMALL				5003	/* Buffer too small */
#define CG_RTN_FUNCTION_UNSUPPORT			5004	/* function not support */
#define CG_RTN_INVALID_PARAM				5005	/* Invalid parameter */
#define CG_RTN_INVALID_HANDLE				5006	/* Invalid handle */
#define CG_RTN_LIB_EXPIRE					5007	/* TrialVersion Library is expired! */
#define CG_RTN_BASE64_ERROR					5008	/* Base64 Encoding/Decoding Error */

#define CG_RTN_CERT_NOT_FOUND				5010    /* certificate not found in MS CryptoAPI Database */
#define CG_RTN_CERT_EXPIRED					5011	/* Certicate Expire */
#define CG_RTN_CERT_NOT_YET_VALID			5012	/* Certificate can not be used now */
#define CG_RTN_CERT_EXPIRE_OR_NOT_YET_USE	5013	/* Some certificates are expired, some can not be used now!!! */
#define CG_RTN_CERT_DENIED					5014	/* Certificate subject not match */
#define CG_RTN_CERT_NOISSUER				5015	/* Unable to find certificate issuer */
#define CG_RTN_CERT_BAD_SIGNATURE			5016	/* Certificate signature is invalid */
#define CG_RTN_CERT_INVALID_KEYUSAGE		5017	/* Invalid certificate keyusage */

#define CG_RTN_CERT_REVOKED					5020	/* Certificate is revoked */
#define CG_RTN_CERT_UNSPECIFIED				5020	/* Certificate is revoked */
#define CG_RTN_CERT_KEY_COMPROMISED			5021	/* Certificate is revoked(key compromised) */
#define CG_RTN_CERT_CA_COMPROMISED			5022	/* Certificate is revoked(CA compromised) */
#define CG_RTN_CERT_AFFILIATION_CHANGED		5023	/* Certificate is revoked(affiliation changed) */
#define CG_RTN_CERT_SUPERSEDED				5024	/* Certificate is revoked(superseded) */
#define CG_RTN_CERT_CESSATION				5025	/* Certificate is revoked(cessation) */
#define CG_RTN_CERT_HOLD					5026	/* Certificate is revoked(hold) */
#define CG_RTN_CERT_REMOVEFROMCRL			5028	/* Certificate is revoked(hold) */

#define CG_RTN_CRL_EXPIRED					5030	/* CRL expired */
#define CG_RTN_CRL_NOT_YET_VALID			5031	/* CRL not yet valid */
#define CG_RTN_CRL_NOT_FOUND				5032	/* CRL not found */
#define CG_RTN_CRL_BAD_SIGNATURE			5034	/* CRL signature invalid */

#define CG_RTN_GET_DIGEST_ERROR				5035	/* Get Digest error */
#define CG_RTN_BAD_SIGNATURE				5036	/* Invalid data signature */
#define CG_RTN_BAD_CONTENT					5037	/* Content not match */

#define CG_RTN_BAD_CAPTCHA					5038

#define CG_RTN_INVALID_CERT					5040	/* Incorrect Certificate format */
#define CG_RTN_INVALID_CRL					5041	/* Incorrect CRL format */
#define CG_RTN_INVALID_PKCS7				5042	/* Incorrect PKCS7 format */
#define CG_RTN_INVALID_KEY					5043	/* Incorrect KEY format */
#define CG_RTN_INVALID_CERTREQ				5044	/* Incorrect PKCS10 format */
#define CG_RTN_INVALID_FORMAT				5045	/* Incorrect format */
#define	CG_RTN_INVALID_PKCS12				5046

#define CG_RTN_OBJ_NOT_FOUND				5050
#define CG_RTN_PKCS7_NO_CONTENT				5051
#define CG_RTN_PKCS7_NO_CERTIFICATE			5052
#define CG_RTN_PKCS7_NO_SIGNERINFO			5053

#define CG_RTN_UNMATCH_CERT_KEY				5060	/* Certificate/PrivateKey not match */

#define CG_RTN_SIGN_ERROR					5061	/* Sign error */
#define CG_RTN_VERIFY_ERROR					5062	/* Verify error */
#define CG_RTN_ENCRYPT_ERROR				5063	/* Encrypt error */
#define CG_RTN_DECRYPT_ERROR				5064	/* Decrypt error */
#define CG_RTN_GENKEY_ERROR					5065	/* Generate key error */
#define CG_RTN_DELETE_USR_CERT_ERROR		5066
#define CG_RTN_BAD_ALGID					5067

#define CG_RTN_OPERATION_CANCELED			5070	/* Operation Cancel */
#define CG_RTN_PASSWD_INVALID				5071
#define CG_RTN_SCARD_BLOCKED				5072
#define CG_RTN_REPLUG_TIMEOUT				5073
#define CG_RTN_DEVICE_DISABLED				5074

#define CG_RTN_XMLPARSE_ERROR				5080
#define CG_RTN_XMLTAG_NOTFOUND				5081

/* CGCAPI return code */
#define CGCAPI_RTN_OPENSTORE_ERROR			5201
#define	CGCAPI_RTN_CREATECHAIN_ERROR		5202
#define CGCAPI_RTN_CRYPTCONTEXT_ERROR		5203
#define CGCAPI_RTN_NO_PRIVATE_KEY			5204
#define CGCAPI_RTN_UNEXPORTABLE				5205
#define CGCAPI_RTN_STORE_ACCESSDENY			5206

#define CG_RTN_UNICODE_ERROR				5901	/* Unicode Error */

#define CG_RTN_FILE_NOT_FOUND				5902	/* File Not Found*/
#define CG_RTN_PATH_NOT_FOUND				5903	/* Path Not Found*/
#define CG_RTN_BAD_NETPATH					5904	/* Network path was not found*/
#define CG_RTN_LOGON_FAILURE				5905	/* Unknown logon user name or bad password*/
#define CG_RTN_ACCESS_DENIED				5906	/* Access Denied, no right */
#define CG_RTN_BAD_INITCODE					5907
#define CG_RTN_INITCODE_NOT_VERIFY			5908
#define CG_RTN_BAD_PASSWD					5909

#define CG_FLAG_VERIFY_CRL					0x00000002

/* for verify flag */
#define CG_FLAG_VERIFY_CONTENT_ONLY			0x00000000
#define CG_FLAG_VERIFY_CERTCHAIN			0x00000001
#define CG_FLAG_VERIFY_FULL					(CG_FLAG_VERIFY_CERTCHAIN | CG_FLAG_VERIFY_CRL)

/* common flag */
#define CG_FLAG_BASE64_ENCODE				0x00001000
#define CG_FLAG_BASE64_DECODE				0x00002000
#define CG_FLAG_DETACHMSG					0x00004000
#define CG_FLAG_ENVELOPING					0x00008000
#define CG_FLAG_ENVELOPED					0x00010000
#define CG_FLAG_MANIFEST					0x00020000
#define CG_FLAG_DATA_APPEND_NULL			0x00040000
#define CG_FLAG_1_5_COMPATABLE				CG_FLAG_DATA_APPEND_NULL | CG_FLAG_DETACHMSG
#define CG_FLAG_SUBJECT_PARTIALMATCH		0x00080000
/**********************************/
#define CG_FLAG_DOUNICODE					0x00100000
#define CG_FLAG_HEX_ENCODE					0x01000000
#define CG_FLAG_VERIFY_DISABLED				0x02000000
/**********************************/

#define CG_FLAG_SUBJECT_COMMON_NAME					0x00010000
#define CG_FLAG_SUBJECT_RSA_EMAILADDR				0x00020000
#define CG_FLAG_SUBJECT_ORGANIZATION_NAME			0x00040000
#define CG_FLAG_SUBJECT_ORGANIZATIONAL_UNIT_NAME	0x00080000
#define CG_FLAG_SUBJECT_COUNTRY_NAME				0x00100000
#define CG_FLAG_SUBJECT_DEVICE_SERIAL_NUMBER		0x00200000

/* enroll flag */
#define CG_FLAG_DELETE_REQUEST_CERT			0x00000001
#define CG_FLAG_WRITE_CERT_TO_CSP			0x00000002

#define CG_TYPE_CERTIFICATE					1
#define CG_TYPE_CERTREQUEST					2
#define CG_TYPE_CRL							3
#define CG_TYPE_PKCS7						4
#define CG_TYPE_SIGNERINFO					5
#define CG_TYPE_PRIVATEKEY					6
#define CG_TYPE_PUBLICKEY					7
#define CG_TYPE_STORE						8
#define CG_TYPE_HASH						9
#define CG_TYPE_SYMKEY						10
#define CG_TYPE_PKCS12						11
#define CG_TYPE_PKCS8						12
#define CG_TYPE_PKCS8INFO					13
#define CG_TYPE_HMAC						14

#define CG_ALGOR_DES						0x01
#define CG_ALGOR_3DES						0x02
#define CG_ALGOR_IDEA						0x03
#define CG_ALGOR_RC4						0x04
#define CG_ALGOR_AES_128					0x05
#define CG_ALGOR_AES_192					0x06
#define CG_ALGOR_AES_256					0x07

#define CG_ALGOR_MD5						0x01
#define CG_ALGOR_SHA1						0x02
#define CG_ALGOR_SHA224						0x03
#define CG_ALGOR_SHA256						0x04
#define CG_ALGOR_SHA384						0x05
#define CG_ALGOR_SHA512						0x06

#define CG_FLAG_MODE_ECB					0x10
#define CG_FLAG_MODE_CBC					0x20

/* pure sign/verify flag */
#define CG_FLAG_DOHASH						0x00010000
#define	CG_FLAG_NOHASHOID					0x00020000

/* key generation flag */
#define CG_FLAG_PKCS5_PBKDF1				0x100
#define CG_FLAG_PKCS5_PBKDF2				0x200


#define CG_KU_DIGITAL_SIGNATURE		0x0080
#define CG_KU_NON_REPUDIATION		0x0040
#define CG_KU_KEY_ENCIPHERMENT		0x0020
#define CG_KU_DATA_ENCIPHERMENT		0x0010
#define CG_KU_KEY_AGREEMENT			0x0008
#define CG_KU_KEY_CERT_SIGN			0x0004
#define CG_KU_CRL_SIGN				0x0002
#define CG_KU_ENCIPHER_ONLY			0x0001
#define CG_KU_DECIPHER_ONLY			0x8000

#define CG_XKU_SSL_SERVER_AUTH		0x1
#define CG_XKU_SSL_CLIENT_AUTH		0x2
#define CG_XKU_EMAIL_PROTECTION		0x4
#define CG_XKU_CODE_SIGN			0x8
#define CG_XKU_MS_SGC				0x10
#define CG_XKU_NS_SGC				0x20

/* sign flag */
#define CGCAPI_FLAG_SELCERT_MANUAL				0x00000001
#define CGCAPI_FLAG_SELCERT_AUTO				0x00000002
#define CGCAPI_FLAG_SELCERT_SELFAUTO			(CGCAPI_FLAG_SELCERT_AUTO|CGCAPI_FLAG_SELCERT_MANUAL)
#define CGCAPI_FLAG_SELCERT_AFTER				0x00000004
#define CGCAPI_FLAG_SELCERT_OLDEST				0x00000008
#define CGCAPI_FLAG_SELCERT_CHECKVALID			0x00000010
#define CGCAPI_FLAG_DISABLE_KEYSPEC_ATSIGNATURE	0x00000020
#define CGCAPI_FLAG_CHECK_PASSWD				0x00000040
#define CGCAPI_FLAG_ACCESS_PUBLICONLY			0x00000080
#define CG_FLAG_CERT_NOATTACH					0x00000100
#define CG_FLAG_CERT_ATTACHALL					0x00000200
#define CGCAPI_FLAG_PASSWD_KEYBOARD				0x00000400
#define CGCAPI_FLAG_PASSWD_KEYPAD				0x00000800
#define CGCAPI_FLAG_ENABLE_EMPTY_PASSWORD		0x00001000
#define CGCAPI_FLAG_SELCERT_USEIE				0x00002000
#define CGCAPI_FLAG_USE_CAPTCHA					0x00008000
#define CGCAPI_FLAG_USE_REPLUG					0x00010000
#define CGCAPI_FLAG_USE_REPLUG2					0x00020000
#define CGCAPI_FLAG_CAPTCHA_RANDOM_LOCATION		0x00100000
#define CGCAPI_FLAG_READERTYPE_CHECK			0x00200000
#define CGCAPI_FLAG_NO_REPLUG_DELAY				0x00200000

#define CGCAPI_FLAG_DISABLE_REPLUG				0x00800000
#define CGCAPI_FLAG_DOUBLE_CHECK_REPLUG			0x04000000

#define CG_FLAG_USE_FILE					0x00000020
#define CG_FLAG_READFILE_DELETE				0x00100000

/* verify flag */
#define CGCAPI_FLAG_FETCH_URLOBJ			0x00000010
#define CGCAPI_FLAG_USING_CERTCHAIN			0x00000020
#define CGCAPI_FLAG_LOCALMACHINE			0x00000040

/* import p12 flag */
#define CGCAPI_FLAG_EXPORTABLE				0x00000001
#define CGCAPI_FLAG_PROTECTED				0x00000002
#define CGCAPI_FLAG_KEYSET_LOCALMACHINE		0x00000004
#define CGCAPI_FLAG_KEYSET_USER				0x00000008
#define CGCAPI_FLAG_PLAINPFXPASS			0x00000010
#define CGCAPI_FLAG_MEMORY					0x00400000

/* iStoreFlags for CGXCAPI_LoadStore */
#define CGCAPI_STORE_WRITABLE			0x00000080

#define CGCAPI_STORE_SYS				0x00000001
#define CGCAPI_STORE_LM					0x00000010
#define CGCAPI_STORE_CU					0x00000100

#define CGCAPI_STORE_SYS_ROOT			0x00000001
#define CGCAPI_STORE_SYS_CA				0x00000002
#define CGCAPI_STORE_SYS_MY				0x00000004
#define CGCAPI_STORE_SYS_OTHER			0x00000008
#define CGCAPI_STORE_LM_ROOT			0x00000010
#define CGCAPI_STORE_LM_CA				0x00000020
#define CGCAPI_STORE_LM_MY				0x00000040
#define CGCAPI_STORE_LM_OTHER			0x00000080
#define CGCAPI_STORE_CU_ROOT			0x00000100
#define CGCAPI_STORE_CU_CA				0x00000200
#define CGCAPI_STORE_CU_MY				0x00000400
#define CGCAPI_STORE_CU_OTHER			0x00000800

#define CGCAPI_STORE_CU_REQUEST			0x00100000
#define CGCAPI_STORE_CU_FILE			0x00400000

#define CGCAPI_STORE_ROOT				0x00000001
#define CGCAPI_STORE_CA					0x00000002
#define CGCAPI_STORE_MY					0x00000004
#define CGCAPI_STORE_OTHER				0x00000008
#define CGCAPI_STORE_ROOTS				0x00000111
#define CGCAPI_STORE_CAS				0x00000222
#define CGCAPI_STORE_MYS				0x00000444
#define CGCAPI_STORE_OTHERS				0x00000888

/* x509 Cert Extersion Cloumn */
#define CG_TYPE_X509EXT_CRLDP				1
#define	CG_TYPE_X509EXT_CERTPOLICIES		2
#define	CG_TYPE_X509EXT_AIA					3
#define	CG_TYPE_X509EXT_AKI					4
#define	CG_TYPE_X509EXT_SKI					5
#define	CG_TYPE_X509EXT_BASICCONTRAINTS		6
#define	CG_TYPE_X509EXT_KEYUSAGE			7
#define	CG_TYPE_X509EXT_SUBJECTALTNAME		8

/* object handle type define*/
typedef void* CGHandle;
typedef CGHandle CGStoreHandle;
typedef CGHandle CGCertHandle;
typedef CGHandle CGKeyHandle;
typedef CGHandle CGPKCS7Handle;
typedef CGHandle CGSignerInfoHandle;
typedef CGHandle CGHashHandle;
typedef CGHandle CGPKCS10Handle;
typedef CGHandle CGCRLHandle;
typedef CGHandle CGHMACHandle;

struct ICGCDLLAPI
{
	virtual void AddRef() = 0;
	virtual void Release() = 0;
	virtual int LoadStore(CGStoreHandle* pStore, int iStoreFlags, int iFlags) = 0;
	virtual int GetCertificates(CGStoreHandle	hStore,
		                        const char* strSubject,
		                        const char* strIssuerSubkect,
		                        int	iFlags,
		                        int	iKeyUsage,
		                        CGCertHandle** phCerts,
		                        int* piCerts) = 0;
	virtual int SignEx(const void* pbData, 
		               unsigned long lData,
		               const char* strSubject,
		               const char* strIssuerSubject, 
		               const char* strProvider,
		               const char* strPin,
		               int iFlags,
		               int iHashFlag,
		               int iKeyUsage,
		               char** pstrSignature) = 0;
	virtual int PKCS7GetUserCertificate(char* strP7Cert, char** strX509Cert) = 0;
	virtual int CertGetSubject(const char* strCert, int iFlags, char** pstrSubject) = 0;
	virtual int CertGetNotBefore(const char* strCert, int iFlags, char* strNotBefore) = 0;
	virtual int CertGetNotAfter(const char* strCert, int iFlags, char* strNotAfter) = 0;
	virtual int CertGetSerialNumber(const char* strCert, int iFlags, char** pstrSerialNumber) = 0;
	virtual void MemFree(void* pMem) = 0;
	virtual void FreeHandle(CGHandle hHandle) = 0;
};