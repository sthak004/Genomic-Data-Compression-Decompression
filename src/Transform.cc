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
std::string Transform::getFileExt(char* fileName)
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
int Transform::Compress(char* fileName)
{
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

		}

		//close the file when done
		fclose(fp);
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
					else if(temp == 0x01) letters.insert(0, "C");
					else if(temp == 0x02) letters.insert(0, "G");
					else if(temp == 0x03) letters.insert(0, "T");
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
					else if(temp == 0x01) letters.insert(0, "C");
					else if(temp == 0x02) letters.insert(0, "G");
					else if(temp == 0x03) letters.insert(0, "T");
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
					else if(temp == 0x01) letters.insert(0, "C");
					else if(temp == 0x02) letters.insert(0, "G");
					else if(temp == 0x03) letters.insert(0, "T");
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
					else if(temp == 0x01) letters.insert(0, "C");
					else if(temp == 0x02) letters.insert(0, "G");
					else if(temp == 0x03) letters.insert(0, "T");
					std::cout << "CURRENT LETTERS: " << letters << std::endl;
					total_char_CP -= 1;
				}
			}
		}

		//close file
		fclose(fr);
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
