////////////////////////////////////////////////////////////////////////
// Image to MSX tiles
//
// Manuel Martinez (salutte@gmail.com)
//
// FLAGS: -std=gnu++14 -g `pkg-config opencv --cflags --libs` -O3 -lpthread

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <functional>

using namespace std::chrono_literals;
 
static const std::vector<cv::Vec3b> colorsRGB = {
	{   0,    0,    0},
	{  33,  200,   66},
	{  94,  220,  120},
	{  84,   85,  237},
	{ 125,  118,  252},
	{ 212,   82,   77},
	{  66,  235,  245},
	{ 252,   85,   84},
	{ 255,  121,  120},
	{ 212,  193,   84},
	{ 230,  206,  128},
	{  33,  176,   59},
	{ 201,   91,  186},
	{ 204,  204,  204},
	{ 255,  255,  255}	
};

static const std::vector<cv::Vec3b> colors = {
	{   0,    0,    0},
	{  66,  200,   33},
	{ 120,  220,   94},
	{ 237,   85,   84},
	{ 252,  118,  125},
	{  77,   82,  212},
	{ 245,  235,   66},
	{  84,   85,  252},
	{ 120,  121,  255},
	{  84,  193,  212},
	{ 128,  206,  230},
	{  59,  176,   33},
	{ 186,   91,  201},
	{ 204,  204,  204},
	{ 255,  255,  255}	
};


static std::pair<double,std::array<uint8_t,8>> colorize(
	const cv::Vec3b src[8], 
	cv::Vec3b target[8], 
	std::vector<cv::Vec3b> &palette) {
	
	std::pair<double,std::array<uint8_t,8>> ret;
	
	ret.first = 0;
	for (size_t i=0; i<8; i++) {
		
		double bestValue = 1e100;
		for (size_t k=0; k<palette.size(); k++) {
			double val = 
				(int(src[i][0]) - palette[k][0]) * (int(src[i][0]) - palette[k][0]) +
				(int(src[i][1]) - palette[k][1]) * (int(src[i][1]) - palette[k][1]) +
				(int(src[i][2]) - palette[k][2]) * (int(src[i][2]) - palette[k][2])
				;
			if (val < bestValue) {
				bestValue = val;
				target[i] = palette[k];
				ret.second[i] = k;
			}
		}
		ret.first += bestValue;
	}
	return ret;
}


static std::pair<size_t,std::array<uint8_t,8>> quantize(
	const cv::Vec3b src[8], 
	cv::Vec3b target[8], 
	std::vector<std::vector<cv::Vec3b>> &palettes) {

	std::map<double,std::pair<size_t,std::array<uint8_t,8>>> scores;
	
	for (size_t j=0; j<palettes.size(); j++) {
		
		std::pair<double,std::array<uint8_t,8>> sp = colorize(src,target,palettes[j]);
		scores[sp.first] = std::make_pair(j,sp.second);
	}

	colorize(src,target,palettes[scores.begin()->second.first]);

	return scores.begin()->second;
}


std::vector<std::array<cv::Vec3b,2>> palette;
std::vector<uint8_t> paletteCode;
std::map<uint8_t,std::array<cv::Vec3b,2>> paletteMap;

typedef std::array<uint8_t,8>  U8x8;
struct Tile {
	
	U8x8 pattern;
	U8x8 color;

	cv::Mat3b img() const {
				
		cv::Mat3b ret(8,8);
		for (size_t i=0; i<8; i++) {
			auto &p = paletteMap[color[i]];
					
			for (size_t j=0; j<8; j++) 
				ret(i,7-j) = p[!!(pattern[i] & (1<<j))];
		}
		return ret;
	}
	
	Tile(const U8x8 &pattern, const U8x8 &color) : pattern(pattern), color(color) {}
	 
	Tile(uint8_t p=0, uint8_t c=0) {
		pattern.fill(p);
		color.fill(c);
	}
	
	bool operator<(const Tile &rhs) const { 
		if (pattern==rhs.pattern) return color < rhs.color;
		return pattern < rhs.pattern;
	}
};

