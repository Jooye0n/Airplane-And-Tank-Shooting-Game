// Win32Project1.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "Win32Project1.h"
#include "vg\openvg.h"
#include "egl\egl.h"
#include "vgLoadImage.h"
#include "Time.h"

#define KEY_DOWN(code) (GetAsyncKeyState(code)&0x8000)

static EGLDisplay currentDisplay;
static EGLSurface currentSurface;
static EGLContext currentContext;

HWND hWnd;

VGImage planeImage;
VGImage backImage;
VGImage missileImage;
VGImage tankImage;
VGImage bombImage;
VGImage enemyImage;

struct PLANE {
	int x, y;
	int throwing;
}plane;

struct Missile{
	int shoot;
	int x, y;
	float vx, vy;
}missile[100];

struct Tank{
	int shoot;
	int x, y;
}tank[100];

struct Bomb {
	int shoot;
	int x, y;
	float vx, vy;
}bomb[100];

struct Enemy{
	int shoot;
	int x, y;
}enemy[100];

void timerproc();

int cx = 0, k = 0;

#define MAX_LOADSTRING 100

// ���� ����:
HINSTANCE hInst;                                // ���� �ν��Ͻ��Դϴ�.
WCHAR szTitle[MAX_LOADSTRING];                  // ���� ǥ���� �ؽ�Ʈ�Դϴ�.
WCHAR szWindowClass[MAX_LOADSTRING];            // �⺻ â Ŭ���� �̸��Դϴ�.

												// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: ���⿡ �ڵ带 �Է��մϴ�.

	// ���� ���ڿ��� �ʱ�ȭ�մϴ�.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_WIN32PROJECT1, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32PROJECT1));

	MSG msg;

	// �⺻ �޽��� �����Դϴ�.
	DWORD lastTime = GetTickCount();
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, 1))
		{
			if (!TranslateAccelerator(msg.hwnd, NULL, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			if (msg.message == WM_QUIT) 
				break;
		}
		DWORD curTime = GetTickCount();
		if (curTime - lastTime>32) // 30 frame per second
		{
			lastTime = lastTime + 33;
			timerproc();
		}
	}
	return (int)msg.wParam;
}



//
//  �Լ�: MyRegisterClass()
//
//  ����: â Ŭ������ ����մϴ�.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32PROJECT1));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WIN32PROJECT1);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   �Լ�: InitInstance(HINSTANCE, int)
//
//   ����: �ν��Ͻ� �ڵ��� �����ϰ� �� â�� ����ϴ�.
//
//   ����:
//
//        �� �Լ��� ���� �ν��Ͻ� �ڵ��� ���� ������ �����ϰ�
//        �� ���α׷� â�� ���� ���� ǥ���մϴ�.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.

	hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 800, 600, nullptr, nullptr, hInstance, nullptr);


	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  �Լ�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ����:  �� â�� �޽����� ó���մϴ�.
//
//  WM_COMMAND  - ���� ���α׷� �޴��� ó���մϴ�.
//  WM_PAINT    - �� â�� �׸��ϴ�.
//  WM_DESTROY  - ���� �޽����� �Խ��ϰ� ��ȯ�մϴ�.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// �޴� ������ ���� �м��մϴ�.
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: ���⿡ hdc�� ����ϴ� �׸��� �ڵ带 �߰��մϴ�.
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_CREATE:
	{
		currentDisplay = eglGetDisplay(GetDC(hWnd));
		eglInitialize(currentDisplay, NULL, NULL);
		eglBindAPI(EGL_OPENVG_API);

		EGLint conf_list[] = { EGL_RED_SIZE, 8,
			EGL_GREEN_SIZE, 8,
			EGL_BLUE_SIZE, 8,
			EGL_ALPHA_SIZE, 8,
			EGL_NONE };
		EGLConfig config;
		EGLint    num_config;
		eglChooseConfig(currentDisplay, conf_list, &config, 1, &num_config);
		currentSurface = eglCreateWindowSurface(currentDisplay, config, hWnd, NULL);
		currentContext = eglCreateContext(currentDisplay, 0, NULL, NULL);
		eglMakeCurrent(currentDisplay, currentSurface, currentSurface, currentContext);

		planeImage = vgLoadImage(TEXT("plane.png"));
		backImage = vgLoadImage(TEXT("background.png"));
		missileImage = vgLoadImage(TEXT("bullet.png"));
		tankImage = vgLoadImage(TEXT("enemy_tank.png"));
		bombImage = vgLoadImage(TEXT("bomb.png"));
		enemyImage= vgLoadImage(TEXT("enemy_plane.png"));

		plane.y = 300;
		plane.x = 30;
		plane.throwing = false;

		srand((unsigned int)time(NULL));

		for (int i = 0; i < 10; i++) {
			missile[i].shoot = false;
			enemy[i].shoot = false;
			tank[i].shoot = false;
		}
	}
	break;

	case WM_KEYDOWN: {
		if (wParam == VK_SPACE) {
			for (int i = 0; i < 100; i++) {
				if (missile[i].shoot == false) {
					missile[i].shoot = true;
					missile[i].x = plane.x + 5;
					missile[i].y = plane.y + 5;
					break;
				}
			}
		}

		if (wParam == 'Z') {
			for (int i = 0; i < 100; i++) {
				if (bomb[i].shoot == false) {
					bomb[i].shoot = true;
					bomb[i].x = plane.x ;
					bomb[i].y = plane.y - 5;
					bomb[i].vy = 10;
					break;
				}
			}
		}
	}break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// ���� ��ȭ ������ �޽��� ó�����Դϴ�.
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

