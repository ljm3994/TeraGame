#pragma once

#define UPDATE_BUTTON_STATE(field) field = static_cast<ButtonState>( ( !!state.field ) | ( ( !!state.field ^ !!lastState.field ) << 1 ) );

//noexcept�� ���� �Լ��� ���ܸ� ������ �ִ��� �������� ���� ǥ��
// false : ���ܸ� ���� �� ����
// �׳� noexcept �� ������ ���ܸ� ���� �� ����
// ���ܸ� ���� �� �������� ���ܸ� ������ terminated �� ȣ���
// noexcept �� ���� ������ �ش� Ű���尡 ���� �Լ��� ���ؼ��� �����Ϸ��� Ư���� ����ȭ�� �����ϱ⵵ �ϸ�
// STL ���� ���ҿ� ���� �̵� ó���� �ҋ� move semantics �� ����Ͽ� ó���ϵ��� �Ѵ�.
// �̷����� ���ɻ� �̵��� �� �� ����...(�ƹ����� ���� �����ϴ� �ͺ��� �̵��ϴ°� �ξ� �̵�)
class MouseImplementation;

class Mouse
{
	static shared_ptr<Mouse> _Instance;
	static once_flag flag;
public:
	static Mouse* Instance()
	{
		call_once(flag, [] {
			_Instance = make_shared<Mouse>();
			});

		return _Instance.get();
	}
	Mouse() noexcept(false);
	~Mouse();

	enum MouseMode
	{
		MODE_ABSOLUTE,  //���� �� ���
		MODE_RELATIVE   //��� �� ���
	};

	struct MouseState
	{
		bool    IsleftButton;
		bool    IsmiddleButton;
		bool    IsrightButton;
		bool    IsxButton1;
		bool    IsxButton2;
		int     x;
		int     y;
		int		ScrollWheelValue;
		MouseMode mode;
	} State;

	class MouseEvent
	{
	private:
		MouseState LastState;

	public:
		enum EventType
		{
			UP,
			HELD,
			RELEASED,
			PRESSED
		};

		EventType IsleftButton;
		EventType IsmiddleButton;
		EventType IsrightButton;
		EventType IsxButton1;
		EventType IsxButton2;

#pragma prefast(suppress: 26495, "Reset()")

		MouseEvent() noexcept { Reset(); }
		void __cdecl Update(const MouseState& state);
		void __cdecl Reset() noexcept;
		MouseState __cdecl GetLastState() const { return LastState; }

	};

	MouseState __cdecl GetState() const;
	//��ũ�� �� ����
	void __cdecl ResetScrollWheelValue();
	//���콺 ��� ����
	void __cdecl SetMode(MouseMode mode);
	// ��� ���� ����
	bool __cdecl isConnected() const;

	//���콺 Ŀ�� ����
	bool __cdecl isVisible() const;
	void __cdecl SetVisible(bool visible);

#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP) && defined(WM_USER)
	void __cdecl SetWindow(HWND hwnd);
	static void __cdecl ProcessMsg(UINT msg, WPARAM wparam, LPARAM lparam);
#endif
};