// Test1.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "Timer.h"

#define MAX_LOADSTRING 100

// ȫ�ֱ���:
HINSTANCE hInst;								// ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������
LPTSTR szBuffer = new TCHAR[10];
int num = 3600;

// �˴���ģ���а����ĺ�����ǰ������:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void setNum(DWORD dwNum){
	HKEY hKey;
	BOOL isWOW64;
	REGSAM p;

	IsWow64Process(GetCurrentProcess(), &isWOW64);
	if(isWOW64){
		p = KEY_WRITE | KEY_WOW64_64KEY;
	} else {
		p = KEY_WRITE;
	}
	if(RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("Software\\MyTimer\\Config"), 0, NULL, 0, p, NULL, &hKey, NULL) != ERROR_SUCCESS){
		return;
	}
	if(RegSetValueEx(hKey, TEXT("Num"), 0, REG_DWORD, (BYTE*)&dwNum, sizeof(DWORD)) != ERROR_SUCCESS){
		RegCloseKey(hKey);
		return;
	}
	RegCloseKey(hKey);
}
void getNum(){
	DWORD dwNum;
	DWORD dwType;
	DWORD dwSize = sizeof(DWORD);
	HKEY hKey;

	if(RegOpenKey(HKEY_CURRENT_USER, _T("Software\\MyTimer\\Config"), &hKey) != ERROR_SUCCESS){
		return;
	}
	if(RegQueryValueEx(hKey, _T("Num"), 0, &dwType, (LPBYTE)&dwNum, &dwSize) != ERROR_SUCCESS){
		RegCloseKey(hKey);
		return;
	}else{
		num = dwNum;
	}
	RegCloseKey(hKey);
}
void enableAutoRun(){
	wchar_t pFileName[MAX_PATH] = {0};
	LPTSTR path = new TCHAR[MAX_PATH];
	DWORD dwRet;
	BOOL isWOW64;
	REGSAM p;
	HKEY hKey;

	dwRet = GetModuleFileName(NULL, pFileName, MAX_PATH);
	wsprintf(path, _T("\"%ls\""), pFileName);
	IsWow64Process(GetCurrentProcess(), &isWOW64);
	if(isWOW64){
		p = KEY_WRITE | KEY_WOW64_64KEY;
	} else {
		p = KEY_WRITE;
	}
	if(RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, NULL, 0, p, NULL, &hKey, NULL) != ERROR_SUCCESS){
		delete[] path;
		return ;
	}
	if(RegSetValueEx(hKey, TEXT("TimerShutDown"), 0, REG_SZ, (BYTE*)path, wcslen(path)*sizeof(TCHAR)) != ERROR_SUCCESS){
		delete[] path;
		RegCloseKey(hKey);
		return;
	}
	delete[] path;
	RegCloseKey(hKey);
}
void disableAutoRun(){
	HKEY hKey;
	if(RegOpenKey(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), &hKey) != ERROR_SUCCESS){
		return;
	}
	if(RegDeleteValue(hKey, TEXT("TimerShutDown")) != ERROR_SUCCESS){
		RegCloseKey(hKey);
		return;
	}
	RegCloseKey(hKey);
}
void CALLBACK shutDown(HWND hWnd, UINT, UINT_PTR, DWORD){
	num--;
	InvalidateRect(hWnd, NULL, TRUE);
	if(num == 0){
		HANDLE hToken;
		TOKEN_PRIVILEGES tkp;
		//��ȡ���̱�־
		if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
			return;
		//��ȡ�ػ���Ȩ��LUID
		LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
		tkp.PrivilegeCount = 1;
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		//��ȡ������̵Ĺػ���Ȩ
		AdjustTokenPrivileges(hToken, false, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
		if(GetLastError() != ERROR_SUCCESS)
			return;
		//ǿ�ƹرռ����
		if(!ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 0))
			return;
		DestroyWindow(hWnd);
	}
}
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: �ڴ˷��ô��롣
	MSG msg;
	HACCEL hAccelTable;

	// ��ʼ��ȫ���ַ���
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TIMER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ִ��Ӧ�ó����ʼ��:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TIMER));

	// ����Ϣѭ��:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
//
//  ע��:
//
//    ����ϣ��
//    �˴�������ӵ� Windows 95 �еġ�RegisterClassEx��
//    ����֮ǰ�� Win32 ϵͳ����ʱ������Ҫ�˺��������÷������ô˺���ʮ����Ҫ��
//    ����Ӧ�ó���Ϳ��Ի�ù�����
//    ����ʽ��ȷ�ġ�Сͼ�ꡣ
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TIMER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_TIMER);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��:
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

   hWnd = CreateWindow(szWindowClass, szTitle, WS_SYSMENU,
      CW_USEDEFAULT, CW_USEDEFAULT, 235, 70, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }
   getNum();
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   SetTimer(hWnd, 0, 1000, shutDown);
   return TRUE;
}

//
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��: ���������ڵ���Ϣ��
//
//  WM_COMMAND	- ����Ӧ�ó���˵�
//  WM_PAINT	- ����������
//  WM_DESTROY	- �����˳���Ϣ������
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// �����˵�ѡ��:
		switch (wmId)
		{
		case ID_01:
			num = 60;
			setNum(num);
			break;
		case ID_05:
			num = 60 * 5;
			setNum(num);
			break;
		case ID_10:
			num = 60 * 10;
			setNum(num);
			break;
		case ID_30:
			num = 60 * 30;
			setNum(num);
			break;
		case ID_60:
			num = 60 * 60;
			setNum(num);
			break;
		case ID_AUTO_ENABLE:
			enableAutoRun();
			break;
		case ID_AUTO_DISABLE:
			disableAutoRun();
			break;
		case ID_SUB_01:
			num -= 60;
			break;
		case ID_SUB_05:
			num -= 60 * 5;
			break;
		case ID_SUB_10:
			num -= 60 * 10;
			break;
		case ID_SUB_30:
			num -= 60 * 30;
			break;
		case ID_SUB_60:
			num -= 60 * 60;
			break;
		case ID_ADD_01:
			num += 60;
			break;
		case ID_ADD_05:
			num += 60 * 5;
			break;
		case ID_ADD_10:
			num += 60 * 10;
			break;
		case ID_ADD_30:
			num += 60 * 30;
			break;
		case ID_ADD_60:
			num += 60 * 60;
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		if(num < 0)
			num = 10;
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: �ڴ���������ͼ����...
		wsprintf(szBuffer, _T("%d"), num);
	  	DrawText(ps.hdc, szBuffer, lstrlen(szBuffer), &(ps.rcPaint), DT_CENTER | DT_BOTTOM | DT_SINGLELINE);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// �����ڡ������Ϣ�������
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