void draw()
{
	eglMakeCurrent(currentDisplay, currentSurface, currentSurface, currentContext);
	vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);

	vgLoadIdentity();
	vgTranslate(k * 800 - cx, 0);
	vgDrawImage(backImage);
	vgLoadIdentity();
	vgTranslate((k + 1) * 800 - cx, 0);
	vgDrawImage(backImage);

	vgLoadIdentity();
	vgTranslate(plane.x, plane.y);
	vgDrawImage(planeImage);

	for (int i = 0; i<100; i++) {
		if (missile[i].shoot) {
			vgLoadIdentity();
			vgTranslate(missile[i].x, missile[i].y);
			vgDrawImage(missileImage);
		}
	}

	for (int i = 0; i<100; i++) {
		if (tank[i].shoot) {
			vgLoadIdentity();
			vgTranslate(tank[i].x, tank[i].y);
			vgDrawImage(tankImage);
		}
	}

	for (int i = 0; i<100; i++) {
		if (bomb[i].shoot) {
			vgLoadIdentity();
			vgTranslate(bomb[i].x, bomb[i].y);
			vgDrawImage(bombImage);
		}
	}

	for (int i = 0; i<100; i++) {
		if (enemy[i].shoot) {
			vgLoadIdentity();
			vgTranslate(enemy[i].x, enemy[i].y);
			vgDrawImage(enemyImage);
		}
	}

	eglSwapBuffers(currentDisplay, currentSurface);
}

void timerproc()
{
	cx += 5;
	k = cx / 800;

	if (KEY_DOWN(VK_UP) && plane.y <= 500) plane.y += 10;
	if (KEY_DOWN(VK_DOWN) && plane.y >= 5) plane.y -= 10;
	if (KEY_DOWN(VK_LEFT) && plane.x >= 5) plane.x -= 10;
	if (KEY_DOWN(VK_RIGHT) && plane.x <= 700) plane.x += 10;

	for (int i = 0; i<100; i++) {
		if (missile[i].shoot) {
			missile[i].x += 20;
				if (missile[i].x>1000) 
					missile[i].shoot = false;
		}
	}

	if (rand() % 60 == 0) {
		for (int i = 0; i < 100; i++) {
			if (tank[i].shoot == false) {
				tank[i].shoot = true;
				tank[i].x = 800;
				tank[i].y = 50;
				break;
			}
		}
	}

	for (int i = 0; i < 100; i++) {
		if (tank[i].shoot) {
			tank[i].x -= 5;
			if (tank[i].x < 0)
				tank[i].shoot = false;
		}
	}


	for (int i = 0; i<100; i++) {
		if (bomb[i].shoot) {
			bomb[i].vy -= 1;
			bomb[i].y += bomb[i].vy;
			if (bomb[i].y<0)
				bomb[i].shoot = false;
		}
	}

	if (rand() % 30 == 0) {
		for (int i = 0; i < 100; i++) {
			if (enemy[i].shoot == false) {
				enemy[i].shoot = true;
				enemy[i].x = 800;
				enemy[i].y = rand() % 500;
				break; 
			}
		}
	}

	for (int i = 0; i < 100; i++) {
		if (enemy[i].shoot) {
			enemy[i].x -= 10;
			if (enemy[i].x < 0)
				enemy[i].shoot = false;
		}
	}

	for (int i = 0; i < 100; i++) {
		for (int j = 0; j < 100; j++) {
			if (enemy[i].shoot) {
				if (missile[j].shoot) {
					if ((enemy[i].x) < (missile[j].x + 30) && (missile[j].x) < (enemy[i].x + 90) && (enemy[i].y) < (missile[j].y + 15) && (missile[j].y) < (enemy[i].y + 60)) {
						missile[j].shoot = false;
						enemy[i].shoot = false;
					}
				}
			}
		}
	}

	for (int i = 0; i < 100; i++) {
		for (int j = 0; j < 100; j++) {
			if (tank[i].shoot) {
				if (bomb[j].shoot) {
					if ((tank[i].x) < (bomb[j].x + 15) && (bomb[j].x) < (tank[i].x + 90) && (tank[i].y) < (bomb[j].y + 30) && (bomb[j].y) < (tank[i].y + 60)) {
						bomb[j].shoot = false;
						tank[i].shoot = false;
					}
				}
			}
		}
	}

	for (int i = 0; i < 100; i++) {
		if (enemy[i].shoot) {
			if ((plane.x) < (enemy[i].x + 90) && (enemy[i].x) < (plane.x + 90) && (plane.y) < (enemy[i].y + 60) && (enemy[i].y) < (plane.y + 60))
				exit(0);
		}
	}

	for (int i = 0; i < 100; i++) {
		if (tank[i].shoot) {
			if ((plane.x) < (tank[i].x + 90) && (tank[i].x) < (plane.x + 90) && (plane.y) < (tank[i].y + 60) && (tank[i].y) < (plane.y + 60))
				exit(0);
		}
	}

	draw();
}




