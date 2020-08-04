#pragma once

struct VideoInfo
{
	_TCHAR* filePath;
	int width;
	int height;
	int XPOS;
	int YPOS;
};

class VideoManager
{
private:
	map<string, VideoInfo> videoMap;
	bool isPlay;
	HWND MovieHwnd;
	HWND Hwnd;
	HINSTANCE hinstance;
	UINT Width;
	UINT Height;
	static shared_ptr<VideoManager> _Instance;
	static once_flag flag;
public:
	static VideoManager* Instance()
	{
		call_once(flag, [] {
			_Instance = make_shared<VideoManager>();
			});

		return _Instance.get();
	}
	VideoManager();
	~VideoManager();
	void SetWindowHwnd(UINT WINSIZEX, UINT WINSIZEY, HWND hwnd, HINSTANCE hinstance);
	void AddMovie(string Key, _TCHAR* filePath, int width, int height, int XPOS, int YPOS);
	void Play(string strKey);
	void Play(_TCHAR* filePath);
	void Stop();
	bool MovieCanPlay();
	HWND GetVideoHandel();
	LONG GetEnd();
	bool isEnd();
	bool GetisPlay();
	void SetIsPlay(bool val);
};