static std::pair<
	cv::Mat2i,
	std::map<size_t, Tile> > quantize0( const cv::Mat3b src, cv::Mat3b target) {
		
	
	auto comp = [](const cv::Vec3b &p0, const cv::Vec3b &p1, const cv::Vec3b &p2) {
		
		return	(p0[0]+p1[0]-2*p2[0])*(p0[0]+p1[0]-2*p2[0]) +
				(p0[1]+p1[1]-2*p2[1])*(p0[1]+p1[1]-2*p2[1]) +
				(p0[2]+p1[2]-2*p2[2])*(p0[2]+p1[2]-2*p2[2]);
	};


	std::map<size_t, Tile> tiles;

	std::vector<cv::Vec3b> colorCombos;
	for (size_t i=0; i<colors.size(); i++)
		for (size_t j=i; j<colors.size(); j++)
			colorCombos.push_back(0.5*colors[i]+0.5*colors[i]);
	
	for (size_t i=0; i<colors.size(); i++) {
		for (size_t j=i+1; j<colors.size(); j++) {
			palette.emplace_back(std::array<cv::Vec3b,2>{colors[i],colors[j]});
			paletteCode.emplace_back(16*(j  + 1) + i  + 1);			
			paletteMap[paletteCode.back()] = palette.back();
		}
	}
	
	

	for (size_t i=0; i<colors.size()-1; i++)
		tiles[i] = Tile(0,240+i+1);
	tiles[14] = Tile(255,254);
	
	cv::Mat2i tileMap(src.rows/8,src.cols/8);
	for (int i=0; i<src.rows; i+=8) {
		for (int j=0; j<src.cols; j+=8) {

			Tile bestTile, bestBase;
			double bestErrorTile = 1e100;

			//for (int t=0; t<15; t++) { // Selects base tile;

			cv::Vec3b backGround;
			for (auto &bc : colorCombos) {

				Tile tile;
				double errorTile = 0;
				for (int ii=0; ii<8; ii++) { // Selects row;
					
					double bestErrorLine = 1e100;
					for (size_t p=0; p<palette.size(); p++) {

						double errorLine = 0;
						uint8_t pattern = 0;
						for (int jj=0; jj<8; jj++) {
							
//							double v0 = comp(colors[t], palette[p][0], src(i+ii,j+jj));
//							double v1 = comp(colors[t], palette[p][1], src(i+ii,j+jj));

							double v0 = comp(bc, palette[p][0], src(i+ii,j+jj));
							double v1 = comp(bc, palette[p][1], src(i+ii,j+jj));

							if (v0<v1) {
								pattern = (pattern<<1);
								errorLine += v0;
							} else {
								pattern = (pattern<<1) + 1;
								errorLine += v1;
							}
						}
						
						if (errorLine<bestErrorLine) {
							bestErrorLine = errorLine;
							tile.pattern[ii] = pattern; 
							tile.color[ii] = paletteCode[p];
						}
					}
					errorTile += bestErrorLine;
				}
				
				if (errorTile<bestErrorTile) {
					bestErrorTile = errorTile;
					bestTile = tile;
					//bestBase = tiles[t];
					backGround = bc;
				} 
			}
			
			tiles[tiles.size()] = bestTile;
			target(cv::Rect(j,i,8,8)) = cv::Mat3b(0.5*bestTile.img()) + cv::Mat3b(8,8,0.5*backGround);
		}
	}
	
	{
		
		std::map<Tile, size_t> tt;
		for (auto &t : tiles) 
			tt[t.second]++;
			
		std::cout << "Size: " << tt.size() << " " << tiles.size() << std::endl;
	}

	return std::make_pair(tileMap, tiles);

}
	

