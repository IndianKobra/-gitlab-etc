#include <cstdio>   //4 files
#include <unistd.h> //4 _chsize
#include <iostream> //4 cout
#include <stdlib.h> //4 exit
using namespace std;


template <typename Type>
class Lvalue
{
	private:
		FILE *file;
		size_t index;
	public:
		Lvalue(FILE *f, size_t i)
		{
			file = f;
			index = i;
		}
		operator Type()
		{
			Type New;//!
			fseek(file, sizeof(Type) * index, SEEK_SET);
			fread(&New, sizeof(Type), 1, file);
			return New;
		}
		void operator=(const Type& right)
		{
			fseek(file, index*sizeof(Type), SEEK_SET);
			fwrite(&right, sizeof(Type), 1, file);
		}
};

template <typename Type>
class FileArray
{
	private:
		FILE *file;
	public:
	FileArray(const char* FileName, bool CreateIfNotExist = false)
	{
		file = fopen(FileName, "rb+");
		if(!file && CreateIfNotExist) file = fopen(FileName, "wb+");
		if(!file && !CreateIfNotExist)
		{
			cout << "file:\"" << FileName << "\" not found\n";
			exit(-1);
		}
	}
	Lvalue<Type> operator[] (size_t n)
	{
		if(n>=size())
		{
			cout << "filearray out of range\n";
			exit(-1);
		}
		return Lvalue<Type>(file, n);
	}
	size_t size()
	{
		fseek(file, 0, SEEK_END);
		return ftell(file) / sizeof(Type);
	}
	void resize(size_t n)
	{
		if(size() < n)
		{
			ftruncate(fileno(file), size()*sizeof(Type));
			fseek(file, 0, size()*sizeof(Type));
			n -=size();
			while(n-->0)
			{
				Type New;
				fwrite(&New, sizeof(Type) , 1, file);
			}
		}
		ftruncate(fileno(file), n*sizeof(Type));
	}
	~FileArray()
	{
		fclose(file);
	}
};
