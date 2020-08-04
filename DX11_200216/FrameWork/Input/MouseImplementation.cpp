#include "Framework.h"
#include "MouseImplementation.h"

shared_ptr<MouseImplementation> MouseImplementation::_Instance = nullptr;
once_flag MouseImplementation::flag;

MouseImplementation::MouseImplementation()
	: state{}, mouse(nullptr), hwnd(nullptr), mode(Mouse::MouseMode::MODE_ABSOLUTE), LastX(0), LastY(0),
	RelativeX(INT32_MAX), RelativeY(INT32_MAX), isFocus(true)
{
	ScrollWheelValue.reset(CreateEventEx(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, EVENT_MODIFY_STATE | SYNCHRONIZE));
	RelativeRead.reset(CreateEventEx(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, EVENT_MODIFY_STATE | SYNCHRONIZE));
	AbsoluteMode.reset(CreateEventEx(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE));
	RelativeMode.reset(CreateEventEx(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE));
}

MouseImplementation::~MouseImplementation()
{
}

void MouseImplementation::GetState(Mouse::MouseState& state) const
{
	memcpy_s(&state, sizeof(Mouse::MouseState), &this->state, sizeof(Mouse::MouseState));
	state.mode = this->mode;

	DWORD result = WaitForSingleObjectEx(ScrollWheelValue.get(), 0, FALSE);

	if (result == WAIT_OBJECT_0)
	{
		state.ScrollWheelValue = 0;
	}

	if (state.mode == Mouse::MouseMode::MODE_RELATIVE)
	{
		result = WaitForSingleObjectEx(RelativeRead.get(), 0, FALSE);

		if (result == WAIT_OBJECT_0)
		{
			state.x = 0;
			state.y = 0;
		}
		else
		{
			SetEvent(RelativeRead.get());
		}
	}
}

void MouseImplementation::ResetScrollWheelValue()
{
	SetEvent(ScrollWheelValue.get());
}

void MouseImplementation::SetMode(Mouse::MouseMode mode)
{
	if (this->mode == mode)
		return;

	SetEvent((mode == Mouse::MODE_ABSOLUTE) ? AbsoluteMode.get() : RelativeMode.get());

	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(tme);
	tme.dwFlags = TME_HOVER;
	tme.hwndTrack = hwnd;
	tme.dwHoverTime = 1;
	if (!TrackMouseEvent(&tme))
	{
		return;
	}
}

bool MouseImplementation::IsConnected() const
{
	return GetSystemMetrics(SM_MOUSEPRESENT) != 0;
}

bool MouseImplementation::IsVisible() const
{
	if (mode == Mouse::MODE_RELATIVE)
	{
		return false;
	}

	CURSORINFO info = { sizeof(CURSORINFO), 0, nullptr, {} };
	if (!GetCursorInfo(&info))
	{
		return false;
	}

	return (info.flags & CURSOR_SHOWING) != 0;
}

void MouseImplementation::SetVisible(bool visible)
{
	if (mode == Mouse::MODE_RELATIVE)
	{
		return;
	}

	CURSORINFO info = { sizeof(CURSORINFO), 0, nullptr, {} };
	if (!GetCursorInfo(&info))
	{
		return;
	}

	bool isvisible = (info.flags & CURSOR_SHOWING) != 0;

	if (isvisible != visible)
		ShowCursor(visible);
}

void MouseImplementation::SetWindow(HWND hwnd)
{
	if (this->hwnd == hwnd)
		return;

	RAWINPUTDEVICE Rid;
	Rid.usUsagePage = 0x1;
	Rid.usUsage = 0x2;
	Rid.dwFlags = RIDEV_INPUTSINK;
	Rid.hwndTarget = hwnd;
	if (!RegisterRawInputDevices(&Rid, 1, sizeof(RAWINPUTDEVICE)))
	{
		return;
	}

	this->hwnd = hwnd;
}

void MouseImplementation::ClipToWindow()
{
	RECT rect;
	GetClientRect(hwnd, &rect);

	POINT ul;
	ul.x = rect.left;
	ul.y = rect.top;

	POINT lr;
	lr.x = rect.right;
	lr.y = rect.bottom;

	MapWindowPoints(hwnd, nullptr, &ul, 1);
	MapWindowPoints(hwnd, nullptr, &lr, 1);

	rect.left = ul.x;
	rect.top = ul.y;

	rect.right = lr.x;
	rect.bottom = lr.y;

	ClipCursor(&rect);
}
