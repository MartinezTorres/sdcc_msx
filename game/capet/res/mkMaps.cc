////////////////////////////////////////////////////////////////////////
// PNG to Font encoder
//
// Manuel Martinez (salutte@gmail.com)
//
// FLAGS: -std=c++14 -g `pkg-config opencv --cflags --libs`

#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <map>

typedef std::array<std::array<uint8_t,4>,4> mapTile;

struct TMap {
	
	uint trows = 0;
	uint tcols = 0;
	
	std::vector<std::vector<uint8_t>> tiles;
};

int main(int argc, char *argv[]) {
	
	if (argc<2) {
		std::cerr << "Use: ./mkMap <map1.txt> <map2.txt> <map3.txt> ..." << std::endl;
		return -1;
	}

	std::map<mapTile,size_t> tileStorage;
	std::vector<TMap> maps;
	
	for (int i=1; i<argc; i++) {

		maps.emplace_back();
		TMap &map = maps.back();
		
		std::vector<std::string> mapInfo;
		size_t rows=0, cols=0;

		std::ifstream iss(argv[i]);
		std::string line;
		while (std::getline(iss, line)) {
			if (line.size()<4) continue;
			if (line[0]=='%') continue;
			rows++;
			cols = std::max(cols,line.size());
			mapInfo.push_back(line);	
		}
		std::cerr << rows << " " << cols << std::endl;
		
		map.trows=(rows+3)/4;
		map.tcols=(cols+3)/4;

		std::cerr << map.trows << " " << map.tcols << std::endl;

		mapInfo.resize(map.trows*4);
		for (auto &&row : mapInfo)
			row.resize(map.tcols*4,'#');
		
		map.tiles = std::vector<std::vector<uint8_t>>(map.trows, std::vector<uint8_t>(map.tcols));
		
		for (size_t i=0; i<map.trows; i++) {
			for (size_t j=0; j<map.tcols; j++) {
				mapTile tile;
				for (size_t ii=0; ii<4; ii++)
					for (size_t jj=0; jj<4; jj++)
						tile[ii][jj] = mapInfo[i*4+ii][j*4+jj];
						
				if (not tileStorage.count(tile))
					tileStorage[tile] = tileStorage.size();
					
				map.tiles[i][j] = tileStorage[tile];
			}
		}
		
		std::cerr << "NTiles: " << tileStorage.size() << std::endl;
//		std::cerr << "Memory Usage: " << 3+tileStorage.size()*16+trows*tcols << std::endl;
	
	
	
/*
	std::string name = argv[1];
	name = name.substr(name.rfind('-')+1);
	name = name.substr(0,name.size()-4);
	name = "font_"+name;
	

	std::cout << "// Font file generated from:" << name.substr(name.rfind('-')+1) << std::endl;
	std::cout << "#include <stdint.h>" << std::endl;
	std::cout << "const uint8_t " << name << "[128][8] = {" << std::endl;
	for (int i=0; i<128; i++) {
		std::cout << std::hex << "\t {";
		for (int j=0; j<8; j++) {
			std::cout << (j?",":"");
			printf("0x%02X",font[i][j]);
		}
		std::cout << "} " << (i==128?' ':',');
//		if (i>31 and i<127) std::cout << " \\\\ '" << char(i) << "' ";
		std::cout << std::endl;
	}
	std::cout << "};" << std::endl;
	*/
	}
	return 0;
}
