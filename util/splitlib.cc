////////////////////////////////////////////////////////////////////////
// SPLIT LIB file to set a module per file
//
// Manuel Martinez (salutte@gmail.com)
//
// FLAGS: -std=c++14 -g

#include <fstream>
#include <iostream>
#include <vector>

struct Node {
	
	std::string tag, line;
	std::vector<Node> children;
};

static Node parse(std::string name, std::istream &src) {

	Node n;
	n.tag = name;
	std::string line;
	while (std::getline(src,line)) {

		if        (line.size()>1 and line[0]=='<' and line[1]!='/') {
			
			n.children.push_back(parse(line.substr(1,line.size()-2), src));
		} else if (line.size()>1 and line[0]=='<') {

			return n;
		} else {
			Node child;
			child.line = line;
			n.children.push_back( child );
		}
	}
	return n;
}
	
static void print(const Node &n, std::ostream &os) {
	
	if (n.tag.size()) {
		os << "<" << n.tag << ">" << std::endl;
		for (auto &&c : n.children) print(c,os);
		os << "</" << n.tag << ">" << std::endl;	
	} else {
		os << n.line << std::endl;
	}
}

int main(int argc, char *argv[]) {
	
	if (argc!=2) {
		std::cout << "Use: ./mkFont <file.lib>" << std::endl;
		return -1;
	}

	std::ifstream src(argv[1]);
	Node n = parse("", src).children.front();
	
	print(n,std::cout);
	
	return 0;
}
