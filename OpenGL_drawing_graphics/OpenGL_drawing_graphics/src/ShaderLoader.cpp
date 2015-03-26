#include "ShaderLoader.h"
#include <gl_core_4_4.h>
#include <GLFW/glfw3.h>
#include <fstream>

string* ShaderLoader::LoadFile(const string _filePath)
{
	string* output = new string();
	string line;

	//open file
	ifstream file;
	file.open(_filePath.c_str());

	//check if succeeded
	if (file.is_open())
	{
		//read the file whilst still readable
		while (file.good())
		{
			getline(file, line);
			output->append(line + "\n");
		}
	}
	else
	{
		printf("Could not load shader program...");
	}

	return output;
}

unsigned int ShaderLoader::LoadShader(unsigned int _type, const char* _path)
{
	//poening the file for read and checking if it succeeded
	FILE* file = fopen(_path, "rb");
	if (file == nullptr)
		return 0;

	//read the shader source
	fseek(file, 0, SEEK_END);

	unsigned int length = ftell(file);
	fseek(file, 0, SEEK_SET);

	char* source = new char[length + 1];
	memset(source, 0, length + 1);

	fread(source, sizeof(char), length, file);
	fclose(file);

	//creating a shader with the read-in data
	unsigned int shader = glCreateShader(_type);
	glShaderSource(shader, 1, &source, 0);
	glCompileShader(shader);
	delete[] source;

	return shader;
}