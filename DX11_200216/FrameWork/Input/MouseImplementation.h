#pragma once
#include "Framework/Input/Mouse.h"


class MouseImplementation
{
private:
	friend void Mouse::ProcessMsg(UINT msg, WPARAM wparam, LPARAM lparam);

	HWND hwnd;
	Mouse::MouseMode mode;

	ScopedHandle ScrollWheelValue;
	ScopedHandle RelativeRead;
	ScopedHandle AbsoluteMode;
	ScopedHandle RelativeMode;

	int LastX;
	int LastY;
	int RelativeX;
	int RelativeY;

	bool isFocus;

	static shared_ptr<MouseImplementation> _Instance;
	static once_flag flag;
public:
	static MouseImplementation* Instance()
	{
		call_once(flag, [] {
			_Instance = make_shared<MouseImplementation>();
			});

		return _Instance.get();
	}
	Mouse::MouseState state;
	Mouse* mouse;


	MouseImplementation();
	~MouseImplementation();

	void GetState(Mouse::MouseState& state) const;
	void ResetScrollWheelValue();

	void SetMode(Mouse::MouseMode mode);
	bool IsConnected() const;
	bool IsVisible() const;
	void SetVisible(bool visible);
	void SetWindow(HWND hwnd);
private:
	void ClipToWindow();
};