#pragma once
class Material;
class VertexBuffer;
class IndexBuffer;

struct PathInfo
{
	string path;
	string name;
	string ext;

	bool isOk;

	PathInfo()
	{
		isOk = false;
	}
};

struct FileInfo
{
	char type = ' ';
	string FilePath;
	string FileName;
	string Ext;
	FILETIME CreationTime;
	FILETIME AccesTime;
	FILETIME WriteTime;
	float FileSize;
};

struct Ray
{
	D3DXVECTOR3 position;
	D3DXVECTOR3 direction;
};

struct StaticMeshDesc
{
	string MatName;
	vector<VertexTangent> vertices;
	vector<UINT> indices;

	unique_ptr<VertexBuffer> vertexbuffers;
	unique_ptr<IndexBuffer> indexbuffers;
};

class Helper
{
public:
	static D3DXVECTOR3 GetNormalVec(const D3DXVECTOR3* v1, const D3DXVECTOR3* v2, const D3DXVECTOR3* v3);

	static D3DXVECTOR3 GetTangent(const D3DXVECTOR3* p1, const D3DXVECTOR3* p2, const D3DXVECTOR3* p3, const D3DXVECTOR2* uv0, const D3DXVECTOR2* uv1, const D3DXVECTOR2* uv2);

	static wstring stringTowstring(const string* value);

	static string wstringTostring(const wstring* value);

	static vector<string> Splitstring(const string* Origine, string tok);

	static bool ShowFileSelectWindow(OUT string* FileName, OUT string* Path, IN const wstring* FileExtension, IN int* ExtensionCount, IN const wstring* Titlestr, IN const bool FileSave);

	static vector<string> GetDrivesList();

	static bool IsDirectoryExist(const std::string& name);

	static void Replace(OUT string* str, IN string comp, IN string rep);

	static bool IsExistDirectory(string path);

	static void CreateFolders(string path, bool isFileName = false);

	static void FileCopy(string FromPath, string ToPath, bool isFileName = true);

	static string GetFileName(string path);

	static string GetExtention(string fileName);

	static bool CreateDirectoryIfNotExitst(string& Name);

	static PathInfo ParsePathFileName(const string& _PathFileName, const string& _pathsep);

	static void AppendBuffer(char* _Buffer, size_t _Bufferlen, string _str);

	static void ResetBuffer(char* _Buffer);

	static bool stringComparator(const FileInfo& a, const FileInfo& b);

	static bool IsExistFile(string Path);

	static string GetDirectoryName(string Path);

	static void ReadStaticMesh(string FilePath, vector<StaticMeshDesc*>& desc);

	static void ReadStaticMaterial(string FilePath, map<string, Material*>& materials);

	static bool PtInRect(RECT Rect, D3DXVECTOR2 InPt);

	static void CreateRandomTexture(ID3D11ShaderResourceView** RandomText, bool isSampler = false, ID3D11SamplerState** Sampler = nullptr);
};