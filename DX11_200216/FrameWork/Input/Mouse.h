#pragma once

#define UPDATE_BUTTON_STATE(field) field = static_cast<ButtonState>( ( !!state.field ) | ( ( !!state.field ^ !!lastState.field ) << 1 ) );

//noexcept를 통해 함수가 예외를 던질수 있는지 없는지에 대한 표기
// false : 예외를 던질 수 있음
// 그냥 noexcept 만 있으면 예외를 던질 수 없음
// 예외를 던질 수 없음에도 예외를 던지면 terminated 가 호출됨
// noexcept 를 쓰는 이유는 해당 키워드가 붙은 함수에 대해서는 컴파일러가 특정한 최적화를 진행하기도 하며
// STL 사용시 원소에 대한 이동 처리를 할떄 move semantics 를 사용하여 처리하도록 한다.
// 이로인해 성능상 이득을 볼 수 있음...(아무래도 값을 복사하는 것보다 이동하는게 훨씬 이득)
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
		MODE_ABSOLUTE,  //절대 값 모드
		MODE_RELATIVE   //상대 값 모드
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
	//스크롤 휠 리셋
	void __cdecl ResetScrollWheelValue();
	//마우스 모드 변경
	void __cdecl SetMode(MouseMode mode);
	// 기능 감지 여부
	bool __cdecl isConnected() const;

	//마우스 커서 관련
	bool __cdecl isVisible() const;
	void __cdecl SetVisible(bool visible);

#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP) && defined(WM_USER)
	void __cdecl SetWindow(HWND hwnd);
	static void __cdecl ProcessMsg(UINT msg, WPARAM wparam, LPARAM lparam);
#endif
};