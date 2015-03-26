#pragma once
#include <string>

using namespace std;

class ShaderLoader
{
public:
	string* LoadFile(const string _filePath);
	unsigned int LoadShader(unsigned int _type, const char* _path);
};