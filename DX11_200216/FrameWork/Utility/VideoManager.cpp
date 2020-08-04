#include "Framework.h"

shared_ptr<VideoManager> VideoManager::_Instance = nullptr;
once_flag VideoManager::flag;

VideoManager::VideoManager()
{
	isPlay = false;
	this->Hwnd = nullptr;
	this->hinstance = nullptr;
	MovieHwnd = 0;
	Width = 0;
	Height = 0;
}

VideoManager::~VideoManager()
{
}

void VideoManager::SetWindowHwnd(UINT WINSIZEX, UINT WINSIZEY, HWND hwnd, HINSTANCE hinstance)
{
	this->Hwnd = hwnd;
	this->hinstance = hinstance;

	Width = WINSIZEX;
	Height = WINSIZEY;
}


void VideoManager::AddMovie(string Key, _TCHAR* filePath, int width, int height, int XPOS, int YPOS)
{
	VideoInfo info;
	info.filePath = filePath;
	info.height = height;
	info.width = width;
	info.XPOS = XPOS;
	info.YPOS = YPOS;

	videoMap.insert(make_pair(Key, info));
}

void VideoManager::Play(string strKey)
{
	auto VideoIter = videoMap.find(strKey);
	int width = (*VideoIter).second.width;
	int height = (*VideoIter).second.height;
	if (!isPlay)
	{
		isPlay = true;
	}
	if (width == 0) { width = Width; }
	if (height == 0) { height = Height; }

	if (MovieHwnd)
	{
		MCIWndClose(MovieHwnd);
		MCIWndDestroy(MovieHwnd);
		MovieHwnd = 0;
	}

	MovieHwnd = MCIWndCreate(Hwnd,
		hinstance,
		MCIWNDF_NOTIFYMODE | MCIWNDF_NOPLAYBAR | MCIWNDF_NOTIFYPOS | MCIWNDF_SHOWALL | MCIWNDF_NOMENU,
		(*VideoIter).second.filePath);
	if (MovieHwnd)
	{
		RECT rt;
		GetClientRect(Hwnd, &rt);
		SetWindowPos(MovieHwnd, NULL, 0, 0, rt.right, rt.bottom, SWP_NOZORDER);
		MCIWndPlay(MovieHwnd);
		isPlay = true;
	}
}

void VideoManager::Play(_TCHAR* filePath)
{
	if (!isPlay)
	{
		isPlay = true;
	}

	if (MovieHwnd)
	{
		MCIWndClose(MovieHwnd);
		MCIWndDestroy(MovieHwnd);
		MovieHwnd = 0;
	} 

	MovieHwnd = MCIWndCreate(Hwnd,
		hinstance,
		MCIWNDF_NOTIFYMODE | MCIWNDF_NOPLAYBAR | MCIWNDF_NOTIFYPOS | MCIWNDF_SHOWALL | MCIWNDF_NOMENU,
		filePath);
	if (MovieHwnd)
	{
		RECT rt;
		GetClientRect(Hwnd, &rt);
		SetWindowPos(MovieHwnd, NULL, 0, 0, rt.right, rt.bottom, SWP_NOZORDER);
		MCIWndPlay(MovieHwnd);
		isPlay = true;
	}
}

void VideoManager::Stop()
{
	if (MovieHwnd)
	{
		isPlay = false;
		MCIWndStop(MovieHwnd);
		MCIWndClose(MovieHwnd);
		MCIWndDestroy(MovieHwnd);
	}
}

bool VideoManager::MovieCanPlay()
{
	if (MovieHwnd)
	{
		return isPlay;
	}
	return false;
}

HWND VideoManager::GetVideoHandel()
{
	return MovieHwnd;
}

LONG VideoManager::GetEnd()
{
	return MCIWndGetEnd(MovieHwnd);
}

bool VideoManager::isEnd()
{
	return GetEnd() == MCIWndGetPosition(MovieHwnd);
}

bool VideoManager::GetisPlay()
{
	return isPlay;
}

void VideoManager::SetIsPlay(bool val)
{
	isPlay = val;
}
