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
	}
	
	return DefWindowProcA(Handle, Message, WParam, LParam);
}

Window::Window(const WindowArgs& InArgs)
	: Args(InArgs)
	, Handle(nullptr)
{
	Instance = GetModuleHandleA(nullptr);
}

Window::~Window()
{
	Shutdown();
}

bool Window::Initialize()
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
		.hbrBackground = nullptr,
		.lpszClassName = "wingine_window_class"
	};

	if (!RegisterClassA(&WindowClass))
	{
		WIN_LOG(Fatal, "Failed to Register Window Class!");
		return false;
	}

	Vector2<uint32> WindowPosition, WindowSize;
	ComputeWindowTransform(Args.InitialPosition, Args.InitialSize, WindowPosition, WindowSize);

	Handle = CreateWindowExA(Args.WindowExStyle, WindowClass.lpszClassName, Args.WindowTitle, Args.WindowStyle, WindowPosition.X, WindowPosition.Y, WindowSize.X, WindowSize.Y, nullptr, nullptr, Instance, nullptr);
	if (!Handle)
	{
		WIN_LOG(Fatal, "Failed to Create Window!");
		return false;
	}

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

void Window::ComputeWindowTransform(const Vector2<uint32>& InPosition, const Vector2<uint32>& InSize, Vector2<uint32>& OutPosition, Vector2<uint32>& OutSize) const
{
	OutPosition = InPosition;
	OutSize = InSize;

	RECT BorderRect = { 0, 0, 0, 0 };
	AdjustWindowRectEx(&BorderRect, Args.WindowStyle, 0, Args.WindowExStyle);

	OutPosition.X += BorderRect.left;
	OutPosition.Y += BorderRect.top;
	OutSize.X += (BorderRect.right - BorderRect.left);
	OutSize.Y += (BorderRect.bottom - BorderRect.top);
}
