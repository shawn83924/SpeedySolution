/*
 * TWCA_LinuxSign.h - PKCS#7 signer (C API)
 *
 * A thin extern "C" wrapper around the C++ ibf::TwcaLinuxSign class, suitable for
 * loading from Python (ctypes), C#/.NET (P/Invoke) or plain C.
 *
 * All functions returning `const char*` return a pointer into a buffer owned
 * by the handle. The pointer stays valid until the next call on the same
 * handle or until TWCA_Free(). Do NOT free it.
 */
#ifndef TWCA_SIGN_H
#define TWCA_SIGN_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- error codes ---------------------------------------------------------*/
#define TWCA_OK                 0
#define TWCA_ERR_PARAM          5005  /* bad argument                        */
#define TWCA_ERR_CONFIG         5001  /* config / properties problem         */
#define TWCA_ERR_FILE           1002  /* file not readable                   */
#define TWCA_ERR_PFX_LOAD       5204  /* .pfx file could not be opened/read  */
#define TWCA_ERR_PFX_PARSE      5205  /* wrong password / corrupt .pfx       */
#define TWCA_ERR_CERT_FILTER    5301  /* no cert matched PFX.signerFilter    */
#define TWCA_ERR_CERT_EXPIRED   5302  /* cert outside its validity period    */
#define TWCA_ERR_KEYUSAGE       5303  /* cert keyUsage not for signing       */
#define TWCA_ERR_ENCODING       5901  /* iconv encoding conversion failed    */
#define TWCA_ERR_SIGN           5400  /* PKCS#7 signing failed               */

typedef void* TWCAHandle;

/* Create / destroy a signer instance. */
TWCAHandle TWCA_New(void);
void       TWCA_Free(TWCAHandle h);

/* Load configuration from a ".properties" file: the default .pfx path and
 * signing options. The password and certificate are supplied via TWCA_SelectCert.
 * Returns TWCA_OK (0) on success. */
int TWCA_LoadProperty(TWCAHandle h,
                      const char* propertyFile);

/* Select (load) the signing certificate from a specific .pfx file.
 * `pfxFile` may be NULL or "" to use the default PFX.path/PFX.file from the
 * last TWCA_LoadProperty() call. `password` is the .pfx password.
 * Returns TWCA_OK (0) on success. */
int TWCA_SelectCert(TWCAHandle h,
                    const char* pfxFile,
                    const char* password);

/* Sign `len` bytes at `data`. `data` is treated as UTF-8. `encoding` selects
 * the signed content encoding: only "UTF-8" and "UTF-16LE" are supported
 * (case-insensitive, dash optional). NULL/"" selects the default "UTF-16LE".
 * "UTF-8" signs as-is; "UTF-16LE" converts before signing; any other value
 * fails with TWCA_ERR_ENCODING.
 * `digest` selects the hash: "SHA1" (default) or "SHA256"; NULL/"" means SHA1.
 * Returns TWCA_OK (0) on success. */
int TWCA_SignPkcs7(TWCAHandle h,
                   const unsigned char* data, int len,
                   const char* encoding,
                   const char* digest);

/* TWCA_SignPkcs7File is currently disabled (not required).
 * Sign the whole content of a file (raw bytes). `digest` as in TWCA_SignPkcs7. */
#if 0
int TWCA_SignPkcs7File(TWCAHandle h, const char* fileName,
                       const char* digest);
#endif

/* Base64 of the DER PKCS#7 from the last successful sign. */
const char* TWCA_GetSignedPkcs7(TWCAHandle h);

/* Signer certificate info. */
const char* TWCA_GetSignerCertB64(TWCAHandle h);
const char* TWCA_GetSignerCertSubject(TWCAHandle h);
const char* TWCA_GetSignerCertSubjectCN(TWCAHandle h);
const char* TWCA_GetSignerCertIssuer(TWCAHandle h);
const char* TWCA_GetSignerCertNotBefore(TWCAHandle h);
const char* TWCA_GetSignerCertNotAfter(TWCAHandle h);
const char* TWCA_GetSignerCertSerial(TWCAHandle h);
const char* TWCA_GetSignerCertFinger(TWCAHandle h);

/* Error info for the last operation on this handle. */
int         TWCA_GetErrorCode(TWCAHandle h);
const char* TWCA_GetErrorMsg(TWCAHandle h);

/* Library version string (static, always valid). */
const char* TWCA_Version(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TWCA_SIGN_H */
