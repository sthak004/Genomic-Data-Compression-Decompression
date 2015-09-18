/*
 * Course: CS 100 Summer Session II 2015
 *
 * First Name: Stavan 
 * Last Name: Thaker 
 * Username: sthak004 
 * Email address: sthak004@ucr.edu
 * SID: 861106198
 *
 * AssignmentID (e.g. lab0, lab1, ..., hw0, hw1,...): hw2
 * Filename: <File>
 *
 * I hereby certify that the contents of this file represent
 * my own original individual work. Nowhere herein is there
 * code from any outside resources such as another individual,
 * a website, or publishings unless specifically designated as
 * permissible by the instructor or TA.
 * I also understand that by cheating, stealing, plagiarism or
 * any other form of academic misconduct defined at
 * http://conduct.ucr.edu/policies/academicintegrity.html,
#include <string>
 * the consequences will be an F in the class, and additional
 * disciplinary sanctions, such as the dismissal from UCR.
 *
 */

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
