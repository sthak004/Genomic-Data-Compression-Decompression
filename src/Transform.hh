#ifndef Transform_hh
#define Transform_hh

//************************************************************
// Class Name: Transform
//
// Design:
//
// Usage/Limitations:
//
//************************************************************

#include <string>
#include <stdint.h>

class Transform
{
	public:

	Transform();
	~Transform();
	
	//Method: Compress
	//Output: Integer,
	//Input: Character Pointer,
	int Compress(char* fileName/*uint8_t letter, short& lines, int32_t& t_letters*/); 

	//Method: getFileExt
	//Output: Text,
	//Input:  Text,
	std::string getFileExt(char* fileName/*const std::string& s*/);

	private:
};

#endif
