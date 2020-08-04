#include "Framework.h"
#include "KeyboardImplementation.h"

shared_ptr<KeyboardImplementation> KeyboardImplementation::_Instance = nullptr;
once_flag KeyboardImplementation::flag;

KeyboardImplementation::KeyboardImplementation()
	: state{}, keyboard(nullptr)
{
}

KeyboardImplementation::~KeyboardImplementation()
{
}

void KeyboardImplementation::GetState(KeyboardState& state) const
{
	memcpy_s(&state, sizeof(KeyboardState), &this->state, sizeof(KeyboardState));
}

void KeyboardImplementation::Reset()
{
	memset(&state, 0, sizeof(KeyboardState));
}

bool KeyboardImplementation::IsConnected() const
{
	return true;
}
