#include "std.h"
#include "resource.h"

#include "CGame.h"

#define WWIDTH 950
#define WHEIGHT 720

LRESULT WINAPI WndProc(HWND p_hwnd,UINT p_msg,WPARAM p_wParam,LPARAM p_lParam);

int WINAPI WinMain(HINSTANCE ip_hInstance,HINSTANCE,LPSTR,int)
{
	WNDCLASS wc;
	MSG msg;
	HWND hwnd;
	wc.cbClsExtra=0;
	wc.cbWndExtra=0;
	wc.hbrBackground=(HBRUSH)1;
	wc.hCursor=LoadCursor(NULL,IDC_ARROW);
	wc.hIcon=LoadIcon(ip_hInstance,MAKEINTRESOURCE(IDI_ICON1));
	wc.hInstance=ip_hInstance;
	wc.lpfnWndProc=WndProc;
	wc.lpszClassName="Assembler";
	wc.lpszMenuName=NULL;
	wc.style=0;
	RegisterClass(&wc);
	hwnd=CreateWindow("Assembler","Assembler",WS_OVERLAPPEDWINDOW,100,100,WWIDTH,WHEIGHT,NULL,NULL,ip_hInstance,NULL);
	try{
		CGame::init(hwnd);
	}catch(EError eerror){
		MessageBox(hwnd,eerror.text,"Hiba",MB_ICONERROR);
		exit(1);
	}
	ShowWindow(hwnd,SW_SHOW);
	UpdateWindow(hwnd);

	ZeroMemory(&msg,sizeof(MSG));

	while(msg.message!=WM_QUIT){
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
			TranslateMessage(&msg);
			DispatchMessage(&msg);	
		}else{
			try{
				CGame::instance()->update();
				CGame::instance()->draw();
			}catch(EError &eerror){
				MessageBox(hwnd,eerror.text,"Hiba",MB_ICONERROR);
				exit(1);
			}
		}
	}
	return (int)msg.wParam;
}

LRESULT WINAPI WndProc(HWND p_hwnd,UINT p_msg,WPARAM p_wParam,LPARAM p_lParam)
{
	int x,y;
	switch(p_msg){
		case WM_CLOSE:
			if(MessageBox(p_hwnd,"Biztos benne, hogy kilép?","Kilépési szándék megerõsítése",MB_YESNO)==IDYES)
				exit(0);
			break;
		case WM_DESTROY:
				PostQuitMessage(0);
			break;
		case WM_KEYDOWN:
			if(p_wParam==VK_ESCAPE)
				PostMessage(p_hwnd,WM_CLOSE,0,0);
			break;
		case WM_LBUTTONDOWN:
			x=LOWORD(p_lParam);
			y=HIWORD(p_lParam);
			try{
				CGame::instance()->getMessageHandler()->messageDown(x,y);
			}catch(EError &eerror){
				MessageBox(p_hwnd,eerror.text,"Hiba",MB_ICONERROR);
				exit(1);
			}
			break;
		case WM_MOUSEMOVE:
			x=LOWORD(p_lParam);
			y=HIWORD(p_lParam);
			try{
				CGame::instance()->getMessageHandler()->messageMove(x,y,((p_wParam & MK_LBUTTON) == 0 ? 0 : 1 ));
			}catch(EError &eerror){
				MessageBox(p_hwnd,eerror.text,"Hiba",MB_ICONERROR);
				exit(1);
			}
			break;
		case WM_LBUTTONUP:
			x=LOWORD(p_lParam);
			y=HIWORD(p_lParam);
			try{
				CGame::instance()->getMessageHandler()->messageUp(x,y);
			}catch(EError &eerror){
				MessageBox(p_hwnd,eerror.text,"Hiba",MB_ICONERROR);
				exit(1);
			}
			break;
		default:
			if(!((p_msg==WM_SYSCOMMAND)&&((p_wParam&0xfff0)==SC_SIZE ||(p_wParam&0xfff0)==SC_MAXIMIZE)))
				return DefWindowProc(p_hwnd,p_msg,p_wParam,p_lParam);
	}
	return 0;
}