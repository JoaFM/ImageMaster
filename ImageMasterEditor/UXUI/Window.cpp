
#include "Window.h"
#include <stdio.h>

#include <windowsx.h>
#include "Utils/IM_Math.h"

#include "External/IMGUI/imgui_impl_win32.h"

#include <dwmapi.h>
#include <intsafe.h>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


LRESULT CALLBACK WindowProcR(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
		return true;

	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


Window::Window(HINSTANCE hInstanceParam)
	:
	m_hInstance(hInstanceParam),
	m_hwnd(nullptr)
{
}

void Window::Init(UINT32 Width, UINT32 Height)
{
	const wchar_t CLASS_NAME[] = L"Sample Window Class";

	WNDCLASS wc = { };

	wc.lpfnWndProc = WindowProcR;
	wc.hInstance = m_hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	// Create the window
	{
		RECT		WindowRect;
		WindowRect.left = (long)0;			// Set Left Value To 0
		WindowRect.right = (long)Width;		// Set Right Value To Requested Width
		WindowRect.top = (long)0;				// Set Top Value To 0
		WindowRect.bottom = (long)Height;		// Set Bottom Value To Requested Height
		AdjustWindowRectEx(&WindowRect, WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX, FALSE, NULL);

		//WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_BORDER,            // Window style

		m_hwnd = CreateWindowEx(
			0,                              // Optional window styles.
			CLASS_NAME,                     // Window class
			L"Image Master",    // Window text
			WS_OVERLAPPEDWINDOW,            // Window style

			// Size and position
			CW_USEDEFAULT, CW_USEDEFAULT,
			WindowRect.right - WindowRect.left,
			WindowRect.bottom - WindowRect.top,

			NULL,       // Parent window    
			NULL,       // Menu
			m_hInstance,  // Instance handle
			NULL        // Additional application data
		);
	}

	if (GetHWND() == NULL)
	{

		//Get the error message ID, if any.
		DWORD errorMessageID = ::GetLastError();
		if (errorMessageID == 0) {
			return; //No error message has been recorded
		}

		LPSTR messageBuffer = nullptr;

		//Ask Win32 to give us the string version of that message ID.
		//The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
		size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

		//Copy the error message into a std::string.
		std::string message(messageBuffer, size);

		//Free the Win32's string's buffer.
		LocalFree(messageBuffer);
	}

	COLORREF DARK_COLOR = 0x00303030;
	BOOL SET_CAPTION_COLOR = SUCCEEDED(DwmSetWindowAttribute(
		m_hwnd, DWMWINDOWATTRIBUTE::DWMWA_CAPTION_COLOR,
		&DARK_COLOR, sizeof(DARK_COLOR)));

	ShowTheWindow();
}

HWND Window::GetHWND()
{
	return m_hwnd;
}

HINSTANCE Window::GetHINSTANCE()
{
	return m_hInstance;
}

bool Window::GetTheWindowRect(RECT* WindowsRect)
{
	return GetWindowRect(m_hwnd, WindowsRect);
}

void Window::SetTitle(std::string NewTitle)
{
	SetWindowTextA(m_hwnd, NewTitle.c_str());
}

void Window::ShowTheWindow()
{
	ShowWindow(m_hwnd, SW_SHOW);
}

void Window::OnMouseMove(LPARAM lParam)
{
	m_MouseX = (INT32)GET_X_LPARAM(lParam);
	m_MouseY = (INT32)GET_Y_LPARAM(lParam);
}

void Window::OnLButtonDown(LPARAM lParam)
{
	m_Mouse_LB_OnDown = true;
}
void Window::OnRButtonDown(LPARAM lParam)
{
	m_Mouse_RB_OnDown = true;
}

void Window::OnLButtonUp(LPARAM lParam)
{
	m_Mouse_LB_OnUp = true;
	m_Mouse_LB_OnDown = false;
	m_Mouse_LB_IsDown = false;
}

void Window::OnRButtonUp(LPARAM lParam)
{
	m_Mouse_RB_OnUp = true;
	m_Mouse_RB_OnDown = false;
	m_Mouse_RB_IsDown = false;
}


void Window::OnMButtonDown(LPARAM lParam)
{
	m_Mouse_MB_OnDown = true;
}

void Window::OnMButtonUp(LPARAM lParam)
{
	m_Mouse_MB_OnUp = true;
	m_Mouse_MB_OnDown = false;
	m_Mouse_MB_IsDown = false;
}

INT32 Window::MouseScroll() const
{
	return m_MouseWeelState;
}

void Window::OnMouseWeel(LPARAM lParam, WPARAM wParam)
{
	m_MouseWeelState = GET_WHEEL_DELTA_WPARAM(wParam);
}

void Window::OnKeyAction(LPARAM lParam, WPARAM wParam, bool IsDown)
{
	//https://docs.microsoft.com/en-us/windows/win32/inputdev/about-keyboard-input#keystroke-message-flags

	WORD vkCode = LOWORD(wParam);                                       // virtual-key code

	BYTE scanCode = LOBYTE(HIWORD(lParam));                             // scan code
	BOOL scanCodeE0 = (HIWORD(lParam) & KF_EXTENDED) == KF_EXTENDED;    // extended-key flag, 1 if scancode has 0xE0 prefix

	BOOL upFlag = (HIWORD(lParam) & KF_UP) == KF_UP;                    // transition-state flag, 1 on keyup
	BOOL repeatFlag = (HIWORD(lParam) & KF_REPEAT) == KF_REPEAT;        // previous key-state flag, 1 on autorepeat
	WORD repeatCount = LOWORD(lParam);                                  // repeat count, > 0 if several keydown messages was combined into one message

	BOOL altDownFlag = (HIWORD(lParam) & KF_ALTDOWN) == KF_ALTDOWN;     // ALT key was pressed

	BOOL dlgModeFlag = (HIWORD(lParam) & KF_DLGMODE) == KF_DLGMODE;     // dialog box is active
	BOOL menuModeFlag = (HIWORD(lParam) & KF_MENUMODE) == KF_MENUMODE;  // menu is active


	auto it = m_CurrentKeyState.find(vkCode);

	if (it == m_CurrentKeyState.end())
	{
		m_CurrentKeyState[vkCode] = KeyState(vkCode, KeyStatus::NONE, altDownFlag);
	}

	{
		if (IsDown)
		{
			if (m_CurrentKeyState[vkCode].keySatus == KeyStatus::OnDown ||
				m_CurrentKeyState[vkCode].keySatus == KeyStatus::IsDown
				)
			{
				m_CurrentKeyState[vkCode].keySatus = KeyStatus::IsDown;
			}
			else
			{
				m_CurrentKeyState[vkCode].keySatus = KeyStatus::OnDown;
			}
		}
		else
		{
			m_CurrentKeyState[vkCode].keySatus = KeyStatus::OnUp;
		}
	}
}

void Window::PumpWindowMessages()
{
	m_MouseWeelState = 0;
	for (auto& keyS : m_CurrentKeyState)
	{
		if (keyS.second.keySatus == KeyStatus::OnUp)
		{
			keyS.second.keySatus = KeyStatus::NONE;
		}
		else if (keyS.second.keySatus == KeyStatus::OnDown)
		{
			keyS.second.keySatus = KeyStatus::IsDown;
		}
	}

	if (m_Mouse_LB_OnDown)
	{
		m_Mouse_LB_OnDown = false;
		m_Mouse_LB_OnUp = false;
		m_Mouse_LB_IsDown = true;
	}

	if (m_Mouse_LB_OnUp)
	{
		m_Mouse_LB_OnDown = false;
		m_Mouse_LB_OnUp = false;
		m_Mouse_LB_IsDown = false;
	}

	if (m_Mouse_RB_OnDown)
	{
		m_Mouse_RB_OnDown = false;
		m_Mouse_RB_IsDown = true;
		m_Mouse_RB_OnUp = false;

	}

	if (m_Mouse_RB_OnUp)
	{
		m_Mouse_RB_IsDown = false;
		m_Mouse_RB_OnUp = false;
		m_Mouse_RB_OnDown = false;
	}


	if (m_Mouse_MB_OnDown)
	{
		m_Mouse_MB_OnDown = false;
		m_Mouse_MB_IsDown = true;
		m_Mouse_MB_OnUp = false;

	}

	if (m_Mouse_MB_OnUp)
	{
		m_Mouse_MB_IsDown = false;
		m_Mouse_MB_OnUp = false;
		m_Mouse_MB_OnDown = false;
	}


	MSG msg = {};
	bool should_close = false;
	bool HasBeenAResizeThisFrame = false;

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		switch (msg.message)
		{
		case WM_MOUSEMOVE:
			OnMouseMove(msg.lParam);
			break;
		case WM_MBUTTONDOWN:
			OnMButtonDown(msg.lParam);
			break;
		case WM_MBUTTONUP:
			OnMButtonUp(msg.lParam);
			break;
		case WM_LBUTTONDOWN:
			OnLButtonDown(msg.lParam);
			break;
		case WM_LBUTTONUP:
			OnLButtonUp(msg.lParam);
			break;
		case WM_RBUTTONDOWN:
			OnRButtonDown(msg.lParam);
			break;
		case WM_RBUTTONUP:
			OnRButtonUp(msg.lParam);
			break;
		case WM_KEYDOWN:
			OnKeyAction(msg.lParam, msg.wParam, true);
			break;
		case WM_MOUSEWHEEL:
			OnMouseWeel(msg.lParam, msg.wParam);
			break;
		case WM_KEYUP:
			OnKeyAction(msg.lParam, msg.wParam, false);
			break;
		default:
			break;
		}
	}
	if (msg.message == WM_QUIT)
	{
		m_WaitingToQuit = true;
	}

}

