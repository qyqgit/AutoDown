// Test1.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Timer.h"

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名
LPTSTR szBuffer = new TCHAR[10];
int num = 3600;

// 此代码模块中包含的函数的前向声明:
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
		//获取进程标志
		if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
			return;
		//获取关机特权的LUID
		LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
		tkp.PrivilegeCount = 1;
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		//获取这个进程的关机特权
		AdjustTokenPrivileges(hToken, false, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
		if(GetLastError() != ERROR_SUCCESS)
			return;
		//强制关闭计算机
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

 	// TODO: 在此放置代码。
	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TIMER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TIMER));

	// 主消息循环:
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
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
//  注释:
//
//    仅当希望
//    此代码与添加到 Windows 95 中的“RegisterClassEx”
//    函数之前的 Win32 系统兼容时，才需要此函数及其用法。调用此函数十分重要，
//    这样应用程序就可以获得关联的
//    “格式正确的”小图标。
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
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中

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
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: 处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
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
		// 分析菜单选择:
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
		// TODO: 在此添加任意绘图代码...
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

// “关于”框的消息处理程序。
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
