#include "./Transform.hh"
#include <iostream>
#include <string>


int main(int argc, char** argv){
	Transform obj;              /*create a transform object*/
	obj.Compress(argv[1]);      /*pass the file into the Compression method*/
	return 0;
}