IM_Math::Int2 Window::GetScreenMousePosPixels() const
{
	POINT MousePos;
	bool suc = GetCursorPos(&MousePos);
	return IM_Math::Int2((INT32)MousePos.x, (INT32)MousePos.y);
}

bool Window::IsKeyDown(KeyCode Key)const
{
	auto it = m_CurrentKeyState.find((INT32)Key);
	if (it != m_CurrentKeyState.end() && (it->second.keySatus == KeyStatus::IsDown || it->second.keySatus == KeyStatus::OnDown))
	{
		return true;
	}
	return false;
}

bool Window::OnKeyDown(KeyCode Key)const
{
	auto it = m_CurrentKeyState.find((INT32)Key);

	if (it != m_CurrentKeyState.end() && it->second.keySatus == KeyStatus::OnDown)
	{
		return true;
	}
	return false;
}

bool Window::OnKeyUp(KeyCode Key)const
{
	auto it = m_CurrentKeyState.find((INT32)Key);

	if (it != m_CurrentKeyState.end() && it->second.keySatus == KeyStatus::OnUp)
	{
		return true;
	}
	return false;
}

bool Window::IsMouseDown(INT32 Index)const
{
	if (Index == 0)
	{
		return m_Mouse_LB_IsDown;
	}
	else if (Index == 1)
	{
		return m_Mouse_RB_IsDown;
	}
	else if (Index == 2)
	{
		return m_Mouse_RB_IsDown;
	}
	else
	{
		return false;
	}
}