static std::pair<
	cv::Mat2i,
	std::map<size_t, Tile> > quantize1( const cv::Mat3b src, cv::Mat3b target) {
		
	
	auto comp = [](const cv::Vec3b &p0, const cv::Vec3b &p1, const cv::Vec3b &p2) {
		
		return	(p0[0]+p1[0]-2*p2[0])*(p0[0]+p1[0]-2*p2[0]) +
				(p0[1]+p1[1]-2*p2[1])*(p0[1]+p1[1]-2*p2[1]) +
				(p0[2]+p1[2]-2*p2[2])*(p0[2]+p1[2]-2*p2[2]);
	};


	std::map<size_t, Tile> tiles;

	std::vector<cv::Vec3b> colorCombos;
	for (size_t i=0; i<colors.size(); i++)
		for (size_t j=i; j<colors.size(); j++)
			colorCombos.push_back(0.5*colors[i]+0.5*colors[j]);
	
	for (size_t i=0; i<colors.size(); i++) {
		for (size_t j=i+1; j<colors.size(); j++) {
			palette.emplace_back(std::array<cv::Vec3b,2>{colors[i],colors[j]});
			paletteCode.emplace_back(16*(j  + 1) + i  + 1);			
			paletteMap[paletteCode.back()] = palette.back();
		}
	}
	
	

	for (size_t i=0; i<colors.size()-1; i++)
		tiles[i] = Tile(0,240+i+1);
	tiles[14] = Tile(255,254);
	
	cv::Mat2i tileMap(src.rows/8,src.cols/8);
	for (int i=0; i<src.rows; i+=8) {
		for (int j=0; j<src.cols; j+=8) {

			Tile bestTile, bestBase;
			double bestErrorTile = 1e100;

			//for (int t=0; t<15; t++) { // Selects base tile;

			cv::Vec3b backGround;
			for (auto &bc : colorCombos) {

				Tile tile;
				double errorTile = 0;
				for (int ii=0; ii<8; ii++) { // Selects row;
					
					double bestErrorLine = 1e100;
					for (size_t p=0; p<palette.size(); p++) {

						double errorLine = 0;
						uint8_t pattern = 0;
						for (int jj=0; jj<8; jj++) {
							
//							double v0 = comp(colors[t], palette[p][0], src(i+ii,j+jj));
//							double v1 = comp(colors[t], palette[p][1], src(i+ii,j+jj));

							double v0 = comp(bc, palette[p][0], src(i+ii,j+jj));
							double v1 = comp(bc, palette[p][1], src(i+ii,j+jj));

							if (v0<v1) {
								pattern = (pattern<<1);
								errorLine += v0;
							} else {
								pattern = (pattern<<1) + 1;
								errorLine += v1;
							}
						}
						
						if (errorLine<bestErrorLine) {
							bestErrorLine = errorLine;
							tile.pattern[ii] = pattern; 
							tile.color[ii] = paletteCode[p];
						}
					}
					errorTile += bestErrorLine;
				}
				
				if (errorTile<bestErrorTile) {
					bestErrorTile = errorTile;
					bestTile = tile;
					//bestBase = tiles[t];
					backGround = bc;
				} 
			}
			
			tiles[tiles.size()] = bestTile;
			target(cv::Rect(j,i,8,8)) = cv::Mat3b(0.5*bestTile.img()) + cv::Mat3b(8,8,0.5*backGround);
		}
	}
	
	{
		
		std::map<Tile, size_t> tt;
		for (auto &t : tiles) 
			tt[t.second]++;
			
		std::cout << "Size: " << tt.size() << " " << tiles.size() << std::endl;
	}

	return std::make_pair(tileMap, tiles);

}
	



