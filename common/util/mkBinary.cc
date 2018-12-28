////////////////////////////////////////////////////////////////////////
// Binary to c file
//
// Manuel Martinez (salutte@gmail.com)
//
// FLAGS: -std=c++14 -g

#include <fstream>
#include <iostream>
#include <vector>

int main(int argc, char *argv[]) {
	
	if (argc!=2) {
		std::cout << "Use: ./mkBinary <file.png>" << std::endl;
		return -1;
	}

	std::ifstream iff(argv[1]);
	
	std::string name = argv[1];
	for (auto &&s : "_!-+/\\.'() ")
		for (auto &&c : name)
			if (c==s) c = '_';
	

	std::cout << "// Binary file generated from:" << argv[1] << std::endl;
	std::cout << "#include <stdint.h>" << std::endl;
	std::cout << "const uint8_t " << name << "[] = {" << std::endl;
	size_t c = 0;
	while (iff) {
		
		int v = iff.get();
		if (not iff) break;
		printf("0x%02X,",v);
		if (++c%16==0) printf("\n");
	}
	std::cout << "};" << std::endl;
	
	return 0;
}