bool Window::OnMouseDown(INT32 Index)const
{
	if (Index == 0)
	{
		return m_Mouse_LB_OnDown;
	}
	else if (Index == 1)
	{
		return m_Mouse_RB_OnDown;
	}
	else if (Index == 2)
	{
		return m_Mouse_MB_OnDown;
	}
	else
	{
		return false;
	}
}

bool Window::OnMouseUp(INT32 Index) const
{
	if (Index == 0)
	{
		return m_Mouse_LB_OnUp;
	}
	else if (Index == 1)
	{
		return m_Mouse_RB_OnUp;
	}
	else if (Index == 2)
	{
		return m_Mouse_MB_OnUp;
	}
	else
	{
		return false;
	}
}

bool Window::ResetMouseToCenter()
{
	RECT WinRec;
	GetWindowRect(m_hwnd, &WinRec);

	LONG WindowCenterX = (WinRec.left + WinRec.right) / 2;
	LONG WindowCenterY = (WinRec.top + WinRec.bottom) / 2;

	return SetCursorPos(WindowCenterX, WindowCenterY);
}

void Window::ResizeSwapChainToWindow()
{
	//m_SwapChain->ResizeToWindow();
}

INT32 Window::GetSwapChainWidth()
{
	// return m_SwapChain->GetWidth();
	return 0;
}

INT32 Window::GetSwapChainHeight()
{
	//return m_SwapChain->GetHeight();
	return 0;
}

bool Window::DoneAndWaitingToQuit() const
{
	return !m_WaitingToQuit;
}

void Window::Present(UINT SyncInterval, UINT Flags)
{
	//m_SwapChain->Present(SyncInterval, Flags);
}

void Window::SetSwapChain(SwapChain* WindowsSwapchain)
{
	//m_SwapChain = WindowsSwapchain;  // TODO Make this a unique ptr
   // m_SwapChain->SetWindow(this);
}
