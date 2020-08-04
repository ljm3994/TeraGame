// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
#define MAX_LOADSTRING 100
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define DEBUG(hr) assert(SUCCEEDED(hr))

// Windows 헤더 파일
#include <windows.h>
#include <mmsystem.h>
#include <vector>
#include <wrl/client.h>
#include <memory>
#include <mutex>
#include <xcall_once.h>
#include <map>
#include <cassert>
#include <unordered_map>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <functional>
#include <Shlwapi.h>
#include <direct.h>
#include <random>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <future>
#include <tchar.h>
#include <vfw.h>

#pragma comment(lib,"vfw32.lib")
using namespace std;
using namespace Microsoft::WRL;

const string AnimationDefaultPath = "ModelData/FBX/Animation/";
const string DefaultMaterialPath = "ModelData/FBX/Materials/";
const string DefaultMeshPath = "ModelData/FBX/Meshes/";
const string TextureDefaultPath = "ModelData/FBX/Model/";
const string BoneDefaultPath = "ModelData/FBX/Bones/";

// C 런타임 헤더 파일입니다.
#include <d3d11.h>
#include <d3d11shader.h>
#include <D3DX11.h>
#include <d3dcompiler.h>
#include <D3DX10math.h>
#include <d3dx9.h>
#include <d3dx9anim.h>
#include "DirectXHelpers.h"
#include "DDSTextureLoader.h"
#include "Model.h"
#include <dinput.h>
#include <pdh.h>
#include <dxgi.h>
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include <PostProcess.h>

#pragma comment(lib, "D3D10.lib")
#pragma comment(lib, "D3DX10.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "pdh.lib")

#include "Libraries/imgui.h"
#include "Libraries/imgui_impl_win32.h"
#include "Libraries/imgui_impl_dx11.h"

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include "FrameWork\Utility\GameMath.h"

#pragma comment (lib, "assimp/assimp-vc142-mtd.lib")

#include "Libraries\GFSDK_SSAO.h"

#pragma comment(lib, "Libraries/GFSDK_SSAO_D3D11.win32.lib")

#define WindowName L"DX11"
#define Engine DXEngine::Instance()
#define InputKey Keyboard::Instance()
#define InputMouse Mouse::Instance()
#define VIDEOMANAGER VideoManager::Instance()
#define LERP(s, e, t) (s + (e - s)*t)
#define BONE_MAX_COUNT 200
#define MAX_INSTANCECOUNT 100

#define SHARED_PTR_DECL(T) \
class T; \
typedef shared_ptr<T> ##T##Ptr;

#define FRIEND_STD_MAKE_SHARED \
template<class T> \
friend class _Ref_count_obj;

#define ENUM_FLAG_OPERATOR(T,X) inline T operator X (T lhs, T rhs) { return (T) (static_cast<std::underlying_type_t <T>>(lhs) X static_cast<std::underlying_type_t <T>>(rhs)); } 
#define ENUM_FLAGS(T) \
enum class T; \
inline T operator ~ (T t) { return (T) (~static_cast<std::underlying_type_t <T>>(t)); } \
ENUM_FLAG_OPERATOR(T,|) \
ENUM_FLAG_OPERATOR(T,^) \
ENUM_FLAG_OPERATOR(T,&) \
enum class T

struct handle_closer { void operator()(HANDLE h) { if (h) CloseHandle(h); } };

typedef unique_ptr<void, handle_closer> ScopedHandle;

//기본헤더
#include "FrameWork/Render/Utility/VertexTypeHelper.h"
#include "FrameWork/Utility/Helper.h"
#include "FrameWork/Input/KeyboardImplementation.h"
#include "FrameWork/Input/MouseImplementation.h"
#include "FrameWork\Utility\VideoManager.h"
#include "FrameWork/Core/DXEngine.h"

// 오브젝트 헤더

//지형 헤더

using namespace GameMath;