#pragma once
#include "Wingine.h"
#include "Containers/Vector.h"

struct WindowSettings
{
public:
	const char* Title;
	Vector2<uint16> Position;
	Vector2<uint16> Size;
};

class Window
{
public:
	Window();
	
public:
	bool Initialize(const WindowSettings& InitialSettings);
	void Show() const;
	void Shutdown();
	
	WindowSettings& GetSettings() { return Settings; }
	HWND& GetHandle() { return Handle; }
	HINSTANCE& GetInstance() { return Instance; }
	
private:
	void ComputeWindowTransform(const Vector2<uint16>& InPosition, const Vector2<uint16>& InSize, Vector2<uint16>& OutPosition, Vector2<uint16>& OutSize) const;
	
private:
	WindowSettings Settings;
	HINSTANCE Instance;
	HWND Handle;

	inline static uint32 WindowStyle = (WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MAXIMIZE | WS_MINIMIZEBOX | WS_THICKFRAME);
	inline static uint32 WindowExStyle = (WS_EX_APPWINDOW);
};