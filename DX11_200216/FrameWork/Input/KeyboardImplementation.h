#pragma once
#include "Framework/Input/Keyboard.h"

class KeyboardImplementation
{
private:
	static shared_ptr<KeyboardImplementation> _Instance;
	static once_flag flag;
public:
	static KeyboardImplementation* Instance()
	{
		call_once(flag, [] {
			_Instance = make_shared<KeyboardImplementation>();
			});

		return _Instance.get();
	}
	KeyboardState state;
	Keyboard* keyboard;
	KeyboardImplementation();
	~KeyboardImplementation();
	void GetState(KeyboardState& state) const;
	void Reset();
	bool IsConnected() const;
};