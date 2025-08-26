#ifndef __ROOME_CLIENT_H__
#define __ROOME_CLIENT_H__

#ifdef LIBRARY_EXPORTS
	#define ROOME_API __declspec(dllexport)
#elif LIBRARY_IMPORTS
	#define ROOME_API __declspec(dllimport)
#else
#    define ROOME_API
#endif

#ifdef __cplusplus

extern "C"
{

#endif

ROOME_API void Roome_Init(HWND hWnd, const char *assetZipPath);
ROOME_API void Roome_Login(const char *server_name, const char *username, const char *passwd);
ROOME_API void Roome_Deinit();
ROOME_API void Roome_Draw();
ROOME_API int Roome_ProcessWndMessage(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam, BOOL bUnicode);
ROOME_API int Roome_SendClipboardImage();

#ifdef __cplusplus
}
#endif

#endif
