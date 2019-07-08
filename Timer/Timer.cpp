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
HWND ghWnd;

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
		return ;
	}
	if(RegSetValueEx(hKey, TEXT("Num"), 0, REG_DWORD, (BYTE*)&dwNum, sizeof(DWORD)) != ERROR_SUCCESS){
		return;
	}
}
void getNum(){
	DWORD dwNum;
	DWORD dwType;
	DWORD dwSize = sizeof(DWORD);
	HKEY hKey;

	if(RegOpenKey(HKEY_CURRENT_USER, _T("Software\\MyTimer\\Config"), &hKey) != ERROR_SUCCESS){
		MessageBox(0, _T("error1"), _T("error1"), 0);
		return;
	}
	if(RegQueryValueEx(hKey, _T("Num"), 0, &dwType, (LPBYTE)&dwNum, &dwSize) != ERROR_SUCCESS){
		MessageBox(0, _T("error2"), _T("error2"), 0);
		return;
	}
	num = dwNum;
	RegCloseKey(hKey);
}
void enableAutoRun(){
	LPCTSTR path = TEXT("Timer");
	BOOL isWOW64;
	REGSAM p;
	HKEY hKey;

	IsWow64Process(GetCurrentProcess(), &isWOW64);
	if(isWOW64){
		p = KEY_WRITE | KEY_WOW64_64KEY | KEY_ALL_ACCESS;
	} else {
		p = KEY_WRITE | KEY_ALL_ACCESS;
	}
	if(RegCreateKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, NULL, 0, p, NULL, &hKey, NULL) != ERROR_SUCCESS){
		return ;
	}
	if(RegSetValueEx(hKey, TEXT("TimerShutDown"), 0, REG_SZ, (BYTE*)path, sizeof(path)*sizeof(TCHAR)) != ERROR_SUCCESS){
		return;
	}
	RegCloseKey(hKey);
}
void disableAutoRun(){
	HKEY hKey;
	int aa = 0;
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS){
		MessageBox(0, _T("error11"), _T("error11"), 0);
		return;
	}
	aa = RegDeleteKey(hKey, TEXT("TimerShutDown"));
	if(aa != ERROR_SUCCESS){
		MessageBox(0, _T("error22"), _T("error22"), 0);
		return;
	}
	RegCloseKey(hKey);
}
void fun(){
	num--;
	InvalidateRect(ghWnd, NULL, TRUE);
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
		//if(!ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 0))
		//	return;
		DestroyWindow(ghWnd);
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
   ghWnd = hWnd;

   if (!hWnd)
   {
      return FALSE;
   }
   getNum();
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   SetTimer(hWnd, 0, 1000, (TIMERPROC)fun);
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
