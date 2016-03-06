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
	int Compress(char* fileName); 

	//Method: getFileExt
	//Output: Text,
	//Input:  Text,
	std::string getFileExt(char* fileName);
};

#endif
