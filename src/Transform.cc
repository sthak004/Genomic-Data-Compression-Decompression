//************************************************************
// Transform Implementation
//************************************************************

#include "./Transform.hh"
#include <bitset>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>



// Transform Constructor
Transform::Transform()
{}

// Transform Destructor
Transform::~Transform()
{}


// Method: getFileExt
//************************************************************
// Purpose: to get a file extension from a passed in pass
//
// Implementation Notes:
//
//************************************************************
std::string Transform::getFileExt(char* fileName/*const std::string& s*/)
{
	std::string fileName_S = std::string(fileName);
	size_t i = fileName_S.rfind('.', fileName_S.length());
	if (i != std::string::npos) {
		return(fileName_S.substr(i+1, fileName_S.length() - i));
	}

	return("");
}


// Method: Compress
//************************************************************
// Purpose: use 2 bits to represent 4 letters in binary and 
//          pack into a byte
//
// Implementation Notes: 'a'/'A' = 00
//			 'c'/'C' = 01
//			 'g'/'G' = 10
//			 't'/'T' = 11
//
//************************************************************
int Transform::Compress(char* fileName/*, uint8_t letter, short& lines, int32_t& t_letters*/)
{
	//
	// Author: Rachid Ounit, Regents of University of California, Riverside
	// Class: CS 100, Summer Session II, 2015.
	//

	// The following C++ instructions does not compile and are just 
	// extracted code. Please see these lines as guidelines about how 
	// to create and close a mapping from a file named "filename" (const char *).
	// This mapping allows you to read only a file and have access to
	// its content through a pointer, "pmap".

	// Create a variable, fileSize, to store the size of the file
	uint32_t fileSize = 0;

	// Open the file in READ ONLY mode
	int fd = open(fileName/*filename*/, O_RDONLY);

	// Estimate the size of the file and store it in “fileSize”
	std::ifstream in(fileName/*filename*/, std::ios::binary | std::ios::ate);
	fileSize = in.tellg();

	// Check if the file can be accessed and is not empty
	if (fd == -1 || fileSize == 0)
	{
		std::cerr << "Failed to open " << fileName/*filename*/ << std::endl;
		return false;
	}


	// Map the full content of the file (fileSize) with flag: PROT_READ, MAP_PRIVATE
	uint8_t *pmap = NULL;
	pmap = (uint8_t*) mmap(0, fileSize, PROT_READ, MAP_PRIVATE, fd, 0);

	// Check if the mapping produced an error
	if ( pmap == MAP_FAILED )
	{
		close(fd);
		std::cerr << "Failed to mmapping the file." << std::endl;
		return false;
	}

	// . . .
	// this is where I think compressing/decompressing should occur 
	// . . .


	//if the filename has the extension .fna then, compress the file
	if(getFileExt(fileName) == "fna")
	{
		//add the appropriate extension to the .fna file
		std::string old_fileName = fileName;
		std::string new_extFileName = old_fileName + ".cds";


		//current number of consecutive 'a, c, g, t' characters in line
		uint8_t letter_counter = 0;

		//total number of 'a, c, g, t' characters in the entire file
		uint32_t tot_lettNum = 0;

		//number of lines in the file
		short numLines = 0;


		//number of 'a, c, g, t' per line
		short char_perLine = 0;
		short temp_perLine = 0;

		//scan file...
		for(int i = 0; i < fileSize; i++)
		{
			if(pmap[i] == 'a' || pmap[i] == 'A')
			{
				tot_lettNum++;
				temp_perLine++;
			}
			else if(pmap[i] == 'c' || pmap[i] == 'C')
			{
				tot_lettNum++;
				temp_perLine++;
			}
			else if(pmap[i] == 'g' || pmap[i] == 'G')
			{
				tot_lettNum++;
				temp_perLine++;
			}
			else if(pmap[i] == 't' || pmap[i] == 'T')
			{
				tot_lettNum++;
				temp_perLine++;
			}
			else if(pmap[i] == '\n')
			{

				if(temp_perLine >= char_perLine)
				{
					char_perLine = temp_perLine++;
					temp_perLine = 0;
				}

				//don't increment counter for a newline
				//increment numLines
				numLines++;
			}

			//if there are other letters, do we print, "a corrupt file"?
		}


		std::cout << "TOTAL LETTERS: " << tot_lettNum << std::endl;
		std::cout << "LETTERS PER LINE: " << char_perLine << std::endl;

		uint32_t *total = &tot_lettNum; 
		short *lines = &char_perLine;

		std::cout << "VALUE OF TOTAL POINTER: " << *total << std::endl;
		std::cout << "VALUE OF LINES POINTER: " << *lines << std::endl;

		std::bitset<32> test1(*total);
		std::bitset<16> test2(*lines);

		std::cout << "BINARY FOR TOTAL LETTERS: " << test1 << std::endl;
		std::cout << "BINARY FOR NUMBER OF LETTERS: " << test2 << std::endl;

		FILE *fp;
		fp = fopen(new_extFileName.c_str(), "a");
		fwrite(total,sizeof(uint32_t),1,fp);
		fwrite(lines,sizeof(short),1,fp);



		//string to hold a set of 4 letters
		std::string letter_4 = "";

		//temp counter for the number of characters left in the file (used for assigning bits to the variable)
		uint32_t total_char = tot_lettNum;	

		//scan file...
		for(int i = 0; i < fileSize; i++)
		{
			//group 4 letters and shift appropriate value
			if (letter_counter == 4 && total_char >= 4)
			{
				//do encoding stuff...
				std::cout << "ENCODING STUFF" << std::endl;

				//declare an 8 bit int to store values into
				uint8_t magicbox = 0;

				//scan the string and swap letters for bits
				std::cout << "STRING OF LETTERS: " << letter_4 << std::endl;
				for(unsigned int i = 0; i < letter_4.size(); i++)
				{
					if(letter_4.at(i) == 'a' || letter_4.at(i) == 'A')
					{
						if( i == (letter_4.size() - 1) )
						{
							magicbox = (magicbox | 0x00);
						}
						else
						{
							magicbox = (magicbox | 0x00) << 2;
						}

						std::bitset<8> magicbox1(magicbox);
						std::cout << "MAGICBOX CONTAINS: " << magicbox1 << std::endl;
					}
					else if(letter_4.at(i) == 'c' || letter_4.at(i) == 'C')
					{
						if( i == (letter_4.size() - 1) )
						{
							magicbox = (magicbox | 0x01);
						}
						else
						{
							magicbox = (magicbox | 0x01) << 2;
						}

						std::bitset<8> magicbox1(magicbox);
						std::cout << "MAGICBOX CONTAINS: " << magicbox1 << std::endl;
					}
					else if(letter_4.at(i) == 'g' || letter_4.at(i) == 'G')
					{
						if( i == (letter_4.size() - 1) )
						{
							magicbox = (magicbox | 0x02);
						}
						else
						{
							magicbox = (magicbox | 0x02) << 2;
						}

						std::bitset<8> magicbox1(magicbox);
						std::cout << "MAGICBOX CONTAINS: " << magicbox1 << std::endl;
					}
					else if(letter_4.at(i) == 't' || letter_4.at(i) == 'T')
					{
						if( i == (letter_4.size() - 1) )
						{
							magicbox = (magicbox | 0x03);
						}
						else
						{
							magicbox = (magicbox | 0x03) << 2;
						}

						std::bitset<8> magicbox1(magicbox);
						std::cout << "MAGICBOX CONTAINS: " << magicbox1 << std::endl;
					}
				}

				std::bitset<8> magicbox1(magicbox);
				std::cout << "MAGICBOX FINAL: " << magicbox1 << std::endl;

				uint8_t* ultramagic = &magicbox;
				fwrite(ultramagic, sizeof(uint8_t), 1, fp);

				total_char -= 4;
				std::cout << "TOTAL REMAINING CHARACTERS: " << total_char << std::endl;
				//subtract 4 because you've gone through 4 characters

				//reset counter			
				letter_counter = 0;

				//reset string
				letter_4 = "";
			}
			else if (total_char < 4 && (letter_counter == total_char))
			{
				std::cout << "******************************: " << std::endl;
				std::cout << "REMAINING LITERAL CHARACTERS: " << letter_4 << std::endl;
				std::cout << "TOTAL REMAINING CHARACTERS: " << total_char << std::endl;	
				//declare an 8 bit int to store values into
				uint8_t magicbox = 0;

				//scan the string and swap letters for bits


				for(unsigned int i = 0; i < letter_4.size(); i++)
				{
					if(letter_4.at(i) == 'a' || letter_4.at(i) == 'A')
					{
						if( i == (letter_4.size() - 1) )
						{
							magicbox = (magicbox | 0x00);
						}
						else
						{
							magicbox = (magicbox | 0x00) << 2;
						}
						//magicbox = (magicbox | 0x00) << 2;

						std::bitset<8> magicbox1(magicbox);
						std::cout << "MAGICBOX CONTAINS: " << magicbox1 << std::endl;
					}
					else if(letter_4.at(i) == 'c' || letter_4.at(i) == 'C')
					{
						if( i == (letter_4.size() - 1) )
						{
							magicbox = (magicbox | 0x01);
						}
						else
						{
							magicbox = (magicbox | 0x01) << 2;
						}
						//magicbox = (magicbox | 0x01) << 2;

						std::bitset<8> magicbox1(magicbox);
						std::cout << "MAGICBOX CONTAINS: " << magicbox1 << std::endl;
					}
					else if(letter_4.at(i) == 'g' || letter_4.at(i) == 'G')
					{
						if( i == (letter_4.size() - 1) )
						{
							magicbox = (magicbox | 0x02);
						}
						else
						{
							magicbox = (magicbox | 0x02) << 2;
						}
						//magicbox = (magicbox | 0x02) << 2;

						std::bitset<8> magicbox1(magicbox);
						std::cout << "MAGICBOX CONTAINS: " << magicbox1 << std::endl;
					}
					else if(letter_4.at(i) == 't' || letter_4.at(i) == 'T')
					{
						if( i == (letter_4.size() - 1) )
						{
							magicbox = (magicbox | 0x03);
						}
						else
						{
							magicbox = (magicbox | 0x03) << 2;
						}
						//magicbox = (magicbox | 0x03) << 2;

						std::bitset<8> magicbox1(magicbox);
						std::cout << "MAGICBOX CONTAINS: " << magicbox1 << std::endl;
					}
				}


				//output bit set to file
				std::bitset<8> magicbox1(magicbox);
				std::cout << "MAGICBOX FINAL: " << magicbox1 << std::endl;;

				uint8_t* ultramagic = &magicbox;
				fwrite(ultramagic, sizeof(uint8_t), 1, fp);
			} 
			if(pmap[i] == 'a' || pmap[i] == 'A')
			{
				letter_counter++;	
				letter_4+=pmap[i];
			}
			else if(pmap[i] == 'c' || pmap[i] == 'C')
			{
				letter_counter++;
				letter_4+=pmap[i];
			}
			else if(pmap[i] == 'g' || pmap[i] == 'G')
			{
				letter_counter++;
				letter_4+=pmap[i];
			}
			else if(pmap[i] == 't' || pmap[i] == 'T')
			{
				letter_counter++;
				letter_4+=pmap[i];
			}
			else if(pmap[i] == '\n')
			{
				//continue if newline is spotted
				continue;
			}

			//if there are other letters, do we print, "a corrupt file"?
		}

		//close the file when done
		fclose(fp);

		//upon sucessfully encoding, delete old .fna file
		// delete code here...

		/*int status;
		  status = remove(fileName);

		  if(status != 0)
		  {
		  printf("Unable to delete file, %s", fileName);
		  perror("Error");
		  }*/
	}
	//else if the filename has the extension .cds then, decompress the file
	else if(getFileExt(fileName) == "cds")
	{
		//rename the file to have the extension .fna
		std::string old_fileName = fileName;
		std::string new_extFileName;

		//find the substring up until the .cds extension part is found
		std::string::size_type pos = old_fileName.find(".cds");
		if (pos != std::string::npos)
		{
			new_extFileName = old_fileName.substr(0, pos);
		}


		uint32_t totLetters = 0;	
		short lettersPerLine = 0;

		FILE *fr = NULL;

		fr = fopen(old_fileName.c_str(), "r");
		fread(&totLetters, sizeof(uint32_t), 1, fr);
		fread(&lettersPerLine, sizeof(short), 1, fr);


		std::cout << "TOTAL LETTERS: " << totLetters << std::endl;
		std::cout << "LETTERS PER LINE: " << lettersPerLine << std::endl;

		uint32_t total_char_CP = totLetters;

		//1 byte buffer
		uint8_t buffer;
		while( fread(&buffer, sizeof(uint8_t), 1, fr) ){
			std::cout << size_t(buffer) << std::endl;
			uint8_t temp = 0;
			std::string letters = "";
			std::cout << "TOTAL LETTERS LEFT: " << total_char_CP << std::endl;
			for(int i = 0; i < 8; i++)
			{
				//IF ever, i = 0, then mask with: 0x03
				if(i == 0)
				{
					temp = (buffer & 0x03);
					std::cout << "TEMP: " << size_t(temp) << std::endl;
					std::bitset<8> temper(temp);
					if(temp == 0x00 && (total_char_CP > 3) ) letters.insert(0, "A");
					else if(temp == 0x01 /*&& (total_char_CP > 0)*/ ) letters.insert(0, "C");
					else if(temp == 0x02 /*&& (total_char_CP > 0)*/ ) letters.insert(0, "G");
					else if(temp == 0x03 /*&& (total_char_CP > 0)*/ ) letters.insert(0, "T");
					std::cout << "CURRENT LETTERS: " << letters << std::endl;
					total_char_CP -= 1;
				}
				//IF ever, i = 1, then mask with: 0x0C
				else if(i == 1)
				{
					temp = (buffer & 0x0C) >> 2;
					std::cout << "TEMP: " << size_t(temp) << std::endl;
					std::bitset<8> temper(temp);
					if(temp == 0x00 && (total_char_CP > 3) ) letters.insert(0, "A");
					else if(temp == 0x01 /*&& (total_char_CP > 0)*/ ) letters.insert(0, "C");
					else if(temp == 0x02 /*&& (total_char_CP > 0)*/ ) letters.insert(0, "G");
					else if(temp == 0x03 /*&& (total_char_CP > 0)*/ ) letters.insert(0, "T");
					std::cout << "CURRENT LETTERS: " << letters << std::endl;
					total_char_CP -= 1;
				}
				//IF ever, i = 2, then mask with: 0x30
				else if(i == 2)
				{
					temp = (buffer & 0x30) >> 4;
					std::cout << "TEMP: " << size_t(temp) << std::endl;
					std::bitset<8> temper(temp);
					if(temp == 0x00 && (total_char_CP > 3) ) letters.insert(0, "A");
					else if(temp == 0x01 /*&& (total_char_CP > 0)*/ ) letters.insert(0, "C");
					else if(temp == 0x02 /*&& (total_char_CP > 0)*/ ) letters.insert(0, "G");
					else if(temp == 0x03 /*&& (total_char_CP > 0)*/ ) letters.insert(0, "T");
					std::cout << "CURRENT LETTERS: " << letters << std::endl;
					total_char_CP -= 1;
				}
				//IF ever, i = 3, then mask with: 0xC0
				else if(i == 3)
				{
					temp = (buffer & 0xC0) >> 6;
					std::cout << "TEMP: " << size_t(temp) << std::endl;
					std::bitset<8> temper(temp);
					if(temp == 0x00 && (total_char_CP > 3) ) letters.insert(0, "A");
					else if(temp == 0x01 /*&& (total_char_CP > 0)*/ ) letters.insert(0, "C");
					else if(temp == 0x02 /*&& (total_char_CP > 0)*/ ) letters.insert(0, "G");
					else if(temp == 0x03 /*&& (total_char_CP > 0)*/ ) letters.insert(0, "T");
					std::cout << "CURRENT LETTERS: " << letters << std::endl;
					total_char_CP -= 1;
				}
			}
		}



		//close file
		fclose(fr);


		/*FILE* fp;
		fp = fopen(old_fileName.c_str(), "r");
		//get the offset numbers and IGNORE them
		uint32_t garbage_value1 = 0;
		short garbage_value2 = 0;
		fread(&garbage_value1, sizeof(uint32_t), 1, fp);
		fread(&garbage_value2, sizeof(short), 1, fp);
		std::cout << "GARBAGE VALUE 1: " << garbage_value1 << std::endl;
		std::cout << "GARBAGE VALUE 2: " << garbage_value2 << std::endl;

		//scan the binary file to extract
		for(int i = 0; i < fileSize; i++){
			// binary: 0000 0000
			//uint8_t buffer = 0;	

			//if there are less than 4 letters, you DON'T need a '\n' char
			if(totLetters > 0)
			{
				uint8_t buffer = 0;	
				//grab however many letters;
				fread(&buffer, sizeof(uint8_t), 1, fp);
				std::cout << "LETTER SET: " << size_t(buffer) << std::endl;

				//string of letters to append to file
				std::string letters = "";

				//scan the 8 bit set
				//you right shift as many times as there are numbers left
				//this ensure you are getting all the correct letters
				//however they will be backwards, so you have to prepend each one
				//char for getting each letters value
				uint8_t temp = 0;

				for(unsigned int i = 0; i < totLetters; i++)
				{
					//IF ever, i = 0, then mask with: 0x03
					if(i == 0)
					{
						temp = (buffer & 0x03);
						std::cout << "TEMP: " << size_t(temp) << std::endl;
						std::bitset<8> temper(temp);
						std::cout << "TEMP IN BINARY: " << std::endl;
						if(temp == 0x00) letters.insert(0, "A");
						else if(temp == 0x01) letters.insert(0, "C");
						else if(temp == 0x02) letters.insert(0, "G");
						else if(temp == 0x03) letters.insert(0, "T");
						std::cout << "CURRENT LETTERS: " << letters << std::endl;
					}
					//IF ever, i = 1, then mask with: 0x0C
					else if(i == 1)
					{
						temp = (buffer & 0x0C) >> 2;
						std::cout << "TEMP: " << size_t(temp) << std::endl;
						std::bitset<8> temper(temp);
						std::cout << "TEMP IN BINARY: " << std::endl;
						if(temp == 0x00) letters.insert(0, "A");
						else if(temp == 0x01) letters.insert(0, "C");
						else if(temp == 0x02) letters.insert(0, "G");
						else if(temp == 0x03) letters.insert(0, "T");
						std::cout << "CURRENT LETTERS: " << letters << std::endl;
					}
					//IF ever, i = 2, then mask with: 0x30
					else if(i == 2)
					{
						temp = (buffer & 0x30) >> 4;
						std::cout << "TEMP: " << size_t(temp) << std::endl;
						std::bitset<8> temper(temp);
						std::cout << "TEMP IN BINARY: " << std::endl;
						if(temp == 0x00) letters.insert(0, "A");
						else if(temp == 0x01) letters.insert(0, "C");
						else if(temp == 0x02) letters.insert(0, "G");
						else if(temp == 0x03) letters.insert(0, "T");
						std::cout << "CURRENT LETTERS: " << letters << std::endl;
					}
					//IF ever, i = 3, then mask with: 0xC0
					else if(i == 3)
					{
						temp = (buffer & 0xC0) >> 6;
						std::cout << "TEMP: " << size_t(temp) << std::endl;
						std::bitset<8> temper(temp);
						std::cout << "TEMP IN BINARY: " << std::endl;
						if(temp == 0x00) letters.insert(0, "A");
						else if(temp == 0x01) letters.insert(0, "C");
						else if(temp == 0x02) letters.insert(0, "G");
						else if(temp == 0x03) letters.insert(0, "T");
						std::cout << "CURRENT LETTERS: " << letters << std::endl;
					}
				}

				//output string to file
				std::cout << "LETTER SET: " << letters << std::endl;	

				//close the file
				fclose(fp);
			}
		}*/



		//make sure to REMOVE THE .cds file
	}
	else
	{
		std::cout << "Please input either a .fna or .cds file" << std::endl;
	}


	// Update/Apply changes to be made to the file before closing
	msync(pmap, fileSize, MS_SYNC);

	// Delete the mapping 
	if (munmap(pmap, fileSize) == -1)
	{
		std::cerr << "Error un-mmapping the file." << std::endl;
		return false;
	}
	// close the file
	close(fd);
}
