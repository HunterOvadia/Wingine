#include "Core/Window.h"
#include "Core/Logger.h"

static LRESULT CALLBACK ProcessMessages(HWND Handle, uint32 Message, WPARAM WParam, LPARAM LParam)
{
	switch(Message)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
		}
		break;

		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		{
		}
		break;
	}
	
	return DefWindowProcA(Handle, Message, WParam, LParam);
}

Window::Window()
	: Handle(nullptr)
{
	Instance = GetModuleHandleA(nullptr);
}

bool Window::Initialize(const WindowSettings& InitialSettings)
{
	const WNDCLASSA WindowClass =
	{
		.style = CS_VREDRAW | CS_HREDRAW,
		.lpfnWndProc = ProcessMessages,
		.cbClsExtra = 0,
		.cbWndExtra = 0,
		.hInstance = Instance, 
		.hIcon = LoadIcon(Instance, IDI_APPLICATION),
		.hCursor = LoadCursor(nullptr, IDC_ARROW),
		.hbrBackground = NULL,
		.lpszClassName = "wingine_window_class"
	};

	if (!RegisterClassA(&WindowClass))
	{
		WIN_LOG(Fatal, "Failed to Register Window Class!");
		return false;
	}

	Vector2<uint16> WindowPosition, WindowSize;
	ComputeWindowTransform(InitialSettings.Position, InitialSettings.Size, WindowPosition, WindowSize);
	Handle = CreateWindowExA(WindowExStyle, WindowClass.lpszClassName, InitialSettings.Title, WindowStyle, WindowPosition.X, WindowPosition.Y, WindowSize.X, WindowSize.Y, nullptr, nullptr, Instance, nullptr);
	if (!Handle)
	{
		WIN_LOG(Fatal, "Failed to Create Window!");
		return false;
	}

	Settings.Title = InitialSettings.Title;
	Settings.Position = WindowPosition;
	Settings.Size = WindowSize;
	return true;
}

void Window::Show() const
{
	ShowWindow(Handle, SW_SHOW);
}

void Window::Shutdown()
{
	if(Handle)
	{
		DestroyWindow(Handle);
		Handle = nullptr;
	}
}

void Window::ComputeWindowTransform(const Vector2<uint16>& InPosition, const Vector2<uint16>& InSize, Vector2<uint16>& OutPosition, Vector2<uint16>& OutSize) const
{
	OutPosition = InPosition;
	OutSize = InSize;

	RECT BorderRect = { 0, 0, 0, 0 };
	AdjustWindowRectEx(&BorderRect, WindowStyle, 0, WindowExStyle);

	OutPosition.X += (uint16)BorderRect.left;
	OutPosition.Y += (uint16)BorderRect.top;
	OutSize.X += (uint16)(BorderRect.right - BorderRect.left);
	OutSize.Y += (uint16)(BorderRect.bottom - BorderRect.top);
}
