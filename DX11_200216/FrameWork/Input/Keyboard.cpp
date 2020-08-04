#include "Framework.h"
#include "Keyboard.h"
#include "KeyboardImplementation.h"

shared_ptr<Keyboard> Keyboard::_Instance = nullptr;
once_flag Keyboard::flag;

namespace
{
	void KeyDown(int key, KeyboardState& state)
	{
		if (key < 0 || key > 0xfe)
			return;

		auto ptr = reinterpret_cast<uint32_t*>(&state);

		unsigned int bf = 1u << (key & 0x1f);
		ptr[(key >> 5)] |= bf;
	}

	void KeyUp(int key, KeyboardState& state)
	{
		if (key < 0 || key > 0xfe)
			return;

		auto ptr = reinterpret_cast<uint32_t*>(&state);

		unsigned int bf = 1u << (key & 0x1f);
		ptr[(key >> 5)] &= ~bf;
	}
}

#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
void __cdecl Keyboard::ProcessMsg(UINT msg, WPARAM wparam, LPARAM lparam)
{
	auto tempkeyboard = KeyboardImplementation::Instance();

	if (!tempkeyboard)
		return;

	bool down = false;

	switch (msg)
	{
	case WM_ACTIVATEAPP:
		tempkeyboard->Reset();
		return;

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		down = true;
		break;

	case WM_KEYUP:
	case WM_SYSKEYUP:
		break;

	default:
		return;
	}

	int vk = static_cast<int>(wparam);
	switch (vk)
	{
	case VK_SHIFT:
		vk = MapVirtualKey((lparam & 0x00ff0000) >> 16, MAPVK_VSC_TO_VK_EX);
		if (!down)
		{
			KeyUp(VK_LSHIFT, tempkeyboard->state);
			KeyUp(VK_RSHIFT, tempkeyboard->state);
		}
		break;

	case VK_CONTROL:
		vk = (lparam & 0x01000000) ? VK_RCONTROL : VK_LCONTROL;
		break;

	case VK_MENU:
		vk = (lparam & 0x01000000) ? VK_RMENU : VK_LMENU;
		break;
	}

	if (down)
	{
		KeyDown(vk, tempkeyboard->state);
	}
	else
	{
		KeyUp(vk, tempkeyboard->state);
	}
}
#endif

Keyboard::Keyboard()
	: Status(KeyboardStatus::NONE), Key(0u)
{
	KeyboardImplementation::Instance()->keyboard = this;
}

Keyboard::~Keyboard()
{
}

KeyboardState __cdecl Keyboard::GetState() const
{
	KeyboardState state;
	KeyboardImplementation::Instance()->GetState(state);
	return state;
}

void __cdecl Keyboard::Reset()
{
	KeyboardImplementation::Instance()->Reset();
}

bool __cdecl Keyboard::IsConnected() const
{
	return KeyboardImplementation::Instance()->IsConnected();
}

///////////////////////////////////////////////////////////////////////////
//Keyboard¿Ã∫•∆Æ

Keyboard::KeyboardEvent::KeyboardEvent() noexcept
{
	Reset();
}

void __cdecl Keyboard::KeyboardEvent::Update(const KeyboardState& State)
{
	auto currPtr = reinterpret_cast<const uint32_t*>(&State);
	auto prevPtr = reinterpret_cast<const uint32_t*>(&LastState);
	auto releasedPtr = reinterpret_cast<uint32_t*>(&released);
	auto pressedPtr = reinterpret_cast<uint32_t*>(&pressed);

	for (size_t j = 0; j < (256 / 32); ++j)
	{
		*pressedPtr = *currPtr & ~(*prevPtr);
		*releasedPtr = ~(*currPtr) & *prevPtr;

		++currPtr;
		++prevPtr;
		++releasedPtr;
		++pressedPtr;
	}

	LastState = State;
}

void __cdecl Keyboard::KeyboardEvent::Reset() noexcept
{
	memset(this, 0, sizeof(KeyboardEvent));
}
