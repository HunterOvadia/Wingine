#pragma once
#include "Wingine.h"
#include "Containers/Vector2.h"

struct WindowArgs
{
public:
	const char* WindowTitle;
	Vector2<uint32> InitialPosition;
	Vector2<uint32> InitialSize;
	uint32 WindowStyle = (WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MAXIMIZE | WS_MINIMIZEBOX | WS_THICKFRAME);
	uint32 WindowExStyle = (WS_EX_APPWINDOW);
};

class Window
{
public:
	Window() = delete;
	explicit Window(const WindowArgs& InArgs);
	
public:
	bool Initialize();
	void Show() const;
	void Shutdown();

private:
	void ComputeWindowTransform(const Vector2<uint32>& InPosition, const Vector2<uint32>& InSize, Vector2<uint32>& OutPosition, Vector2<uint32>& OutSize) const;
	
private:
	WindowArgs Args;
	HINSTANCE Instance;
	HWND Handle;
};