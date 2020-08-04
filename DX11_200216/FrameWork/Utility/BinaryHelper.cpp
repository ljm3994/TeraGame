#include "Framework.h"
#include "BinaryHelper.h"

BinaryWrite::BinaryWrite(wstring Path)
{
	File = CreateFile(Path.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
}

BinaryWrite::~BinaryWrite()
{
	CloseHandle(File);
}

void BinaryWrite::Int(const int* Data)
{
	WriteFile(File, Data, sizeof(int), &Size, nullptr);
}

void BinaryWrite::Uint(const UINT* Data)
{
	WriteFile(File, Data, sizeof(UINT), &Size, nullptr);
}

void BinaryWrite::Float(const float* Data)
{
	WriteFile(File, Data, sizeof(float), &Size, nullptr);
}

void BinaryWrite::Double(const double* Data)
{
	WriteFile(File, Data, sizeof(double), &Size, nullptr);
}

void BinaryWrite::String(const string* Data)
{
	UINT size = Data->size();
	Uint(&size);

	const char* str = Data->c_str();
	WriteFile(File, str, sizeof(char) * size, &Size, nullptr);
}

void BinaryWrite::Matrix(const D3DXMATRIX* Data)
{
	WriteFile(File, Data, sizeof(D3DXMATRIX), &Size, nullptr);
}

void BinaryWrite::D3DXVector3(const D3DXVECTOR3* Data)
{
	WriteFile(File, Data, sizeof(D3DXVECTOR3), &Size, nullptr);
}

void BinaryWrite::D3DXVector2(const D3DXVECTOR2* Data)
{
	WriteFile(File, Data, sizeof(D3DXVECTOR2), &Size, nullptr);
}

void BinaryWrite::Byte(const void* Data, const UINT* ArraySize)
{
	WriteFile(File, Data, *ArraySize, &Size, nullptr);
}

BinaryReader::BinaryReader(wstring Path)
{
	File = CreateFile(Path.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
}

BinaryReader::~BinaryReader()
{
	CloseHandle(File);
}

int BinaryReader::Int()
{
	int val;
	bool Result = ReadFile(File, &val, sizeof(int), &Size, nullptr);

	return val;
}

UINT BinaryReader::Uint()
{
	UINT val = 0; 
	bool Result = ReadFile(File, &val, sizeof(UINT), &Size, nullptr);
	return val;
}

float BinaryReader::Float()
{
	float val;
	bool Result = ReadFile(File, &val, sizeof(float), &Size, nullptr);

	return val;
}

double BinaryReader::Double()
{
	double val;
	bool Result = ReadFile(File, &val, sizeof(double), &Size, nullptr);

	return val;
}

string BinaryReader::String()
{
	UINT Size = 0;
	Size = Uint();
	bool Result = false;
	if (Size > 0)
	{
		char* Temp = new char[Size + 1];
		Result = ReadFile(File, Temp, sizeof(char) * Size, &this->Size, nullptr);
		Temp[Size] = '\0';
		return Temp;
	}

	return "";
}

D3DXVECTOR3 BinaryReader::D3DXVector3()
{
	D3DXVECTOR3 val;
	val.x = Float();
	val.y = Float();
	val.z = Float();
	return val;
}

D3DXVECTOR2 BinaryReader::D3DXVector2()
{
	D3DXVECTOR2 val;
	val.x = Float();
	val.y = Float();
	return val;
}

D3DXMATRIX BinaryReader::Matrix()
{
	D3DXMATRIX matrix;
	matrix._11 = Float();
	matrix._12 = Float();
	matrix._13 = Float();
	matrix._14 = Float();

	matrix._21 = Float();
	matrix._22 = Float();
	matrix._23 = Float();
	matrix._24 = Float();

	matrix._31 = Float();
	matrix._32 = Float();
	matrix._33 = Float();
	matrix._34 = Float();

	matrix._41 = Float();
	matrix._42 = Float();
	matrix._43 = Float();
	matrix._44 = Float();

	return matrix;
}

bool BinaryReader::Byte(void** Data, const UINT* ArraySize)
{
	bool Result = ReadFile(File, *Data, *ArraySize, &Size, NULL);

	return Result;
}
