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
 * the consequences will be an F in the class, and additional
 * disciplinary sanctions, such as the dismissal from UCR.
 *
 */

//************************************************************
// Transform Implementation
//************************************************************

#include "./Transform.hh"
#include <iostream>
#include <string>


int main(int argc, char** argv){
	Transform obj;
	obj.Compress(argv[1]);
	return 0;
}
