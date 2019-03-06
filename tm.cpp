#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <strsafe.h>
#include <locale.h>

#define UNICODE
#define _UNICODE


using namespace std;

void ErrorExit(LPTSTR lpszFunction, DWORD dw)
{
	// Retrieve the system error message for the last-error code

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message and exit the process

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	printf("%s\n", (LPCTSTR)lpDisplayBuf);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
	ExitProcess(dw);
}

typedef struct _REG_TZI_FORMAT
{
	LONG Bias;
	LONG StandardBias;
	LONG DaylightBias;
	SYSTEMTIME StandardDate;
	SYSTEMTIME DaylightDate;
} REG_TZI_FORMAT;

int main()
{
	TIME_ZONE_INFORMATION tziOld, tziNew;
	DWORD dwRet; 
	
	REG_TZI_FORMAT regTZI;

	DWORD dwBufLen = sizeof(regTZI);
	LONG lRet;

	// Enable the required privilege

	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	

	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
	LookupPrivilegeValue(NULL, SE_TIME_ZONE_NAME, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

	// Retrieve the current time zone information

	dwRet = GetTimeZoneInformation(&tziOld);

	setlocale(LC_CTYPE, "");
	if (dwRet == TIME_ZONE_ID_STANDARD || dwRet == TIME_ZONE_ID_UNKNOWN)
		wprintf(L"%s\n", tziOld.StandardName);
	else if (dwRet == TIME_ZONE_ID_DAYLIGHT)
		wprintf(L"%s\n", tziOld.DaylightName);
	else
	{
		printf("GTZI failed (%d)\n", GetLastError());
		return 0;
	}
	wprintf(L"Year: %u\n", tziOld.StandardDate.wYear);
	wprintf(L"Mon: %u\n", tziOld.StandardDate.wMonth);
	wprintf(L"Day: %u\n", tziOld.StandardDate.wDay);
	wprintf(L"Hour: %u\n", tziOld.StandardDate.wHour);
	wprintf(L"Min: %u\n", tziOld.StandardDate.wMinute);
	wprintf(L"Sec: %u\n", tziOld.StandardDate.wSecond);
	
	HKEY hKey;

	//setlocale(LC_ALL, "chs");

	LPCSTR sub = "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Time Zones\\Eastern Standard Time";
	lRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE, sub, 0, KEY_QUERY_VALUE, &hKey);
	printf("%d\n", lRet);
	if (lRet != ERROR_SUCCESS) {
		ErrorExit(TEXT("RegOpenKeyExA"), 2L);
		return 0;
	}

	printf("begin read value\n");
	lRet = RegQueryValueExA(hKey, "TZI", NULL, NULL, (LPBYTE)&regTZI, &dwBufLen);
	
	if (lRet != ERROR_SUCCESS) {
		printf("error!");
		ErrorExit(TEXT("RegQueryValueExA"), GetLastError());
		return 0;
	}
	/*printf("%ld\n", regTZI.Bias);
	printf("%ld\n", regTZI.StandardBias);
	SYSTEMTIME sysTime = regTZI.StandardDate;
	wprintf(L"Year: %u\n", sysTime.wYear);
	wprintf(L"Mon: %u\n", sysTime.wMonth);
	wprintf(L"Day: %u\n", sysTime.wDay);
	wprintf(L"Hour: %u\n", sysTime.wHour);
	wprintf(L"Min: %u\n", sysTime.wMinute);
	wprintf(L"Sec: %u\n", sysTime.wSecond);*/
	
	DWORD dwBufSize = 32 * sizeof(WCHAR);
	
	if (RegQueryValueExA(hKey, "Dlt", NULL, NULL, (LPBYTE)tziNew.DaylightName, &dwBufSize) == ERROR_SUCCESS)
		wprintf(L"Dlt:%s\n", tziNew.DaylightName);

	if (RegQueryValueExA(hKey, "Std", NULL, NULL, (LPBYTE)tziNew.StandardName, &dwBufSize) == ERROR_SUCCESS)
		wprintf(L"Std:%s\n", tziNew.StandardName);

	RegCloseKey(hKey);
	

	// Adjust the time zone information

	ZeroMemory(&tziNew, sizeof(tziNew));
	tziNew.Bias = regTZI.Bias;
	tziNew.Bias = regTZI.Bias;
	tziNew.StandardBias = regTZI.StandardBias;
	tziNew.DaylightBias = regTZI.DaylightBias;
	tziNew.StandardDate.wYear = regTZI.StandardDate.wYear;
	tziNew.StandardDate.wMonth = regTZI.StandardDate.wMonth;
	tziNew.StandardDate.wDay = regTZI.StandardDate.wDay;
	tziNew.StandardDate.wDayOfWeek = regTZI.StandardDate.wDayOfWeek;
	tziNew.StandardDate.wHour = regTZI.StandardDate.wHour;
	tziNew.StandardDate.wMinute = regTZI.StandardDate.wMinute;
	tziNew.StandardDate.wSecond = regTZI.StandardDate.wSecond;
	tziNew.StandardDate.wMilliseconds = regTZI.StandardDate.wMilliseconds;

	tziNew.DaylightDate.wYear = regTZI.DaylightDate.wYear;
	tziNew.DaylightDate.wMonth = regTZI.DaylightDate.wMonth;
	tziNew.DaylightDate.wDay = regTZI.DaylightDate.wDay;
	tziNew.DaylightDate.wDayOfWeek = regTZI.DaylightDate.wDayOfWeek;
	tziNew.DaylightDate.wHour = regTZI.DaylightDate.wHour;
	tziNew.DaylightDate.wMinute = regTZI.DaylightDate.wMinute;
	tziNew.DaylightDate.wSecond = regTZI.DaylightDate.wSecond;
	tziNew.DaylightDate.wMilliseconds = regTZI.DaylightDate.wMilliseconds;

	if (!SetTimeZoneInformation(&tziNew))
	{
		printf("STZI failed (%d)\n", GetLastError());
		return 0;
	}

	// Retrieve and display the newly set time zone information

	
	// Disable the privilege

	tkp.Privileges[0].Attributes = 0;
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

	printf("success~!");
	return 1;
}