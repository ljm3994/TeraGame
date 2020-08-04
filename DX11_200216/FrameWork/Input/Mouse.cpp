#include "Framework.h"
#include "Mouse.h"
#include "Framework/Input/MouseImplementation.h"

shared_ptr<Mouse> Mouse::_Instance = nullptr;
once_flag Mouse::flag;

#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
void __cdecl Mouse::SetWindow(HWND hwnd)
{
	MouseImplementation::Instance()->SetWindow(hwnd);
}

void __cdecl Mouse::ProcessMsg(UINT msg, WPARAM wparam, LPARAM lparam)
{
	auto Tempmouse = MouseImplementation::Instance();

	if (!Tempmouse)
		return;

	HANDLE Events[3];
	Events[0] = Tempmouse->ScrollWheelValue.get();
	Events[1] = Tempmouse->AbsoluteMode.get();
	Events[2] = Tempmouse->RelativeMode.get();
	switch (WaitForMultipleObjectsEx(_countof(Events), Events, FALSE, 0, FALSE))
	{
	case WAIT_OBJECT_0:
		Tempmouse->state.ScrollWheelValue = 0;
		ResetEvent(Events[0]);
		break;
	case (WAIT_OBJECT_0 + 1):
	{
		Tempmouse->mode = MODE_ABSOLUTE;
		ClipCursor(nullptr);

		POINT point;
		point.x = Tempmouse->LastX;
		point.y = Tempmouse->LastY;

		// We show the cursor before moving it to support Remote Desktop
		ShowCursor(TRUE);

		if (MapWindowPoints(Tempmouse->hwnd, nullptr, &point, 1))
		{
			SetCursorPos(point.x, point.y);
		}
		Tempmouse->state.x = Tempmouse->LastX;
		Tempmouse->state.y = Tempmouse->LastY;
	}
	break;
	case (WAIT_OBJECT_0 + 2):
	{
		ResetEvent(Tempmouse->RelativeRead.get());

		Tempmouse->mode = MODE_RELATIVE;
		Tempmouse->state.x = Tempmouse->state.y = 0;
		Tempmouse->RelativeX = INT32_MAX;
		Tempmouse->RelativeY = INT32_MAX;

		ShowCursor(FALSE);

		Tempmouse->ClipToWindow();
	}
	case WAIT_FAILED:
		throw exception("WaitForMultipleObjectsEx");
	}

	switch (msg)
	{
	case WM_ACTIVATEAPP:
		if (wparam)
		{
			Tempmouse->isFocus = true;

			if (Tempmouse->mode == MODE_RELATIVE)
			{
				Tempmouse->state.x = Tempmouse->state.y = 0;

				ShowCursor(FALSE);

				Tempmouse->ClipToWindow();
			}
		}
		else
		{
			int scrollWheel = Tempmouse->state.ScrollWheelValue;
			memset(&Tempmouse->state, 0, sizeof(State));
			Tempmouse->state.ScrollWheelValue = scrollWheel;

			Tempmouse->isFocus = false;
		}
		return;
	case WM_INPUT:
		if (Tempmouse->isFocus && Tempmouse->mode == MODE_RELATIVE)
		{
			RAWINPUT raw;
			UINT rawSize = sizeof(raw);

			UINT resultData = GetRawInputData(reinterpret_cast<HRAWINPUT>(lparam), RID_INPUT, &raw, &rawSize, sizeof(RAWINPUTHEADER));
			if (resultData == UINT(-1))
			{
				throw std::exception("GetRawInputData");
			}

			if (raw.header.dwType == RIM_TYPEMOUSE)
			{
				if (!(raw.data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE))
				{
					Tempmouse->state.x = raw.data.mouse.lLastX;
					Tempmouse->state.y = raw.data.mouse.lLastY;

					ResetEvent(Tempmouse->RelativeRead.get());
				}
				else if (raw.data.mouse.usFlags & MOUSE_VIRTUAL_DESKTOP)
				{
					// This is used to make Remote Desktop sessons work
					const int width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
					const int height = GetSystemMetrics(SM_CYVIRTUALSCREEN);

					int x = static_cast<int>((float(raw.data.mouse.lLastX) / 65535.0f) * width);
					int y = static_cast<int>((float(raw.data.mouse.lLastY) / 65535.0f) * height);

					if (Tempmouse->RelativeX == INT32_MAX)
					{
						Tempmouse->state.x = Tempmouse->state.y = 0;
					}
					else
					{
						Tempmouse->state.x = x - Tempmouse->RelativeX;
						Tempmouse->state.y = y - Tempmouse->RelativeY;
					}

					Tempmouse->RelativeX = x;
					Tempmouse->RelativeY = y;

					ResetEvent(Tempmouse->RelativeRead.get());
				}
			}
		}
		return;
	case WM_MOUSEMOVE:
		break;

	case WM_LBUTTONDOWN:
		Tempmouse->state.IsleftButton = true;
		break;

	case WM_LBUTTONUP:
		Tempmouse->state.IsleftButton = false;
		break;

	case WM_RBUTTONDOWN:
		Tempmouse->state.IsrightButton = true;
		break;

	case WM_RBUTTONUP:
		Tempmouse->state.IsrightButton = false;
		break;

	case WM_MBUTTONDOWN:
		Tempmouse->state.IsmiddleButton = true;
		break;

	case WM_MBUTTONUP:
		Tempmouse->state.IsmiddleButton = false;
		break;

	case WM_MOUSEWHEEL:
		Tempmouse->state.ScrollWheelValue += GET_WHEEL_DELTA_WPARAM(wparam);
		return;
	case WM_XBUTTONDOWN:
		switch (GET_XBUTTON_WPARAM(wparam))
		{
		case XBUTTON1:
			Tempmouse->state.IsxButton1 = true;
			break;

		case XBUTTON2:
			Tempmouse->state.IsxButton2 = true;
			break;
		}
		break;

	case WM_XBUTTONUP:
		switch (GET_XBUTTON_WPARAM(wparam))
		{
		case XBUTTON1:
			Tempmouse->state.IsxButton1 = false;
			break;

		case XBUTTON2:
			Tempmouse->state.IsxButton2 = false;
			break;
		}
		break;
	case WM_MOUSEHOVER:
		break;
	default:
		return;
	}

	if (Tempmouse->mode == MODE_ABSOLUTE)
	{
		// All mouse messages provide a new pointer position
		int xPos = static_cast<short>(LOWORD(lparam));
		int yPos = static_cast<short>(HIWORD(lparam));

		Tempmouse->state.x = Tempmouse->LastX = xPos;
		Tempmouse->state.y = Tempmouse->LastY = yPos;
	}
}
#endif