int main(int argc, char *argv[]) {
	
	if (argc!=2) {
		std::cout << "Use: ./mkImage <file.png>" << std::endl;
		return -1;
	}

	cv::Mat3b img = cv::imread(argv[1]);
	
	if (img.rows%8) throw std::runtime_error("Row size not multiple of 8");
	if (img.cols%8) throw std::runtime_error("Col size not multiple of 8");
	
	std::cerr << img.rows << " " << img.cols << std::endl;
	
	cv::imshow("original", img);
	
	std::vector<std::vector<cv::Vec3b>> msx1palette;
	std::vector<std::array<uint8_t,2>> msx1paletteValues;
	for (size_t i=0; i<colors.size(); i++) {
		for (size_t j=i+1; j<colors.size(); j++) {
			msx1palette.emplace_back(std::vector<cv::Vec3b>{colors[i],colors[j]});
			msx1paletteValues.emplace_back(std::array<uint8_t,2> {
				16*(j  + 1) + i  + 1,
				0
			});
		}
	}

	std::vector<std::vector<cv::Vec3b>> msx1dualpalette;
	std::vector<std::array<uint8_t,2>> msx1dualpaletteValues;
	for (size_t i=0; i<colors.size(); i++) {
		for (size_t j=0; j<colors.size(); j++) {
			for (size_t ii=0; ii<colors.size(); ii++) {
				for (size_t jj=0; jj<colors.size(); jj++) {
					if (i+jj < i+ii) continue;
					if (j+ii < i+jj) continue;
					if (j+jj < j+ii) continue;
					msx1dualpalette.emplace_back(std::vector<cv::Vec3b>{
						0.5*colors[i]+0.5*colors[ii],
						0.5*colors[i]+0.5*colors[jj],
						0.5*colors[j]+0.5*colors[ii],
						0.5*colors[j]+0.5*colors[jj],
					});
					msx1dualpaletteValues.emplace_back(std::array<uint8_t,2> {
						16*(j  + 1) + i  + 1,
						16*(jj + 1) + ii + 1
					});
				}
			}
		}
	}

	std::vector<std::vector<cv::Vec3b>> msx1dualpalette2;
	std::vector<std::array<uint8_t,2>> msx1dualpaletteValues2;
	for (size_t i=0; i<colors.size(); i++) {
		for (size_t j=0; j<colors.size(); j++) {
			for (size_t ii=0; ii<colors.size(); ii++) {
				for (size_t jj=0; jj<colors.size(); jj++) {
					if (i+jj < i+ii) continue;
					if (j+ii < i+jj) continue;
					if (ii != jj) continue;
					msx1dualpalette2.emplace_back(std::vector<cv::Vec3b>{
						0.5*colors[i]+0.5*colors[ii],
						0.5*colors[i]+0.5*colors[jj],
						0.5*colors[j]+0.5*colors[ii],
						0.5*colors[j]+0.5*colors[jj],
					});
					msx1dualpaletteValues2.emplace_back(std::array<uint8_t,2> {
						16*(j  + 1) + i  + 1,
						16*(jj + 1) + ii + 1
					});
				}
			}
		}
	}
	
	cv::Mat3b work = img.clone();
	cv::Mat3b target;
	
	quantize0(img, work);
	cv::resize(work, target, cv::Size(), 4, 4, cv::INTER_NEAREST);
	cv::imshow("target", target); cv::waitKey(0);
	quantize1(img, work);
	cv::resize(work, target, cv::Size(), 4, 4, cv::INTER_NEAREST);
	cv::imshow("target", target); cv::waitKey(0);
	quantize0(img, work);
	cv::resize(work, target, cv::Size(), 4, 4, cv::INTER_NEAREST);
	cv::imshow("target", target); cv::waitKey(0);
	quantize1(img, work);
	cv::resize(work, target, cv::Size(), 4, 4, cv::INTER_NEAREST);
	cv::imshow("target", target); cv::waitKey(0);
	quantize0(img, work);
	cv::resize(work, target, cv::Size(), 4, 4, cv::INTER_NEAREST);
	cv::imshow("target", target); cv::waitKey(0);
	quantize1(img, work);
	cv::resize(work, target, cv::Size(), 4, 4, cv::INTER_NEAREST);
	cv::imshow("target", target); cv::waitKey(0);
	
	volatile bool alive = true;
	volatile int key=0;
	std::thread t{[&](){
		while (alive) {
			cv::resize(work, target, cv::Size(), 4, 4, cv::INTER_NEAREST);
			cv::imshow("target", target);
			key = cv::waitKey(500);
		}
	}};
			
			
	int status = 0;
	while (true) {
		
		std::this_thread::sleep_for(100ms);
		
		if (key == -1) continue;

		status = ((key&255)-'0')%10;
		std::cout << status << std::endl;
		
		
		typedef std::array<U8x8,2>  MTile;
		std::map<MTile,int> tileCounts;

		auto pv = msx1paletteValues;
		if (status==2) pv = msx1dualpaletteValues;
		if (status==3) pv = msx1dualpaletteValues2;

		for (int i=0; i<img.rows; i+=8) {
			for (int j=0; j<img.cols; j+=8) {
				
				MTile t0, t1;
				for (int ii=0; ii<8; ii++) {
					
					std::pair<size_t,std::array<uint8_t,8>> Q;
					if (status==1) Q = quantize(&img(i+ii,j), &work(i+ii,j), msx1palette);
					if (status==2) Q = quantize(&img(i+ii,j), &work(i+ii,j), msx1dualpalette);
					if (status==3) Q = quantize(&img(i+ii,j), &work(i+ii,j), msx1dualpalette2);
						
					t0[1][ii] = pv[Q.first][0];
					t1[1][ii] = pv[Q.first][1];
					for (int jj=0; jj<8; jj++) {
						t0[0][ii] += (!!(Q.second[jj]&0x01))<<jj;
						t1[0][ii] += (!!(Q.second[jj]&0x02))<<jj;
					}
				}
				tileCounts[t0]++;
				tileCounts[t1]++;
			}
		}
		
		std::cout << tileCounts.size() << std::endl;
	}
	
		
	


/*	std::string name = argv[1];
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
	return 0;
}
