#include "Framework.h"
#include "Dirent.h"
#include <atlbase.h>
#include <ShObjIdl.h>
#include <shellapi.h>
#include <FrameWork\Render\Shader\Material.h>
#include <FrameWork\Utility\BinaryHelper.h>
#include "FrameWork\Utility\Xml.h"

D3DXVECTOR3 Helper::GetNormalVec(const D3DXVECTOR3* v1, const D3DXVECTOR3* v2, const D3DXVECTOR3* v3)
{

	D3DXVECTOR3 A = *v2 - *v1;
	D3DXVECTOR3 B = *v3 - *v1;

	D3DXVECTOR3 normal;

	D3DXVec3Cross(&normal, &A, &B);

	return normal;
}

D3DXVECTOR3 Helper::GetTangent(const D3DXVECTOR3* p1, const D3DXVECTOR3* p2, const D3DXVECTOR3* p3, const D3DXVECTOR2* uv0, const D3DXVECTOR2* uv1, const D3DXVECTOR2* uv2)
{
	D3DXVECTOR3 e0 = *p2 - *p1;
	D3DXVECTOR3 e1 = *p3 - *p1;

	float u0 = uv1->x - uv0->x;
	float u1 = uv2->x - uv0->x;
	float v0 = uv1->y - uv0->y;
	float v1 = uv2->y - uv0->y;

	float d = 1.0f / (u0 * v1 - v0 * u1);

	D3DXVECTOR3 tangent;
	tangent = (v1 * e0 - v0 * e1) * d;

	return D3DXVECTOR3();
}

wstring Helper::stringTowstring(const string* value)
{
	wstring temp = L"";
	temp.assign(value->begin(), value->end());

	return temp;
}

string Helper::wstringTostring(const wstring* value)
{
	string temp = u8"";

	temp.assign(value->begin(), value->end());

	return temp;
}

vector<string> Helper::Splitstring(const string* Origine, string tok)
{
	vector<string> result;
	string Temp = *Origine;
	int cutAt = 0;
	while ((cutAt = (int)Temp.find_first_of(tok)) != Temp.npos)
	{
		if (cutAt > 0)
			result.push_back(Temp.substr(0, cutAt));

		Temp = Temp.substr(cutAt + 1);
	}

	if (Temp.length() > 0)
		result.push_back(Temp.substr(0, cutAt));

	return result;
}

bool Helper::ShowFileSelectWindow(OUT string* FileName, OUT string* Path, IN const wstring* FileExtension, IN int* ExtensionCount, IN const wstring* Titlestr, IN const bool FileSave)
{
	if (*ExtensionCount > 0)
	{
		setlocale(LC_ALL, "");
		CComPtr<IFileDialog> pfd;
		CoInitialize(0);
		COMDLG_FILTERSPEC* c_rgSaveTypes = new COMDLG_FILTERSPEC[*ExtensionCount];
		wstring* Wtemp = new wstring[*ExtensionCount];
		wstring* WtempSpec = new wstring[*ExtensionCount];
		for (int i = 0; i < *ExtensionCount; i++)
		{
			Wtemp[i] = FileExtension[i] + L" File";
			WtempSpec[i] = L"*." + FileExtension[i];
			c_rgSaveTypes[i].pszName = Wtemp[i].c_str();
			c_rgSaveTypes[i].pszSpec = WtempSpec[i].c_str();;
		}

		pfd = nullptr;
		HRESULT hr;

		if (FileSave)
		{
			hr = pfd.CoCreateInstance(CLSID_FileSaveDialog, NULL,
				CLSCTX_INPROC_SERVER);
		}
		else
		{
			hr = pfd.CoCreateInstance(CLSID_FileOpenDialog, NULL,
				CLSCTX_INPROC_SERVER);
		}

		DEBUG(hr);

		pfd->SetTitle(Titlestr->c_str());
		pfd->SetOptions(FOS_OVERWRITEPROMPT | FOS_FORCEFILESYSTEM | FOS_PATHMUSTEXIST);
		pfd->SetFileTypes(*ExtensionCount, c_rgSaveTypes);
		pfd->SetFileTypeIndex(1);
		pfd->Show(FindWindowW(WindowName, WindowName));

		CComPtr<IShellItem> items;

		if (pfd->GetResult(&items) == S_OK)
		{
			wchar_t* path;
			size_t pReturnValue = 0;
			char Drive[256];
			char dir[256];
			char fname[256];
			char ext[256];
			HRESULT hr = items->GetDisplayName(SIGDN_FILESYSPATH, &path);

			if (SUCCEEDED(hr))
			{
				char* strpath = new char[255];
				wcstombs_s(&pReturnValue, strpath, 255, path, 255);
				_splitpath_s(strpath, Drive, 256, dir, 256, fname, 256, ext, 256);

				*FileName = fname;
				*Path = strpath;

				return true;
				delete[] strpath;
			}
			CoTaskMemFree(path);
		}
		CoUninitialize();
		delete[] c_rgSaveTypes;
		delete[] Wtemp;
		delete[] WtempSpec;
	}
	return false;
}

