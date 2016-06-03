#pragma comment(lib,"gdiplus")
#pragma comment(lib,"dwmapi")

#include <windows.h>
#include <dwmapi.h>
#include <math.h>
#include <gdiplus.h>

using namespace Gdiplus;

#define WIDTH 50
TCHAR szClassName[] = TEXT("Window");

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
		SetTimer(hWnd, 0x1234, 1000, 0);
		break;
	case WM_TIMER:
		InvalidateRect(hWnd, 0, 1);
		break;
	case WM_ERASEBKGND:
		{
			SYSTEMTIME st;
			GetLocalTime(&st);
			Graphics g((HDC)wParam);
			SolidBrush brush(Color(255, 32, 32, 32));
			for (int i = 0; i < 3; ++i)
			{
				const int n = ((WORD*)&st)[4 + i];
				for (int j = 0; j < 8; ++j)
				{
					if (!((n >> j) & 1))
					{
						const int x = WIDTH*(2 * i - j / 4 + 1);
						const int y = WIDTH*(3 - j % 4);
						g.FillRectangle(&SolidBrush(Color::Black), x, y, WIDTH, WIDTH);
						g.ExcludeClip(Rect(x, y, WIDTH, WIDTH));
					}
				}
			}
			g.FillEllipse(&brush, -WIDTH * 6, -WIDTH * 4, WIDTH * 6 * 4, WIDTH * 4 * 4);
		}
		return 1;
	case WM_DESTROY:
		KillTimer(hWnd, 0x1234);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	ULONG_PTR gdiToken;
	GdiplusStartupInput gdiSI;
	GdiplusStartup(&gdiToken, &gdiSI, NULL);
	MSG msg;
	WNDCLASS wndclass = {
		0,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0, IDC_ARROW),
		0,
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	RECT rect = { 0, 0, WIDTH * 6, WIDTH * 4 };
	AdjustWindowRectEx(&rect, WS_CAPTION | WS_SYSMENU, 0, WS_EX_TOOLWINDOW);
	HWND hWnd = CreateWindowEx(
		WS_EX_TOOLWINDOW,
		szClassName,
		TEXT("Binary Clock"),
		WS_CAPTION | WS_SYSMENU,
		CW_USEDEFAULT,
		0,
		rect.right - rect.left,
		rect.bottom - rect.top,
		0,
		0,
		hInstance,
		0
		);
	MARGINS margins = { -1, -1, -1, -1 };
	DwmExtendFrameIntoClientArea(hWnd, &margins);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	GdiplusShutdown(gdiToken);
	return (int)msg.wParam;
}
