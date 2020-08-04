#pragma once
#include "Framework/Input/KeyboardType.h"

enum class KeyboardStatus
{
	NONE,
	PRESS,
	RELEASE
};

class Keyboard
{
private:
	friend class KeyboardInput;
	KeyboardStatus Status;
	unsigned char Key;

	static shared_ptr<Keyboard> _Instance;
	static once_flag flag;
public:
	static Keyboard* Instance()
	{
		call_once(flag, [] {
			_Instance = make_shared<Keyboard>();
			});

		return _Instance.get();
	}
	Keyboard() noexcept(false);
	~Keyboard();

	class KeyboardEvent
	{
	public:
		KeyboardState released;
		KeyboardState pressed;

#pragma prefast(suppress: 26495, "Reset()")
		KeyboardEvent() noexcept;

		void __cdecl Update(const KeyboardState& State);
		void __cdecl Reset() noexcept;

		bool __cdecl IsKeyPressed(Keys key) const { return pressed.IsKeyDown(key); }
		bool __cdecl IsKeyReleased(Keys key) const { return released.IsKeyDown(key); }

		KeyboardState __cdecl GetLastState() const { return LastState; }

		KeyboardState LastState;
	};

	KeyboardState __cdecl GetState() const;
	void __cdecl Reset();

	bool __cdecl IsConnected() const;

#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP) && defined(WM_USER)
	static void __cdecl ProcessMsg(UINT msg, WPARAM wparam, LPARAM lparam);
#endif
};