vector<string> Helper::GetDrivesList()
{
	vector<string> res;

	DWORD mydrives = 2048;
	char lpBuffer[2048];

	DWORD countChars = GetLogicalDriveStringsA(mydrives, lpBuffer);

	if (countChars > 0)
	{
		string var = std::string(lpBuffer, countChars);
		Replace(&var, "\\", "");
		res = Splitstring(&var, "\0");
	}

	return res;
}

bool Helper::IsDirectoryExist(const std::string& name)
{
	bool bExists = false;

	if (name.size() > 0)
	{
		DIR* pDir = 0;
		pDir = opendir(name.c_str());
		if (pDir != NULL)
		{
			bExists = true;
			(void)closedir(pDir);
		}
	}

	return bExists;
}

void Helper::Replace(OUT string* str, IN string comp, IN string rep)
{
	string temp = *str;

	size_t startPos = 0;

	while ((startPos = temp.find(comp, startPos)) != string::npos)
	{
		temp.replace(startPos, comp.length(), rep);
		startPos += rep.length();
	}

	*str = temp;
}

bool Helper::IsExistDirectory(string path)
{
	DWORD attribue = GetFileAttributesA(path.c_str());

	BOOL temp = (attribue != INVALID_FILE_ATTRIBUTES && (attribue & FILE_ATTRIBUTE_DIRECTORY));

	return temp == TRUE;
}

void Helper::CreateFolders(string path, bool isFileName)
{
	vector<string> folders = Splitstring(&path, "/");

	if (isFileName)
		folders.pop_back();

	string temp = "";
	for (string folder : folders)
	{
		temp += folder + "/";

		if (!IsExistDirectory(temp))
			CreateDirectoryA(temp.c_str(), 0);
	}
}

void Helper::FileCopy(string FromPath, string ToPath, bool isFileName)
{
	string DirPath = FromPath + "\0";
	//ToPath += GetFileName(FromPath) + "\0";
	SHFILEOPSTRUCT fo;
	wstring Path = stringTowstring(&DirPath);
	wstring TopathTemp = stringTowstring(&ToPath);


	fo.hwnd = NULL;
	fo.wFunc = FO_MOVE;
	fo.pFrom = Path.c_str();
	fo.pTo = TopathTemp.c_str();
	fo.fFlags = FOF_NOCONFIRMATION;

	SHFileOperation(&fo);
}

string Helper::GetFileName(string path)
{
	Replace(&path, "\\", "/");

	size_t index = path.find_last_of('/');

	string fileName = path.substr(index + 1, path.length());

	string extention = GetExtention(fileName);

	if (extention == "tga" || extention == "psd")
	{
		size_t index = fileName.find_last_of('.');

		fileName.replace(index + 1, 3, "png");
	}

	return fileName;
}

string Helper::GetExtention(string fileName)
{
	size_t index = fileName.find_last_of('.');

	return fileName.substr(index + 1, fileName.length());
}

bool Helper::CreateDirectoryIfNotExitst(string& Name)
{
	bool res = false;

	if (Name.size() > 0)
	{
		if (!IsDirectoryExist(Name))
		{
			res = true;

			CreateDirectoryA(Name.c_str(), NULL);
		}
	}

	return res;
}

PathInfo Helper::ParsePathFileName(const string& _PathFileName, const string& _pathsep)
{
	PathInfo result;

	if (_PathFileName.size() > 0)
	{
		string pfn = _PathFileName;
		string separator = _pathsep;

		Replace(&pfn, "\\", separator);
		Replace(&pfn, "/", separator);

		size_t Slash = pfn.find_last_of(separator);
		if (Slash != string::npos)
		{
			result.name = pfn.substr(Slash + 1);
			result.path = pfn.substr(0, Slash);
			result.isOk = true;
		}

		size_t extIndex = pfn.find_last_of('.');
		if (extIndex != string::npos)
		{
			if (!result.isOk)
			{
				result.name = pfn;
				result.isOk = true;
			}

			result.ext = pfn.substr(extIndex + 1);
			Replace(&result.name, "." + result.ext, "");
		}
	}

	return result;
}

void Helper::AppendBuffer(char* _Buffer, size_t _Bufferlen, string _str)
{
	string str = _str;
	size_t len = _Bufferlen - 1u;
	size_t slen = strlen(_Buffer);

	if (str != "" && str != "\n")
	{
		Replace(&str, "\n", "");
		Replace(&str, "\r", "");
	}

	_Buffer[slen] = '\0';
	string strstr = string(_Buffer);
	if (strstr.size() > 0)
		strstr += "\n";
	strstr += _str;
	if (len > strstr.size())
		len = strstr.size();

	strncpy_s(_Buffer, _Bufferlen, strstr.c_str(), len);

	_Buffer[len] = '\0';
}

