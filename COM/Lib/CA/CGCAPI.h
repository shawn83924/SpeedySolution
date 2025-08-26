/*! \file CGCAPI.h 
 *	\brief This package is an CAPI encrypt/decrypt/sign/verify library
 *
 */

#ifndef _CGCAPI_H_
#define _CGCAPI_H_

#ifdef __cplusplus
extern "C" {
#endif

//Laputa : for memory leak
/*#include <stdlib.h>
#ifdef _DEBUG
#include <crtdbg.h>
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define malloc(s) _malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
//#define CGCAPIMemAlloc(s) _malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
#endif*/

#include <windows.h>
#include <wincrypt.h>
#include <time.h>

#ifndef CG_CONST_DEFINE
#define CG_CONST_DEFINE

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

/* this structure is used for get blob data */
typedef struct {
	unsigned long lValue;		/* data length */
	unsigned char *pValue;		/* data value */
} CGBlob;

typedef struct _cgconfvalue {
	int num;
	struct _cgattr *attr;
} CGAttributes;

typedef struct _cgattr {
	int type;
	char *name;
	union {
		CGBlob data;
		CGAttributes attrs;
	} value;
} cgattr;

/* object handle type define*/
typedef void *	CGHandle;
typedef CGHandle CGStoreHandle;
typedef CGHandle CGCertHandle;
typedef CGHandle CGKeyHandle;
typedef CGHandle CGPKCS7Handle;
typedef CGHandle CGSignerInfoHandle;
typedef CGHandle CGHashHandle;
typedef CGHandle CGPKCS10Handle;
typedef CGHandle CGCRLHandle;
typedef CGHandle CGHMACHandle;

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

#endif	/* CG_CONST_DEFINE */

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
#define CGCAPI_FLAG_CAPTCHA_RANDOM_LOCATION		        0x00100000
#define CGCAPI_FLAG_READERTYPE_CHECK			        0x00200000
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


/*! 
 CGCAPI lib memory alloc function
 * @param[in]	lSize    required size
 * \return		memory pointer.
 * \remarks		call CGCAPIMemFree to release
 */

//Laputa
void *CGCAPIMemAlloc(unsigned long lSize);


/*! 
 CGCAPI lib memory realloc function
 * @param[in]	pMem    original memory pointer
 * @param[in]	lSize   required size
 * \return		memory pointer.
 * \remarks		call CGCAPIMemFree to release
 */

void *CGCAPIMemRealloc(void *pMem, unsigned long lSize);

/*! 
 CGCAPI lib memory free function
 * @param[in]	pMem    memory pointer to release
 * \remarks		
 */
 
void CGCAPIMemFree(void *pMem);

int CGCAPI_Encode(
	unsigned char *	in,
	unsigned long	inlen,
	unsigned char **	out,
	unsigned long	*outlen);


int CGCAPI_Decode(
	unsigned char *	in,
	unsigned long	inlen,
	unsigned char **	out,
	unsigned long	*outlen);

int CGXCAPI_HexDecode(
	char *pbHexData,
	int lHexData,
	char **pbData,
	int *lData);

int CGXCAPI_HexEncode(
	char *pbData,
	int lData,
	char **pbHexData,
	int *lHexData);

/*! 
 CGCAPI lib handle(cert, pkcs7, key, store...) free function
 * @param[in]	hHandle  object handle
 * \remarks		
 */

void CGXCAPIFreeHandle(CGHandle hHandle);



/*! 
 CGCAPI lib blob free function
 * @param[in]	pstBlob  blob pointer
 * \remarks		
 */

void CGXCAPIFreeBlob(CGBlob *pstBlob);

/*! 
 open CAPI store for search issuer cert & crl
 * @param[out]	pStore  store handle
 * @param[in]	iStoreFlags  opened store flag
 * @param[in]	iFlags  reserved
 * \return		error code.
 * \remarks		
 */

int CGXCAPI_LoadStore(CGStoreHandle *pStore, int iStoreFlags, int iFlags);

/* CGXCAPI_LoadStoreByName
 *
 * open CAPI store for search issuer cert & crl
 * input:
 *		iStoreFlags: opened store flag
 *		iFlags: reserved
 * output:
 *		pStore: store handle
 * return:
 *		return code
 * remark:
 */
int CGXCAPI_LoadStoreByName(CGStoreHandle *pStore, int iFlags, const wchar_t *strName);

/* CGXCAPI_Decode
 *
 * base64 decode
 * input:
 *		pbEncoded: base64 encoded data
 *		lEncoded: encoded data length
 *		pemtitle: PEM header(optional)
 * output:
 *		pstDecodedBlob: decoded data
 * return:
 *		return code
 * remark:
 *		call CGXCAPIFreeBlob to release pstDecodedBlob
 */
int CGXCAPI_Decode(
	const char *pbEncoded,
	unsigned long lEncoded,
	CGBlob *pstDecodedBlob,
	const char *pemtitle);

/* CGXCAPI_Encode
 *
 * base64 encode
 * input:
 *		pbData: data
 *		lData: data length
 *		pemtitle: PEM header(optional)
 * output:
 *		pstEncodedBlob: base64 encoded data
 * return:
 *		return code
 * remark:
 *		call CGXCAPIFreeBlob to release pstEncodedBlob
 */
int CGXCAPI_Encode(
	const void *	pbData,
	unsigned long	lData,
	CGBlob *		pstEncodedBlob,
	const char *	pemtitle);

int CGXCAPI_EncodeEx(
	char *str,
	int length,
	char **ret,
	int *ret_length);

/* CGXCAPI_DecodeObject
 *
 * decode data to handle
 * input:
 *		iType:	object type
 *		pbEncoded: encoded data (base64 or not, depending on iFlags)
 *		lEncoded: encoded data length
 *		iFlags: decode flag (CG_FLAG_BASE64_DECODE)
 *		pParam: optional param (NULL)
 * output:
 *		pHandle: object handle
 * return:
 *		return code
 * remark:
 *		call CGXCAPIFreeHandle to release pHandle
 */
int CGXCAPI_DecodeObject(
	CGHandle *		pHandle,
	int				iType,
	const void *	pbEncoded,
	unsigned long	lEncoded,
	int				iFlags,
	const void *	pParam);

/* CGXCAPI_EncodeObject
 *
 * encode handle to data
 * input:
 *		hHandle: object handle
 *		iType:	object type
 *		iFlags: decode flag (CG_FLAG_BASE64_ENCODE)
 *		pParam: optional param (int *: NULL or 0 for no pem header)
 * output:
 *		pstBlob: encoded blob (base64 or not, depending on iFlags)
 * return:
 *		return code
 * remark:
 *		call CGXCAPIFreeBlob to free pstBlob
 */
int CGXCAPI_EncodeObject(
	CGHandle		hHandle,
	int				iType,
	CGBlob *		pstBlob,
	int				iFlags,
	const void *	pParam);

/* CGXCAPI_GetUserCertificate
 *
 * get current user certificate
 * input:
 *		strSubject: cert subject filter
 *		strIssuerSubject: cert issuer filter
 *		iFlags: get cert flag
 *		iKeyUsage: Key Usage
 * output:
 *		phCert: certificate handle
 * return:
 *		return code
 * remark:
 *		call CGXCAPIFreeHandle to release phCert
 */
int CGXCAPI_GetUserCertificate(
	const char *	strSubject,
	const char *	strIssuerSubject,
	int				iFlags,
	int				iKeyUsage,
	CGCertHandle *	phCert);

int CGXCAPI_GetUserCertificateFromCSP(
	const char *	strProvider,
	const char *	strSubject,
	const char *	strIssuerSubject,
	int				iFlags,
	int				iKeyUsage,
	int				iDisableATSpec,
	CGCertHandle *	phCert);

/* CGXCAPI_GetCertificateByIssuer
 *
 * get certificate in stores and check issuer
 * input:
 *		hStore: store handle
 *		strIssuer: cert issuer filter
 *		strCertSerial: cert serial
 *		iFlags: get cert flag
 *		iKeyUsage: Key Usage
 * output:
 *		phCert: certificate handle
 * return:
 *		return code
 * remark:
 *		call CGXCAPIFreeHandle to release phCert
 */
int CGXCAPI_GetCertificateByIssuer(
				CGStoreHandle hStore,
				const char *strIssuer,
				const char *strCertSerial,
				int iFlags,
				int iKeyUsage,
				CGCertHandle *phCert);

/* CGXCAPI_GetCertificate
 *
 * get certificate in stores
 * input:
 *		hStore: store handle
 *		strSubject: cert subject filter
 *		iFlags: get cert flag
 *		iKeyUsage: Key Usage
 * output:
 *		phCert: certificate handle
 * return:
 *		return code
 * remark:
 *		call CGXCAPIFreeHandle to release phCert
 */

int CGXCAPI_GetCertificate(
	CGStoreHandle	hStore,
	const char *	strSubject,
	const char *	strIssuerSubject,
	int				iFlags,
	int				iKeyUsage,
	CGCertHandle *	phCert);

/* CGXCAPI_GetUserCertificate
 *
 * get current user certificate
 * input:
 *		strSubject: cert subject filter
 *		iFlags: get cert flag
 *		iKeyUsage: Key Usage
 * output:
 *		phCerts: certificates list
 *		piCerts: certificates number
 * return:
 *		return code
 * remark:
 *		call CGXCAPIFreeHandle to release phCert
 */
int CGXCAPI_GetUserCertificates(
	const char *	strSubject,
	int				iFlags,
	int				iKeyUsage,
	CGCertHandle **	phCerts,
	int *			piCerts);

int CGCAPI_GetUserCertificates(
	const char *	strProvider,
	const char *	strSubject,
	int				iFlags,
	int				iKeyUsage,
	int		iDisableATSpec,
	char ***		pCerts,
	int *			piCerts);

int CGCAPI_GetUserCertificate(
	const char *	strProvider,
	const char *	strSubject,
	int				iFlags,
	int				iKeyUsage,
	char **			pCert);

int CGXCAPI_GetCertificates(
	CGStoreHandle	hStore,
	const char *	strSubject,
	const char *strIssuerSubkect,
	int				iFlags,
	int				iKeyUsage,
	CGCertHandle **	phCerts,
	int *			piCerts);

int CGXCAPI_ValidateCertSubject(
	CGCertHandle	hCert,
	const char *strSubject,
	int				iFlags);

/* CGXCAPI_VerifyCertChain
 *
 * verify certificate chain
 * input:
 *		hCert: cert handle
 *		hStore: store handle
 *		iFlags: verify flag (CG_FLAG_VERIFY_CRL)
 *		iTime: verify time
 * output:
 * return:
 *		return code
 * remark:
 */
int CGXCAPI_VerifyCertChain(
	CGCertHandle	hCert,
	CGStoreHandle	hStore,
	int				iFlags,
	int				iTime);

/* CGXCAPI_ValidateCertificate
 *
 * validate certificate
 * input:
 *		hCert: cert handle
 *		iTime: verify time
 * output:
 * return:
 *		return code
 * remark:
 */
int CGXCAPI_ValidateCertificate(
	CGCertHandle	hCert,
	const char *	strSubject,
	int				iFlags,
	int				iTime);

/* CGXCAPI_Sign
 *
 * sign data to pkcs7 format
 * input:
 *		hCert: cert handle
 *		hStore: store handle
 *		pbData: sign data
 *		lData: sign data length
 *		iFlags: sign flag
 * output:
 *		phSignature: signature handle
 * return:
 *		return code
 * remark:
 *		call CGXCAPIFreeHandle to release phSignature
 */
int CGXCAPI_Sign(
	CGCertHandle	hCert,
	CGStoreHandle	hStore,
	const void *	pbData,
	unsigned long	lData,
	int				iFlags,
	int				iHashFlags,
	CGPKCS7Handle *	phSignature);

int CGXCAPI_SignInit(
	CGPKCS7Handle *	phPKCS7,
	CGCertHandle	hSigner,
	CGKeyHandle		hPriKey,
	int				iFlags);

int CGXCAPI_SignUpdate(
	CGPKCS7Handle	hPKCS7,
	const void *	pbData,
	unsigned long	lData,
	int iFlags);

int CGXCAPI_SignFinal(
	CGHashHandle	hPKCS7,
	int				iFlags,
	CGBlob *		pstSignBlob);

/* CGXCAPI_Verify
 *
 * verify signature in pkcs7 format
 * input:
 *		hStore: store handle
 *		hSignature: signature handle
 *		pbData: sign data
 *		lData: sign data length
 *		strSubject: valid cert subject filter
 *		iSignerIndex: signer index (0)
 *		iFlags: sign flag
 *		iTime: verify time
 * output:
 *		phSignerInfo: signer info handle (optional)
 *		phCert: signer cert handle (optional)
 * return:
 *		return code
 * remark:
 *		call CGXCAPIFreeHandle to release phSignerInfo & phCert
 */
int CGXCAPI_Verify(
	CGStoreHandle	hStore,
	CGPKCS7Handle	hSignature,
	const void *	pbData,
	unsigned long	lData,
	const char *	strSubject,
	int				iSignerIndex,
	int				iFlags,
	time_t			iTime,
	CGSignerInfoHandle *	phSignerInfo,
	CGCertHandle *	phCert);

int CGXCAPI_VerifyInit(
	CGHashHandle	*phHash,
	int				iFlags);

int CGXCAPI_VerifyUpdate(
	CGHashHandle	hHash,
	const void *	pbData,
	unsigned long	lData,
	int iFlags);

int CGXCAPI_VerifyFinal(
	CGPKCS7Handle	hPKCS7,
	CGCertHandle	hCert,
	CGHashHandle	hHash,
	int iSignerIndex,
	int iFlags);

/* CGXCAPI_ComposeSignerInfo
 *
 * compose cert and raw signature to signer info
 * input:
 *		hCert: cert handle
 *		pbRawSig: raw signature
 *		lRawSig: raw signature length
 *		iFlags: reserved
 * output:
 *		phDigest: signer info handle
 * return:
 *		return code
 * remark:
 *		call CGXCAPIFreeHandle to release phDigest
 */
int CGXCAPI_ComposeSignerInfo(
	CGCertHandle	hCert,
	const void *	pbRawSig,
	unsigned long	lRawSig,
	int iHashFlags,
	CGSignerInfoHandle *phDigest);

/* CGXCAPI_ComposeSignature
 *
 * compose cert and signer info to pkcs7 format
 * input:
 *		hCert: cert handle
 *		hDigest: signer info handle
 *		pbData: data
 *		lData: data length
 *		iFlags: reserved
 * output:
 *		phSignature: signature handle
 * return:
 *		return code
 * remark:
 *		call CGXCAPIFreeHandle to release phSignature
 */
int CGXCAPI_ComposeSignature(
	CGCertHandle	hCert,
	CGSignerInfoHandle	hDigest,
	const void *	pbData,
	unsigned long	lData,
	int				iFlags,
	CGPKCS7Handle *	phSignature);

int CGCAPI_SymmetricEncrypt(
	char *strPassword, 
	const void *pbData,
	unsigned long lData,
	const void *pbSalt,
	unsigned long lSalt,
	unsigned long lIteration,
	unsigned char *iv, 
	int iAlgor,
	int iMode,
	char **pstrCipher,
	unsigned long * lstrCipher);

int CGCAPI_SymmetricEncryptEx(
	int iAlgor,	
	const void *	pbData,
	unsigned long	lData,
	unsigned char *key, 
	int keylen, 
	unsigned char *iv, 
	int iv_size,
	int iFlags,
	char **pstrCipher,
	unsigned long * lstrCipher);

int CGCAPI_SymmetricEncryptFileEx(
	int iAlgor,	
	wchar_t *srcfilepath,
	wchar_t *dstfilepath,
	unsigned char *key, 
	int keylen, 
	unsigned char *iv, 
	int iv_size,
	int iFlags);

int CGCAPI_SymmetricDecryptFileEx(
	int iAlgor,	
	wchar_t *srcfilepath,
	wchar_t *dstfilepath,
	unsigned char *key, 
	int keylen, 
	unsigned char *iv, 
	int iv_size,
	int iFlags);

int CGCAPI_SymmetricDecrypt(
	char *strPassword, 
	const char *strCipher,
	unsigned long  lstrCipher,
	const void *pbSalt,
	unsigned long lSalt,
	unsigned long lIteration,
	unsigned char *iv, 
	int iAlgor,
	int iMode,
	char **	pbData,
	unsigned long*	lData);

int CGCAPI_SymmetricDecryptEx(
	int iAlgor,	
	const char *	strCipher,
	unsigned long  lstrCipher,
	unsigned char *key, 
	int keylen, 
	unsigned char *iv, 
	int iv_size,
	int iFlags,
	char **	pbData,
	unsigned long*	lData);

/* CGXCAPI_Encrypt
 *
 * encrypt by cert to pkcs7 format
 * input:
 *		hCerts: cert handle array
 *		iCerts: cert handle number
 *		pbData: data
 *		lData: data length
 *		iFlags: encrypt algorithm
 * output:
 *		pstCipherBlob: cipher blob (der)
 * return:
 *		return code
 * remark:
 *		call CGXCAPIFreeBlob to release pstCipherBlob
 */
int CGXCAPI_Encrypt(
	CGCertHandle *	hCerts,
	int 			iCerts,
	const void *	pbData,
	unsigned long	lData,
	int				iFlags,
	CGBlob *		pstCipherBlob);

/* CGXCAPI_Decrypt
 *
 * decrypt pkcs7 cipher
 * input:
 *		hStore: store handle
 *		pbCipher: cipher (der)
 *		lCipher: cipher length
 *		iFlags: researved
 * output:
 *		pstClearBlob: data blob
 *		phEncryptCert: encrypt cert handle
 * return:
 *		return code
 * remark:
 *		call CGXCAPIFreeBlob to release pstCipherBlob
 */
int CGXCAPI_Decrypt(
	CGStoreHandle	hStore,
	const void *	pbCipher,
	unsigned long	lCipher,
	int				iFlags,
	CGBlob *		pstClearBlob,
	CGCertHandle *	phEncryptCert);

/* CGXCAPI_SignAndEncrypt
 *
 * encrypt by cert to pkcs7 format
 * input:
 *		hSignCert: sign cert handle
 *		hEncryptCert: encrypt cert handle
 *		hStore: store handle
 *		pbData: data
 *		lData: data length
 *		iFlags: encrypt algorithm
 * output:
 *		pstSignCipherBlob: signature and cipher blob (der)
 * return:
 *		return code
 * remark:
 *		call CGXCAPIFreeBlob to release pstCipherBlob
 */
int CGXCAPI_SignAndEncrypt(
	CGCertHandle	hSignCert,
	CGCertHandle	hEncryptCert,
	CGStoreHandle	hStore,
	const void *	pbData,
	unsigned long	lData,
	int				iFlags,
	int				iHashFlags,
	CGBlob *		pstSignCipherBlob);

/* CGXCAPI_DecryptAndVerify
 *
 * decrypt and verify pkcs7 cipher
 * input:
 *		hStore: store handle
 *		pbCipher: cipher (der)
 *		lCipher: cipher length
 *		iSignerIndex: signer index
 *		iFlags: researved
 * output:
 *		pstClearBlob: data blob
 *		phSignerCert: signer info handle (optional)
 *		phEncryptCert:signer cert handle (optional)
 * return:
 *		return code
 * remark:
 *		call CGXCAPIFreeBlob to release pstCipherBlob
 */
int CGXCAPI_DecryptAndVerify(
	CGStoreHandle	hStore,
	const void *	pbCipher,
	unsigned long	lCipher,
	int				iSignerIndex,
	int				iFlags,
	CGBlob *		pstClearBlob,
	CGCertHandle *	phSignerCert,
	CGCertHandle *	phEncryptCert);

/* CGXCAPI_CreateRSAPublicKey
 *
 * create rsa public key pair
 * input:
 *		pbModule: n
 *		lModule: length of n
 *		pbExponent: e
 *		lExponent: length of e
 *		iFlags: reserved
 * output:
 *		phKey: key handle
 * return:
 * remark:
 */
int CGXCAPI_CreateRSAPublicKey(
	unsigned char *	pbModule,
	unsigned long	lModule,
	unsigned char * pbExponent,
	unsigned long	lExponent,
	int				iFlags,
	CGKeyHandle *	phKey);

/* CGXCAPI_ByteToKey
 *
 * generate key value
 * input:
 *		pass: password
 *		passlen: password length
 *		salt: salt
 *		saltlen: salt length
 *		iter: iteration count
 *		md: hash method
 * output:
 *		key: output key
 *		keylen: needed output length
 *		iv: output iv
 *		ivlen: needed iv length
 * return:
 *		return code
 * remark:
 */
int CGXCAPI_ByteToKey(
	const void *pass, int passlen,
	const void *salt, int saltlen,
	int iter,
	int iFlags,
	void *key, int keylen,
	void *iv, int ivlen);

/* CGXCAPI_GenerateKey
 *
 * generate symmetric key
 * input:
 *		strPassword: password to derive phKey
 *		pbSalt: salt used to derive phKey
 *		lSalt: pbSalt length
 *		lIteration: iteration count
 *		iFlags: encrypt algorithm & flag
 *		iAlgor: encryption algorithm
 * output:
 *		phKey: symmetric key handle
 * return:
 *		return code
 * remark:
 *		call CGXCAPIFreeHandle to release phKey
 */
int CGXCAPI_GenerateKey(
	int					iAlgor,
	const char *		strPassword,
	const void *		pbSalt,
	unsigned long		lSalt,
	unsigned long		lIteration,
	int					iFlags,
	CGKeyHandle *		phKey);

int CGXCAPI_CreateKey(
	int iAlgor,
	unsigned char *key, 
	int keylen, 
	unsigned char *iv, 
	int iv_size,
	int iFlags,
	CGKeyHandle *phKey);

/* CGXCAPI_EncryptInit
 *
 * encrypt initialize
 * input:
 *		iv: initial vector
 *		iFlags: reserved
 * output:
 * return:
 *		return code
 * remark:
 */
int CGXCAPI_EncryptInit(
	CGKeyHandle 		hKey,
	const void *		iv,
	int					iFlags);

/* CGXCAPI_EncryptUpdate
 *
 * encrypt data
 * input:
 *		hKey: symmetric key handle
 *		pbData: clear data
 *		lData: pbData length
 *		iFlags: reserved
 * output:
 *		pbCipher: cipher output
 *		plcipher: (in)pbCipher buffer length (out) pbCipher output length
 * return:
 *		return code
 * remark:
 *		call CGXCAPIFreeBlob to release pstCipherBlob
 */
int CGXCAPI_EncryptUpdate(
	CGKeyHandle 		hKey,
	const void *		pbData,
	unsigned long		lData,
	void *				pbCipher,
	unsigned long *		plCipher,
	int					iFlags);

/* CGXCAPI_EncryptFinal
 *
 * decrypt and verify pkcs7 cipher
 * input:
 *		hKey: symmetric key handle
 *		iFlags: reserved
 * output:
 *		pbCipher: cipher output
 *		plCipher: (in)pbCipher buffer length (out) pbCipher output length
 * return:
 *		return code
 * remark:
 */
int CGXCAPI_EncryptFinal(
	CGKeyHandle 		hKey,
	void *				pbCipher,
	unsigned long *		plCipher,
	int					iFlags);

/* CGXCAPI_DecryptInit
 *
 * decrypt initialize
 * input:
 *		iv: initial vector
 *		iFlags: reserved
 * output:
 * return:
 *		return code
 * remark:
 */
int CGXCAPI_DecryptInit(
	CGKeyHandle 		hKey,
	const void *		iv,
	int					iFlags);

/* CGXCAPI_DecryptUpdate
 *
 * decrypt data
 * input:
 *		hKey: symmetric key handle
 *		pbCipher: cipher data
 *		lCipher: pbCipher length
 *		iFlags: reserved
 * output:
 *		pbClear: clear output
 *		plClear: (in)pbClear buffer length (out) pbClear output length
 * return:
 *		return code
 * remark:
 */
int CGXCAPI_DecryptUpdate(
	CGKeyHandle 		hKey,
	const void *		pbCipher,
	unsigned long		lCipher,
	void *				pbClear,
	unsigned long *		plClear,
	int					iFlags);

/* CGXCAPI_DecryptFinal
 *
 * decrypt final
 * input:
 *		hKey: symmetric key handle
 *		iFlags: reserved
 * output:
 *		pbClear: cipher output
 *		plClear: (in)pbCipher buffer length (out) pbCipher output length
 * return:
 *		return code
 * remark:
 */
int CGXCAPI_DecryptFinal(
	CGKeyHandle 		hKey,
	void *				pbClear,
	unsigned long *		plClear,
	int					iFlags);

/*! 
 get private key from cert
 * @param[in]	hCert   cert handle
 * @param[out]	phKey   key handle
 * \return		error code.
 * \remarks		call CGCAPIFreeHandle to release phKey
 */
int CGXCAPI_GetPrivateKey(
	CGCertHandle	hCert,
	CGKeyHandle *	phKey);

int CGXCAPI_GetPrivateKeyWithKSP(
	CGCertHandle	hCert,
	CGKeyHandle *	phKey);

/*! 
 get provider name from cert
 * @param[in]	hCert   cert handle
 * @param[out]	ppOutProviderName	
 * \return		error code.
 * \remarks		call CGCAPIMemFree to release ppOutProviderName
 */


int CGXCAPI_CertGetProvInfo(
	CGCertHandle	hCert,
	char **ppOutProviderName,
	char **ppOutContainerName);

int CGCAPI_GetCertProvInfo(
	const char *	strSubject,
	const char *	strIssuerSubject,
	int				iFlags,
	int				iKeyUsage,
	char **ppOutProviderName,
	char **ppOutContainerName);

/*! 
 get public key from cert
 * @param[in]	hCert   cert handle
 * @param[out]	phKey   key handle
 * \return		error code.
 * \remarks		call CGCAPIFreeHandle to release phKey
 */
int CGXCAPI_CertGetPublicKey(
	CGCertHandle	hCert,
	CGKeyHandle *	phKey);

int CGXCAPI_CertGetVersion(
	CGCertHandle	hCert,
	int				iFlags,
	char *version);



/*! 
 get number of signer info in pkcs7 signature
 * @param[in]	hSignature   signature handle
 * @param[out]	piCount   number of signer
 * \return		error code.
 * \remarks		
 */
int CGXCAPI_PKCS7GetSignerInfoCount(
	CGPKCS7Handle	hSignature,
	int *			piCount);


/*! 
 get signer info in pkcs7 signature
 * @param[in]	hSignature   signature handle
 * @param[in]	iSignerIndex   signer index
 * @param[out]	phDigest   signer info
 * \return		error code.
 * \remarks		call CGXCAPIFreeHandle to release phDigest
 */
int	CGXCAPI_PKCS7GetSignerInfo(
	CGPKCS7Handle	hSignature,
	int				iSignerIndex,
	CGSignerInfoHandle *	phDigest);


/*! 
 get signer cert in pkcs7 signature
 * @param[in]	hSignature   signature handle
 * @param[in]	iSignerIndex   signer index
 * @param[out]	phCert   cert info
 * \return		error code.
 * \remarks		call CGXCAPIFreeHandle to release phCert
 */

int CGXCAPI_PKCS7GetSignerCertificate(
	CGPKCS7Handle	hSignature,
	int				iSignerIndex,
	CGCertHandle *	phCert);

/* CGXCAPI_PKCS7GetContent
 *
 * get content in pkcs7 signature
 * input:
 *		hSignature: signature handle
 * output:
 *		pstContentBlob: content blob
 * return:
 *		return code
 * remark:
 *		call CGXCAPIFreeBlob to release pstContentBlob
 */
int CGXCAPI_PKCS7GetContent(
	CGPKCS7Handle	hSignature,
	CGBlob *		pstContentBlob);

int CGXCAPI_PKCS7GetEncryptedDigest(
	CGPKCS7Handle	hSignature,
	CGBlob *		pstContentBlob);

/* CGXCAPI_PKCS7GetUserCertificate
 *
 * get x509 cert from pkcs7
 * input:
 *		hCerts: p7 handle
 * output:
 *		phCert: cert handle:
 * return:
 *		return code
 * remark:
 *		call CGXCAPIFreeHandle to release phCert
 */
int CGXCAPI_PKCS7GetUserCertificate(
	CGPKCS7Handle	hCerts,
	CGCertHandle *	phCert);

/* CGXCAPI_PKCS7GetCertChain
 *
 * get x509 cert from pkcs7
 * input:
 *		hCerts: p7 handle
 * output:
 *		phCerts: p7 handle:
 * return:
 *		return code
 * remark:
 *		call CGXCAPIFreeHandle to release phCerts
 */
int CGXCAPI_PKCS7GetCertChain(
	CGPKCS7Handle	hCerts,
	CGPKCS7Handle *	phCerts);

/* CGXCAPI_PKCS7AddSignerCertificate
 *
 * add signer cert to pkcs7 signature
 * input:
 *		hSignature: signature handle
 *		hCert: cert handle
 * output:
 * return:
 *		return code
 * remark:
 */
int CGXCAPI_PKCS7AddSignerCertificate(
	CGPKCS7Handle	hSignature,
	CGCertHandle	hCert);

/* CGXCAPI_PureSign
 *
 * pure sign
 * input:
 *		hPriKey: private key handle
 *		pbData: data
 *		lData: data length
 *		iFlags: sign flag (hash algor, do hash or not, sign raw data or with hash)
 * output:
 *		pstSignatureBlob: signature blob
 * return:
 *		return code
 * remark:
 *		call CGXCAPIFreeBlob to release pstContentBlob
 *		sign raw data is support on after win2000.
 */
int CGXCAPI_PureSign(
	CGKeyHandle		hPriKey,
	const void *	pbData,
	unsigned long	lData,
	int				iFlags,
	CGBlob *		pstSignatureBlob);

int CGCAPI_PureVerify(
	char *			pstrSignature,
	int 			lSignature,
	const void *	pbData,
	unsigned long	lData,
	const char *	strSubject,
	int				iFlags,
	int				iHashalg,
	int				iKeyUsage);

/* CGXCAPI_PureVerify
 *
 * pure verify
 * input:
 *		hPubKey: public key handle
 *		pbSignature: signature data
 *		lSignature: signature length
 *		pbData: data
 *		lData: data length
 *		iFlags: verify flag (hash algor, do hash or not, sign raw data or with hash)
 * output:
 * return:
 *		return code
 * remark:
 */
int CGXCAPI_PureVerify(
	CGKeyHandle		hPubKey,
	const void *	pbSignature,
	unsigned long	lSignature,
	const void *	pbData,
	unsigned long	lData,
	int				iFlags);

/* CGXCAPI_HashInit
 *
 * init hash handle
 * input:
 *		iFlags: hash flag (hash algor)
 * output:
 *		phHasn: hash handle
 * return:
 *		return code
 * remark:
 *		call CGXCAPIFreeHandle to release hash handle
 */
int CGXCAPI_HashInit(
	CGHashHandle *	phHash,
	int				iHashFlags);

/* CGXCAPI_HashUpdate
 *
 * init hash handle
 * input:
 *		hHash: hash handle
 *		pbData: data
 *		lData: data length
 *		iFlags: researved
 * output:
 * return:
 *		return code
 * remark:
 */
int CGXCAPI_HashUpdate(
	CGHashHandle	hHash,
	const void *	pbData,
	unsigned long	lData,
	int				iFlags);

/* CGXCAPI_HashFinal
 *
 * get hash data
 * input:
 *		hHash: hash handle
 *		iFlags: researved
 * output:
 *		pstHashBlob: hash data
 * return:
 *		return code
 * remark:
 *		call CGXCAPIFreeBlob to release pstHashBlob
 */
int CGXCAPI_HashFinal(
	CGHashHandle	hHash,
	int				iFlags,
	CGBlob *		pstHashBlob);

/* CGXCAPI_PublicEncrypt
 *
 * direct encrypt by public key
 * input:
 *		hKey: key handle
 *		pbData: data
 *		lData: data length
 *		iFlags: researved
 * output:
 *		pstCipherBlob: cipher blob
 * return:
 *		return code
 * remark:
 *		call CGXCAPIFreeBlob to release pstCipherBlob
 *		not always support, depend on the default CSP proprieties
 */
int CGXCAPI_PublicEncrypt(
	CGKeyHandle		hKey,
	const void *	pbData,
	unsigned long	lData,
	int				iFlags,
	CGBlob *		pstCipherBlob);

/* CGXCAPI_PublicDecrypt
	not support
 */
int CGXCAPI_PublicDecrypt(
	CGKeyHandle		hKey,
	const void *	pbCipher,
	unsigned long	lCipher,
	int				iFlags,
	CGBlob *		pstClearBlob);

/* CGXCAPI_PrivateEncrypt
	not support
 */
int CGXCAPI_PrivateEncrypt(
	CGKeyHandle		hKey,
	const void *	pbData,
	unsigned long	lData,
	int				iFlags,
	CGBlob *		pstCipherBlob);

/* CGXCAPI_PrivateDecrypt
 *
 * direct decrypt by private key
 * input:
 *		hKey: key handle
 *		pbCipher: cipher data
 *		lCipher: cipher data length
 *		iFlags: researved
 * output:
 *		pstClearBlob: clear blob
 * return:
 *		return code
 * remark:
 *		call CGXCAPIFreeBlob to release pstClearBlob
 *		not always support, depend on the default CSP proprieties
 */
int CGXCAPI_PrivateDecrypt(
	CGKeyHandle		hKey,
	const void *	pbCipher,
	unsigned long	lCipher,
	int				iFlags,
	CGBlob *		pstClearBlob);

/* CGXCAPI_ExportPKCS12
 *
 * export cert and private key to pkcs12 format
 * input:
 *		hSubject: cert handle
 *		hStore: store handle
 *		iFlags: export flags
 *		strPassword: password to encrypt private key
 * output:
 *		pstP12Blob: pkcs12
 * return:
 *		return code
 * remark:
 *		call CGXCAPIFreeBlob to release pstP12Blob
 */
int CGXCAPI_ExportPKCS12(
	CGCertHandle	hCert,
	CGStoreHandle	hStore,
	int				iFlags,
	const char *	strPassword,
	CGBlob *		pstP12Blob);

/* CGXCAPI_ImportPKCS12
 *
 * import pkcs12 format cert and private key
 * input:
 *		hStore: import cert to hStore
 *		pbP12: pkcs12 content
 *		lP12: pbPKCS12 length
 *		strPassword: password to encrypt private key
 *		iFlags: import flag
 * output:
 *		pstP12Blob: pkcs12
 * return:
 *		return code
 * remark:
 *		call CGXCAPIFreeBlob to release pstP12Blob
 */
int CGXCAPI_ImportPKCS12(
	CGStoreHandle hStore,
	unsigned char *	pbP12,
	unsigned long	lP12,
	const char *	strPassword,
	const wchar_t *	wstrFriendlyName,
	int				iFlags);

/* CGXCAPI_CertGetSubject
 *
 * get cert subject
 * input:
 *		hCert: cert handle
 * output:
 *		pstrSubject: cert subject
 * return:
 *		return code
 * remark:
 *		call CGCAPIMemFree to release pstrSubject
 */
int CGXCAPI_CertGetSubject(
	CGCertHandle	hCert,
	int				iFlags,
	char **			pstrSubject);

int CGXCAPI_CertGetSubjectW(
							CGCertHandle	hCert,
							int				iFlags,
							wchar_t **		pstrSubject);

int CGXCAPI_CertGetSubjectCNU(
	CGCertHandle	hCert,
	int				iFlags,
	wchar_t **			pstrSubject);

/* CGXCAPI_CertGetIssuer
 *
 * get cert issuer subject
 * input:
 *		hCert: cert handle
 * output:
 *		pstrIssuer: cert issuer name
 * return:
 *		return code
 * remark:
 *		call CGCAPIMemFree to release pstrSubject
 */
int CGXCAPI_CertGetIssuer(
	CGCertHandle	hCert,
	int				iFlags,
	char **			pstrIssuer);

int CGXCAPI_CertGetIssuerW(
						   CGCertHandle	hCert,
						   int				iFlags,
						   wchar_t **		pstrIssuer);

int CGXCAPI_CertGetIssuerCNU(
	CGCertHandle	hCert,
	int				iFlags,
	wchar_t **			pstrIssuer);

/* CGXCAPI_CertGetNotBefore
 *
 * get cert notbefore date
 * input:
 *		hCert: cert handle
 *		strNotBefore: cert notbefore date buffer
 * output:
 * return:
 *		return code
 * remark:
 *		date format: YYYYMMDDHHmmss[+|-]+hhmm
 */
int CGXCAPI_CertGetNotBefore(
	CGCertHandle	hCert,
	int				iFlags,
	char *			strNotBefore);

/* CGXCAPI_CertGetNotAfter
 *
 * get cert notafter date
 * input:
 *		hCert: cert handle
 *		strNotAfter: cert notafter date buffer
 * output:
 * return:
 *		return code
 * remark:
 *		date format: YYYYMMDDHHmmss[+|-]hhmm
 */
int CGXCAPI_CertGetNotAfter(
	CGCertHandle	hCert,
	int				iFlags,
	char *			strNotAfter);

/* CGXCAPI_CertGetSerialNumber
 *
 * get cert subject
 * input:
 *		hCert: cert handle
 * output:
 *		pstrSubject: cert subject
 * return:
 *		return code
 * remark:
 *		call CGCAPIMemFree to release pstrSubject
 */
int CGXCAPI_CertGetSerialNumber(
	CGCertHandle	hCert,
	int				iFlags,
	CGBlob *		pstSerialNumber);

/* CGXCAPI_CertGetKeyUsage
 *
 * get cert keyusage
 * input:
 *		hCert: cert handle
 * output:
 *		piKeyUsage: key usage
 * return:
 *		return code
 * remark:
 */
int CGXCAPI_CertGetKeyUsage(
	CGCertHandle	hCert,
	int *			piKeyUsage);

/* CGXCAPI_CertGetExtension
 *
 * get cert extension
 * input:
 *		hCert: cert handle
 *		iExtension: extension id
 *		iFlags: reserved
 * output:
 *		pExtension: extension
 *		piCritical: critical flag
 * return:
 *		return code
 * remark:
 */
int CGXCAPI_CertGetExtension(
	CGCertHandle	hCert,
	const char *	strExtension,
	int				iFlags,
	void *			pExtension,
	int *			piCritical);

/* CGXCAPIFreeAttributes
 *
 * free cert extension value
 * input:
 *		pConf: conf pointer
 * output:
 * return:
 * remark:
 */
void CGXCAPIFreeAttributes(CGAttributes *pAttrs);

/* CGXCAPI_CertGetDigest
 *
 * get hash of der cert
 * input:
 *		hCert: cert handle
 *		iFlags: researved
 * output:
 *		pstrSubject: cert subject
 * return:
 *		return code
 * remark:
 *		call CGCAPIMemFree to release pstrSubject
 */
int CGXCAPI_CertGetDigest(
	CGCertHandle	hCert,
	int				iFlags,
	CGBlob *		pstHashBlob);

/* CGXCAPI_GetCertIssuer
 *
 * get Cert Handle from StoreHandle filtering by specified subject cert 
 * input:
 *		hSubject: cert handle
 *		hStore: store handle
 * output:
 *		phIssuer: issuer cert handle
 * return:
 *		return code
 * remark:
 *		call CGXCAPIFreeHandle to release phIssuer
 */
int CGXCAPI_GetCertIssuer(
	CGCertHandle	hSubject,
	CGStoreHandle	hStore,
	CGCertHandle *	phIssuer);

/* CGXCAPI_SetPrivateKeyPin
 *
 * Set private key pin
 * input:
 *		hPrivateKey: Private Key Handle
 *		pin: Token Pin
 * output:
 *		
 * return:
 *		return code
 * remark:
 *		
 */
int CGXCAPI_SetPrivateKeyPin(
	CGKeyHandle hPrivateKey, 
	const char *pin, 
	int iFlags ) ;


	
/* CGXCAPI_MultiSignInit
 *
 * 
 * input:
 *		
 *		strProvider: CSP NAME
 * output:
 *		
 * return:
 *		return code
 * remark:
 *		
 */
int CGXCAPI_MultiSignInit( 
	const wchar_t *strProvider , 
	int iFlags );

/* CGXCAPI_MultiSignEnd
 *
 * 
 * input:
 *		
 * output:
 *		
 * return:
 *		return code
 * remark:
 *		
 */

void CGXCAPI_MultiSignEnd( int iFlags ) ;

/* CGXCAPI_LoadStoreFromCSP
 *
 * Create a temporary memory store, load csp certificate
 * input:
 *		
 *		strProvider	: CSP NAME
 *		iFlags		:	0 : default all type 
						CGCAPI_FLAG_DISABLE_KEYSPEC_ATSIGNATURE: ignore AT_SIGNATURE
						CGCAPI_FLAG_ACCESS_PUBLICONLY : no access private key
 * output:
 *		pStore : Store handle
 * return:
 *		return code
 * remark:
 *		
 */
int CGXCAPI_LoadStoreFromCSP(
	CGStoreHandle *pStore, 
	const char *strProvider ,
	int iFlags);

/* CGXCAPI_RegisterCertificate
 *
 * Copy certificate from 1 store to another store
 * input:
 *		fromStore: from CSP Store
 *		toStore	:  to CSP Store
 * output:
 *		
 * return:
 *		return code
 * remark:
 *		
 */
int CGXCAPI_RegisterCertificate(
	CGStoreHandle fromStore, 
	CGStoreHandle toStore, 
	int iFlags);

int CGXCAPI_UnRegisterCertificate(
	const char *strProvider, 
	CGStoreHandle toStore, 
	int iFlags);

// Simplified Function

int CGCAPI_PFXP1Sign(
	unsigned char *	pbP12,
	unsigned long	lP12,
	const char *	strPassword,
	const void *	pbData,
	unsigned long	lData,
	int				iFlags,
	int				iHashalg,
	int				iKeyUsage,
	char **			pstrSignature,
	int *			lSignature,
	char **			pstrCert);

int CGCAPI_PFXP7Sign(
	unsigned char *	pbP12,
	unsigned long	lP12,
	const char *	strPassword,
	const void *	pbData,
	unsigned long	lData,
	int				iFlags,
	int				iHashFlag,
	int				iKeyUsage,
	char **			pstrSignature);

int CGCAPI_PFXGetCertCount(
	unsigned char *	pbP12,
	unsigned long	lP12,
	const char *	strPassword,
	int *			lOutCntCert );


/* CGCAPI_Sign
 *
 * sign data to pkcs7 format
 * input:
 *		pbData: sign data
 *		lData: sign data length
 *		strSubject: select cert subject filter
 *		iFlags: sign flag
 *		iKeyUsage: key usage
 * output:
 *		pstrSignature: signature blob
 * return:
 *		return code
 * remark:
 *		call CGXCAPIMemFreee to release pstrSignature
 */
int CGCAPI_Sign(
	const void *	pbData,
	unsigned long	lData,
	const char *	strSubject,
	int				iFlags,
	int				iKeyUsage,
	char **			pstrSignature);

/* CGCAPI_Verify
 *
 * verify signature in pkcs7 format
 * input:
 *		strSignature: signature
 *		pbData: sign data
 *		lData: sign data length
 *		strSubject: valid cert subject filter
 *		iFlags: sign flag
 *		iKeyUsage: key usage
 *		iTime: verify time
 * output:
 *		pstrSignerInfo: signer info (optional)
 *		pstrCert: signer cert (optional)
 * return:
 *		return code
 * remark:
 *		call CGXCAPIMemFree to release pstrSignerInfo & pstrCert
 */
int CGCAPI_Verify(
	const char *	strSignature,
	const void *	pbData,
	unsigned long	lData,
	const char *	strSubject,
	int				iFlags,
	int				iKeyUsage,
	time_t			iTime,
	char **			pstrSignerInfo,
	char **			pstrCert);

/* CGCAPI_ComposePKCS7
 *
 * compose cert and raw signature pkcs7 format
 * input:
 *		strCert: cert
 *		strRawSig: base64 raw signature
 *		pbData: data
 *		lData: data length
 *		iFlags: reserved
 * output:
 *		pstrSignature: signature
 * return:
 *		return code
 * remark:
 *		call CGXCAPIMemFree to release pstrSignature
 */
int CGCAPI_ComposePKCS7(
	const char *	strCert,
	const char *	strRawSig,
	const char *	pbData,
	unsigned long	lData,
	int				iFlags,
	int				iHashFlags,
	char **			pstrSignature);

/* CGCAPI_ComposeSignature
 *
 * compose cert and signer info to pkcs7 format
 * input:
 *		strCert: cert
 *		strDigest: signer info
 *		iFlags: reserved
 * output:
 *		pstrSignature: signature
 * return:
 *		return code
 * remark:
 *		call CGXCAPIMemFree to release pstrSignature
 */
int CGCAPI_ComposeSignature(
	const char *	strCert,
	const char *	strDigest,
	int				iFlags,
	char **			pstrSignature);

/* CGCAPI_VerifyDigest
 *
 * verify digest
 * input:
 *		strCert: cert
 *		strDigest: signer info
 *		pbData: data
 *		lData: data length
 *		strSubject: cert subject filter
 *		iFlags: verify flag
 *		iTime: verify time
 * output:
 * return:
 *		return code
 * remark:
 */
int CGCAPI_VerifyDigest(
	const char *	strCert,
	const char *	strDigest,
	const void *	pbData,
	unsigned long	lData,
	const char *	strSubject,
	int				iFlags,
	time_t			iTime);

/* CGCAPI_CertEncrypt
 *
 * encrypt by cert
 * input:
 *		strCert: cert
 *		pbData: data
 *		lData: data length
 *		iFlags: encrypt algorithm
 * output:
 *		pstrCipher: cipher blob
 * return:
 *		return code
 * remark:
 *		call CGXCAPIMemFree to release pstrCipher
 */
int CGCAPI_CertEncrypt(
	const char *	strCert,
	const void *	pbData,
	unsigned long	lData,
	int				iFlags,
	char **			pstrCipher);

/* CGCAPI_Encrypt
 *
 * encrypt by cert in store
 * input:
 *		strSubject: cert subject filter
 *		iStores: store to be searched
 *		iSelCertFlags: select cert flag
 *		pbData: data
 *		lData: data length
 *		iFlags: encrypt algorithm
 * output:
 *		pstrCipher: cipher blob
 * return:
 *		return code
 * remark:
 *		call CGXCAPIMemFree to release pstrCipher
 */
int CGCAPI_Encrypt(
	const char *	strSubject,
	int				iStores,
	int				iSelCertFlags,
	const void *	pbData,
	unsigned long	lData,
	int				iFlags,
	int				iKeyUsage,
	char **			pstrCipher);

/* CGCAPI_Decrypt
 *
 * decrypt pkcs7 cipher
 * input:
 *		strCipher: cipher
 *		iFlags: researved
 * output:
 *		ppbClear: clear data
 *		plClear: clear data length (optional)
 * return:
 *		return code
 * remark:
 *		call CGXCAPIMemFree to release ppbClear
 */
int CGCAPI_Decrypt(
	const char *	strCipher,
	int				iFlags,
	int				iKeyUsage,
	void **			ppbClear,
	unsigned long *	plClear);

int CGCAPI_SignAndEncrypt(
	char	*signerSubject,
	char	*strEncryptCert,
	const void *	pbData,
	unsigned long	lData,
	int				iFlags,
	int				iHashFlags,
	int				iKeyUsage,
	char **			pstrSignCipher);

int CGCAPI_DecryptAndVerify(
	const char		*strCipher,
	int				iSignerIndex,
	int				iFlags,
	void **			ppbClear,
	unsigned long *	plClear,
	char			**strSignerCert,
	char			**strEncryptCert);

/* CGCAPI_Hash
 *
 * decrypt pkcs7 cipher
 * input:
 *		pbData: data
 *		lData: data length
 *		iFlags: hash algorithm
 * output:
 *		ppbDigest: base64 encoded hashed digest
 * return:
 *		return code
 * remark:
 *		call CGXCAPIMemFree to release ppbDigest
 */
int CGCAPI_Hash(
	const void *	pbData,
	unsigned long	lData,
	int				iFlags,
	char **			pstrDigest);

/* CGCAPI_ExportPKCS12
 *
 * export cert and private key to pkcs12 format
 * input:
 *		strSubject: certificate subject
 *		iFlags: export flags
 *		strPassword: password to encrypt private key
 *		iKeyUsage:Key Usage
 * output:
 *		ppbP12: pkcs12
 *		plP12: length of pkcs12
 * return:
 *		return code
 * remark:
 *		call CGXCAPIFreeBlob to release pstP12Blob
 */
int CGCAPI_ExportPKCS12(
	const char *	strSubject,
	int				iFlags,
	int				iKeyUsage,
	const char *	strPassword,
	void **			ppbP12,
	unsigned long *	plP12);

int CGCAPI_ValidateCertSubject(const char *strCert,const char *strSubject,int iFlags);

int CGCAPI_CertGetSerialNumber(const char *strCert, int iFlags, char **pstrSerialNumber);

/* CGCAPI_CertGetSubject
 *
 * get cert subject
 * input:
 *		strCert: pem cert
 * output:
 *		pstrSubject: cert subject
 * return:
 *		return code
 * remark:
 *		call CGCAPIMemFree to release pstrSubject
 */
int CGCAPI_CertGetSubject(
	const char *	strCert,
	int				iFlags,
	char **			pstrSubject);

int CGCAPI_CertGetSubjectW(
						   const char *	strCert,
						   int				iFlags,
						   wchar_t **		pstrSubject);

int CGCAPI_CertGetSubjectCNU(
	const char *	strCert,
	int				iFlags,
	wchar_t **			pstrSubject);

/* CGCAPI_CertGetIssuer
 *
 * get cert issuer subject
 * input:
 *		strCert: pem cert
 * output:
 *		pstrIssuer: cert issuer name
 * return:
 *		return code
 * remark:
 *		call CGCAPIMemFree to release pstrSubject
 */
int CGCAPI_CertGetIssuer(
	const char *	strCert,
	int				iFlags,
	char **			pstrIssuer);

int CGCAPI_CertGetIssuerW(
						  const char *	strCert,
						  int				iFlags,
						  wchar_t **		pstrIssuer);

/// add by Luke Yang, 2012/5/16
int CGCAPI_CertGetIssuerCNU(
	const char *	strCert,
	int				iFlags,
	wchar_t **			pstrIssuer);

/* CGCAPI_CertGetNotBefore
 *
 * get cert notbefore date
 * input:
 *		strCert: pem cert
 *		strNotBefore: cert notbefore date buffer
 * output:
 * return:
 *		return code
 * remark:
 *		date format: YYYYMMDDHHmmss[+|-]hhmm
 */
int CGCAPI_CertGetNotBefore(
	const char *	strCert,
	int				iFlags,
	char *			strNotBefore);

/* CGCAPI_CertGetNotAfter
 *
 * get cert notafter date
 * input:
 *		strCert: pem cert
 *		strNotAfter: cert notafter date buffer
 * output:
 * return:
 *		return code
 * remark:
 *		date format: YYYYMMDDHHmmss[+|-]hhmm
 */
int CGCAPI_CertGetNotAfter(
	const char *	strCert,
	int				iFlags,
	char *			strNotAfter);

/* CGCAPI_CertGetDigest
 *
 * get hash of der cert
 * input:
 *		strCert: pem cert
 *		iFlags: hash algorithm(MD5 or ShA1)
 * output:
 *		pstrDigest: hex value of hash
 * return:
 *		return code
 * remark:
 */
int CGCAPI_CertGetDigest(
	const char *	strCert,
	int				iFlags,
	char **			pstrDigest);

/* CGCAPI_XMLInit
 *
 * init XML CGCAPI function
 * input:
 * output:
 * return:
 * remark:
 */
void CGCAPI_XMLInit();

/* CGXCAPI_XMLSign
 *
 * CGCAPI XML sign
 * input:
 *		hPrivateKey: signing key
 *		hCert: signer certificate
 *		pbXML: xml message
 *		lXML: length of pbXML
 *		Ref: reserved:
 *		iFlags: reserved
 * output:
 *		pXMLSignature: xml signature
 * return:
 *		return code
 * remark:
 *		call CGCAPIMemFree to release pXMLSignature
 */
int CGXCAPI_XMLSign(
	CGCertHandle	hCert,
	CGKeyHandle		hPrivateKey,
	const char *	pbXML,
	unsigned long	lXML,
	const char *	Ref,
	int				iFlags,
	char **			pstrXMLSignature);

/* CGCAPI_XMLSign
 *
 * CGCAPI XML sign
 * input:
 *		pbXML: xml message
 *		lXML: length of pbXML
 *		Ref: reserved:
 *		strSubject: select certificate subject
 *		iSelFlags: select cert fiag
 *		iFlags: reserved
 * output:
 *		pXMLSignature: xml signature
 * return:
 *		return code
 * remark:
 *		call CGCAPIMemFree to release pXMLSignature
 */
int CGCAPI_XMLSign(
	const char *pbXML,
	unsigned long lXML,
	const char * Ref,
	const char * strSubject,
	int iSelFlags,
	int iKeyUsage,
	int iFlags,
	char ** pXMLSignature);

/* CGXCAPI_XMLVerify
 *
 * xml verify
 * input:
 *		XMLSignature: xml signature
 *		pbXML: xml message
 *		lXML: length of pbXML
 *		Ref: reserved:
 *		hStore: cert chain & crl store
 *		strSubject: valid certificate subject
 *		iTime: validity time
 *		iFlags: reserved
 * output:
 *		pstrDigest: base64 encoded digest value
 *		pstrCert: base64 encoded x509 cert
 * return;
 *		return code
 * remark:
 *		call CGCAPIMemFree to release pstrDigest & pstrCert
 */
int CGXCAPI_XMLVerify(
	const char *XMLSignature,
	const char *pbXML,
	unsigned long lXML,
	const char *Ref,
	CGStoreHandle hStore,
	const char *strSubject,
	int iFlags,
	time_t iTime,
	char **pstrDigest,
	char **pstrCert);

/* CGXCAPI_XMLVerify
 *
 * xml verify
 * input:
 *		XMLSignature: xml signature
 *		pbXML: xml message
 *		lXML: length of pbXML
 *		Ref: reserved:
 *		strSubject: valid certificate subject
 *		iTime: validity time
 *		iFlags: reserved
 * output:
 *		pstrDigest: base64 encoded digest value
 *		pstrCert: base64 encoded x509 cert
 * return;
 *		return code
 * remark:
 *		call CGCAPIMemFree to release pstrDigest & pstrCert
 */
int CGCAPI_XMLVerify(
	const char *XMLSignature,
	const char * pbXML,
	unsigned long lXML,
	const char *Ref,
	const char *strSubject,
	int iFlags,
	int iKeyUsage,
	time_t iTime,
	char **pstrDigest,
	char **pstrCert);

/* CGXCAPI_ShowCertificate
 *
 * Show Message Dialog with two button "憑證檢視" & "確定"
 * input:
 *		strMessage : Dialog Message
 *		hCert : Content to show after press "憑證檢視"
 *		iFlags : Reserve parameter
 * return;
 *		return code
 * remark:
 */
int CGXCAPI_ShowCertificate(WCHAR *strMessage, CGCertHandle hCert, int iFlags);

/* CGCAPI_ShowCertificate
 *
 * Show Message Dialog with two button "憑證檢視" & "確定"
 * input:
 *		strMessage : Dialog Message
 *		strCert : Content to show after press "憑證檢視"
 *		iFlags : Reserve parameter
 * return;
 *		return code
 * remark:
 */
int CGCAPI_ShowCertificate(WCHAR *strMessage, const char *strCert, int iFlags);

/* CGCAPI_PKCS7GetUserCertificate
 *
 * get x509 cert from pkcs7
 * input:
 *		strP7Cert: p7 cert
 * output:
 *		strX509Cert: X509 cert
 * return:
 *		return code
 * remark:
 *		call CGCAPIMemFree to release strX509Cert
 */
int CGCAPI_PKCS7GetUserCertificate(char *strP7Cert,char **strX509Cert);

/* CGCAPI_ImportPKCS12
 *
 * import pkcs12 format cert and private key
 * input:
 *		iStoreFlags: choose Store to import
 *		pbP12: pkcs12 content
 *		lP12: pbPKCS12 length
 *		strPassword: password to encrypt private key
 *		iFlags: import flag
 * output:
 *		pstP12Blob: pkcs12
 * return:
 *		return code
 * remark:
 *		call CGXCAPIFreeBlob to release pstP12Blob
 */
int CGCAPI_ImportPKCS12(
	int iStoreFlags,
	unsigned char *	pbP12,
	unsigned long	lP12,
	const char *	strPassword,
	const wchar_t *	wstrFriendlyName,
	int				iFlags);

/* CGCAPI_CertGetExtension
 *
 * Parse Cert Extersion column
 * input:
 *		X509Cert: cert
 *		extname: specified FXML X509V3 extension name 
 * output:
 *		extvalue: corresponded value 
 *		return code
 * remark:
 *		call CGCryptMemFree to release extvalue
 */

int CGCAPI_CertGetFXMLExtension(
	const char	*X509Cert,
	int			index,
	char		**extvalue);


/* CGCAPI_PureSign
 *
 * input:
 *		pbData: sign data
 *		lData: sign data length
 *		strSubject: select cert subject filter
 *		iFlags: select cert flag & base64 encode
 *		iFlags: sign flag (hash algor, do hash or not, sign raw data or with hash)
 *		iKeyUsage: key usage
 * output:
 *		pstrSignature: signature blob
 * return:
 *		return code
 * remark:
 *		call CGXCAPIMemFreee to release pstrSignature
 *		sign raw data is support on after win2000.
 *      Add By Ken. 2003.4.29
 */
int CGCAPI_PureSign(
	const void *	pbData,
	unsigned long	lData,
	const char *	strSubject,
	int				iFlags,
	int				iHashalg,
	int				iKeyUsage,
	char **			pstrSignature,
	int *			lSignature,
	char **			pstrCert);

/* CGCAPI_SignEx
 *
 * sign data to pkcs7 format
 * input:
 *		pbData: sign data
 *		lData: sign data length
 *		strSubject: select cert subject filter
 *		strProvider: CSP Provider
 *		strPin:	Token Pin
 *		iFlags: sign flag
 *		iKeyUsage: key usage
 * output:
 *		pstrSignature: signature blob
 * return:
 *		return code
 * remark:
 *		call CGXCAPIMemFreee to release pstrSignature
 */
int CGCAPI_SignEx(
        const void * pbData,
        unsigned long lData,
        const char * strSubject,
		const char * strIssuerSubject,
		const char *strProvider,
        const char *strPin,
        int    iFlags,
		int    iHashFlag,
        int    iKeyUsage,
        char **   pstrSignature);

/* CGCAPI_RegisterCertificate
 *
 * Add certificate in csp to Local Machine My store
 * input:
 *		strProvider : CSP Name 
 * output:
 *		return code
 * remark:
 *		
 */
int CGCAPI_RegisterCertificate(const char *strProvider , int iFlags);

int CGCAPI_UnRegisterCertificate(const char *strProvider , int iFlags);

int CGCAPI_PureSignEx(
	const void *	pbData,
	unsigned long	lData,
	const char *	strSubject,
	const char *	strIssuerSubject,
	const char *	strProvider,
	const char *	strPin,
	int				iFlags,
	int				iHashalg,
	int				iKeyUsage,
	char **			pstrSignature,
	int *			lSignature,
	char **			pstrCert);

void CGXCAPI_ShowDebugMessage(int bShow);


int CGXCAPI_GetUserCertificatesFromCSP(
	const char *	strProvider,
	const char *	strSubject,
	int				iFlags,
	int				iKeyUsage,
	int				iDisableATSpec,
	CGCertHandle **	phCerts,
	int *			piCerts);


int CGXCAPI_ReadFileAndHashA(
	char *filepath,			// -> UNC Format
	int iDigestMethod, 
	int iFlags, 
	char **rtnContent, 
	int *rtnLength) ;

int CGXCAPI_ReadFileAndHashW(
	wchar_t *filepath,			// -> UNC Format
	int iDigestMethod, 
	int iFlags, 
	char **rtnContent, 
	int *rtnLength) ;

int CGXCAPI_ChangeP12Pwd(unsigned char *ppfx, int pfxlen, 
						 char *strOldPFXPasswd, char *strNewPFXPasswd,
						 CGBlob *pstP12Blob) ;
int CGXCAPI_DecodeP12(
						unsigned char	*ppfx, 
						int				pfxlen , 
						char			*strPFXPasswd , 
						CGCertHandle	*pouthCert , 
						CGKeyHandle		*pouthKey) ;

int CGXCAPI_ImportP12ToCSP(
						char			*CSP_Name, 
						char			*strPin,
						int				iFlags, 
						CGKeyHandle		hPriKey, 
						CGCertHandle	hCert);

int CGXCAPI_LoadStoreFromPKCS12(
					unsigned char *ppfx, int pfxlen , char *strPFXPasswd,
					CGStoreHandle *pHandle, 
					int iFlags);
int CGXCAPI_SaveStoreToFile(CGStoreHandle hStore) ;

int CGXCAPI_LoadStoreFromStoreFile(	 CGStoreHandle *phOutStore,  int iFlags) ;

int CGXCAPI_DeleteUsrCertFromStore(
	CGHandle hCertStore,
	int		iFlags,
	int		iKeyUsage);

int CGXCAPI_DeleteUsrCert(
	const char *	strSubject,
	int				iFlags,
	int				iKeyUsage);

int CGXCAPI_DeleteUsrCertFromCSP(
	const char *	strProvider,
	const char *	strSubject,
	int				iFlags,
	int				iKeyUsage);

int CGCAPI_CheckPFX(
	unsigned char *	pbP12,
	unsigned long	lP12,
	const char *	strPassword,
	char *			subject,
	int				iFlags,
	int				iKeyUsage);

int CGXCAPI_ImportCertificate(
	CGHandle hStore,
	CGCertHandle hCert,
	int	iFlags);

int CGCAPI_ImportCertificate(
	int iStoreFlags,
	char *strX509Cert,
	int	iFlags);

int CGCAPI_DeleteEmptyContainer(
	const char *strProvider);

int CGXCAPI_AcceptPKCS7(char *cstrCertificate, int iFlags,char *pin,WCHAR *wfriendly_name,long *result);
int CGXCAPI_CreatePKCS10(char *HashAlgorithm,int providerType,char *providerName,int KeySpec,int GenKeyFlags,wchar_t *dn,char *oid,char *pin,char **CertReq,long *result);
int CGXCAPI_EnumProviders(int providerType,int iIndex,int iFlags,char **strProvName,long *result);
void GenContainerName(char *name);
void GenContainerNameW(wchar_t *name);


int CGXCAPI_ComposePKCS7(
	CGCertHandle	hCert,
	CGBlob			p1Blob,
	const void *	pbData,
	unsigned long	lData,
	int				iFlags,
	int				iHashFlags,
	CGPKCS7Handle *	hSignature);

/***/
void xBinaryToHexString( unsigned char * value, int len, char *strHex );
void debugTimeToFile(const char *debugMsg, long i1 );
void setCheckU3(int flag) ;

/***/

int CGXCAPI_InstallCert(char *cstrCertificate, int iFlags);

void CGXCAPICheckKeyUsage(int iCheck);
int CGXCAPIGetCheckKeyUsage();

#ifdef __cplusplus
}
#endif

#endif