#pragma warning( disable : 4355 )
Mouse::Mouse()
{
	MouseImplementation::Instance()->mouse = this;
}

Mouse::~Mouse()
{
}

Mouse::MouseState __cdecl Mouse::GetState() const
{
	MouseState state;
	MouseImplementation::Instance()->GetState(state);
	return state;
}

void __cdecl Mouse::ResetScrollWheelValue()
{
	MouseImplementation::Instance()->ResetScrollWheelValue();
}

void __cdecl Mouse::SetMode(MouseMode mode)
{
	MouseImplementation::Instance()->SetMode(mode);
}

bool __cdecl Mouse::isConnected() const
{
	return MouseImplementation::Instance()->IsConnected();
}

bool __cdecl Mouse::isVisible() const
{
	return MouseImplementation::Instance()->IsVisible();
}

void __cdecl Mouse::SetVisible(bool visible)
{
	MouseImplementation::Instance()->SetVisible(visible);
}

////////////////////////////////////////////////////////////////////////////////////////
//버튼 이벤트 함수

#define UPDATE_BUTTON_STATE(field) field = static_cast<EventType>( ( !!state.field ) | ( ( !!state.field ^ !!LastState.field ) << 1 ) );
void __cdecl Mouse::MouseEvent::Update(const MouseState& state)
{
	UPDATE_BUTTON_STATE(IsleftButton);

	assert((!state.IsleftButton && !LastState.IsleftButton) == (IsleftButton == UP));
	assert((state.IsleftButton && LastState.IsleftButton) == (IsleftButton == HELD));
	assert((!state.IsleftButton && LastState.IsleftButton) == (IsleftButton == RELEASED));
	assert((state.IsleftButton && !LastState.IsleftButton) == (IsleftButton == PRESSED));

	UPDATE_BUTTON_STATE(IsmiddleButton);
	UPDATE_BUTTON_STATE(IsrightButton);
	UPDATE_BUTTON_STATE(IsxButton1);
	UPDATE_BUTTON_STATE(IsxButton2);

	LastState = state;
}

void __cdecl Mouse::MouseEvent::Reset() noexcept
{
	memset(this, 0, sizeof(MouseEvent));
}