void Helper::ResetBuffer(char* _Buffer)
{
	_Buffer[0] = '\0';
}

bool Helper::stringComparator(const FileInfo& a, const FileInfo& b)
{
	bool result;
	if (a.type != b.type)
		result = (a.type < b.type);
	else
	{
		result = (a.FileName < b.FileName);
	}

	return result;
}

bool Helper::IsExistFile(string Path)
{
	DWORD attribute = GetFileAttributesA(Path.c_str());

	return attribute < 0xffffffff;
}

string Helper::GetDirectoryName(string Path)
{
	size_t index = Path.find_last_of('/');
	
	return Path.substr(0, index + 1);
}

void Helper::ReadStaticMesh(string FilePath, vector<StaticMeshDesc*>& desc)
{
	FilePath = DefaultMeshPath + FilePath + ".mesh";
	BinaryReader* r = new BinaryReader(Helper::stringTowstring(&FilePath));

	UINT count = r->Uint();
	desc.resize(count);
	for (UINT i = 0; i < count; i++)
	{
		desc[i] = new StaticMeshDesc();
		desc[i]->MatName = r->String();

		UINT verticeCount = r->Uint();
		desc[i]->vertices.resize(verticeCount);
		void* p = (void*)&desc[i]->vertices[0];
		UINT ArraySize = sizeof(VertexTangent) * verticeCount;
		r->Byte(&p, &ArraySize);

		UINT indicesCount = r->Uint();
		desc[i]->indices.resize(indicesCount);
		void* p2 = (void*)&desc[i]->indices[0];
		UINT ArraySize2 = sizeof(UINT) * indicesCount;
		r->Byte(&p2, &ArraySize2);

		desc[i]->vertexbuffers.reset(new VertexBuffer(desc[i]->vertices.data(), sizeof(VertexTangent), desc[i]->vertices.size()));
		desc[i]->indexbuffers.reset(new IndexBuffer(desc[i]->indices.data(), desc[i]->indices.size()));
	}

	delete r;
}

void Helper::ReadStaticMaterial(string FilePath, map<string, Material*>& materials)
{
	FilePath = DefaultMaterialPath + FilePath + ".mat";

	XmlDocument* document = new XmlDocument();
	document->LoadFile(FilePath.c_str());

	XmlElement* root = document->FirstChildElement();
	XmlElement* matNode = root->FirstChildElement();

	string folder = Helper::GetDirectoryName(FilePath);
	size_t index = folder.find_last_of('/');
	folder = folder.substr(0, index);
	index = folder.find_last_of('/');
	folder = folder.substr(index + 1, folder.length());
	folder += "/";

	do
	{
		XmlElement* node = matNode->FirstChildElement();

		Material* material = new Material();
		material->SetName(node->GetText());

		node = node->NextSiblingElement();

		if (node->GetText())
		{
			string file = node->GetText();
			if (Helper::IsExistFile(TextureDefaultPath + folder + file))
			{
				material->SetDiffuseMap(file, TextureDefaultPath + folder + file);
			}
		}

		node = node->NextSiblingElement();

		if (node->GetText())
		{
			string file = node->GetText();
			if (Helper::IsExistFile(TextureDefaultPath + folder + file))
			{
				material->SetSpecularMap(TextureDefaultPath + folder + file);
			}
		}

		node = node->NextSiblingElement();

		if (node->GetText())
		{
			string file = node->GetText();
			if (Helper::IsExistFile(TextureDefaultPath + folder + file))
			{
				material->SetNormalMap(TextureDefaultPath + folder + file);
			}
		}

		D3DXCOLOR color;

		node = node->NextSiblingElement();
		color.r = node->FloatAttribute("R");
		color.g = node->FloatAttribute("G");
		color.b = node->FloatAttribute("B");
		color.a = node->FloatAttribute("A");
		material->GetBuffer()->data.ambient = color;

		node = node->NextSiblingElement();
		color.r = node->FloatAttribute("R");
		color.g = node->FloatAttribute("G");
		color.b = node->FloatAttribute("B");
		color.a = node->FloatAttribute("A");
		material->GetBuffer()->data.diffuse = color;

		node = node->NextSiblingElement();
		color.r = node->FloatAttribute("R");
		color.g = node->FloatAttribute("G");
		color.b = node->FloatAttribute("B");
		color.a = node->FloatAttribute("A");
		material->GetBuffer()->data.specular = color;

		materials.insert(make_pair(material->GetName(), material));

		matNode = matNode->NextSiblingElement();
	} while (matNode != nullptr);
}

bool Helper::PtInRect(RECT Rect, D3DXVECTOR2 InPt)
{
	if (Rect.left <= InPt.x && InPt.x <= Rect.right &&
		Rect.top >= InPt.y && InPt.y >= Rect.bottom)
	{
		return true;
	}
	return false;
}
