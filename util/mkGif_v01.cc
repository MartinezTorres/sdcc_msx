////////////////////////////////////////////////////////////////////////
// Image to MSX tiles
//
// Manuel Martinez (salutte@gmail.com)
//
// FLAGS: -std=gnu++14 -g `pkg-config opencv --silence-errors --cflags --libs` `pkg-config opencv4 --silence-errors --cflags --libs` -Ofast -lpthread -fopenmp -lgomp -Wno-format-nonliteral
 
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <deque>
#include <map>
#include <thread>
#include <chrono>
#include <functional>

using namespace std::chrono_literals;

struct TileCompare {
	
	template<int N, typename T>
	class DCT {
	public:
		
		T U[N*N];
		
		DCT() {
			
			for (int i=0; i<N*N; i++)
				U[i] = M(i/N,i%N);

		};
		
		static T M(int n, int k) {

			static const long double pi = acos(-1.L);
			long double r = sqrt(.25L)*8.L/N;
			if (n==0) r = sqrt(.128L)*8.L/N;
			return T(r*cos(n*pi*(2*k+1)/(2*N)));
		}
		
		// 1D transform is the naive matrix multiplication
		template<typename Q>
		void D1D(Q *out, const Q *in, int sk=1) {
			
			for (int i=0; i<N; i++) {
				out[i*sk] = 0;
				for (int j=0; j<N; j++)
					out[i*sk] += in[j*sk]*U[N*i+j];
			}
		}

		// 2D transform is performed using row/colum decomposition
		template<typename Q>
		void D2D(Q *out, const Q *in) {

			Q tmp[N*N];
			
			for (int i=0; i<N; i++)
				D1D(tmp+i*N, in+i*N); 

			for (int i=0; i<N; i++)
				D1D(out+i, tmp+i, N); 
		}
	};

	DCT<8,float> dct;	
	
	
	static cv::Mat3b BGR2YCC(cv::Mat3b img) {
		
		//Slightly altered values to avoid clamming
		const int YR = 19595, YG = 38470, YB = 7471, CB_R = -11059, CB_G = -21709, CB_B = 32767, CR_R = 32767, CR_G = -27439, CR_B = -5329;
		
		cv::Mat3b ret = img.clone();
		for (int i=0; i<img.rows; i++) {
			for (int j=0; j<img.cols; j++) {
				const int b = img(i,j)[0], g = img(i,j)[1], r = img(i,j)[2];
				ret(i,j)[0] = (r * YR + g * YG + b * YB + 32767) >> 16;
				ret(i,j)[1] = 128 + ((r * CB_R + g * CB_G + b * CB_B + 32768) >> 16);
				ret(i,j)[2] = 128 + ((r * CR_R + g * CR_G + b * CR_B + 32768) >> 16);
			}
		}
		return ret;
	}	
	
	
	std::vector<float> getBlock(cv::Mat3b t) {
		
		// Q Tables [Y-lum|C-chroma]
		static const int Q[3][64] = {
			{ 16,11,12,14,12,10,16,14,13,14,18,17,16,19,24,40,26,24,22,22,24,49,35,37,29,40,58,51,61,60,57,51,56,55,64,72,92,78,64,68,87,69,55,56,80,109,81,87,95,98,103,104,103,62,77,113,121,112,100,120,92,101,103,99 },
			{ 17,18,18,24,21,24,47,26,26,47,99,66,56,66,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99 },
			{ 17,18,18,24,21,24,47,26,26,47,99,66,56,66,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99 } 
		};

		// ZIGZAG CODE
		static const int ZZ[]   = { 0,1,8,16,9,2,3,10,17,24,32,25,18,11,4,5,12,19,26,33,40,48,41,34,27,20,13,6,7,14,21,28,35,42,49,56,57,50,43,36,29,22,15,23,30,37,44,51,58,59,52,45,38,31,39,46,53,60,61,54,47,55,62,63 };

		t = BGR2YCC(t.clone());

		std::vector<float> B, D(64*3);

		for (int k=0; k<3; k++)
			for (auto &c : t) B.push_back(c[k]);

		for (int k=0; k<3; k++)
			dct.D2D(&D[k*64], &B[k*64]);

		for (int k=0; k<3; k++)
			for (int i=0; i<64; i++)
				D[k*64+ZZ[i]] *= 1./Q[k][i];
		
		return D;
	}
	
	double compare(cv::Mat3b t0, cv::Mat3b t1) {
		
		return cv::norm(getBlock(t0),getBlock(t1));
	}

	double compareBlock(const std::vector<float> &b0, const std::vector<float> &b1) {
		
		return cv::norm(b0,b1);
	}
	
};

TileCompare TC;

struct Colorspace {
	
	std::array<float, 256> TsRGB2lin;
	std::array<uint8_t,4096> Tlin2sRGB;

	Colorspace() {
	
		for (size_t i=0; i<256; i++) {
			
			double srgb = (i+0.5)/255.999;
			double lin = 0;
			if (srgb <= 0.0405) {
				lin = srgb/12.92;
			} else {
				lin = std::pow((srgb+0.055)/(1.055),2.4);
			}
			TsRGB2lin[i]=lin;
		}
		
		for (size_t i=0; i<4096; i++) {

			double lin = (i+0.5)/4095.999;
			double srgb = 0;
			if (lin <= 0.0031308) {
				srgb = lin*12.92;
			} else {
				srgb = 1.055*std::pow(lin,1/2.4)-0.055;
			}
			Tlin2sRGB[i]=srgb*255.999999999;
		}
	}

	cv::Vec3f sRGB2Lin(cv::Vec3b a) const {
		
		return {
			TsRGB2lin[a[0]],
			TsRGB2lin[a[1]],
			TsRGB2lin[a[2]]
		};
	}

	cv::Vec3b lin2sRGB(cv::Vec3f a) const {
	
		auto cap = [](float f){	
			int i =  std::round(f*4095.999+0.5);
			return std::min(std::max(i,0),4095); 
		};
		
		return {
			Tlin2sRGB[cap(a[0])],
			Tlin2sRGB[cap(a[1])],
			Tlin2sRGB[cap(a[2])],
		};
	}

	cv::Mat3f sRGB2Lin(cv::Mat3b a) const {

		cv::Mat3f ret(a.rows, a.cols);
		for (int i=0; i<a.rows; i++)
			for (int j=0; j<a.cols; j++) 
				ret(i,j) = sRGB2Lin(a(i,j));

		return ret;
	}

	cv::Mat3b lin2sRGB(cv::Mat3f a) const {

		cv::Mat3b ret(a.rows, a.cols);
		for (int i=0; i<a.rows; i++)
			for (int j=0; j<a.cols; j++) 
				ret(i,j) = lin2sRGB(a(i,j));
				
		return ret;
	}
};

static Colorspace CS;


/*
// Interlaced Video description. 
// We aim at 12.5 frames per second on european MSX, and 12 on american MSXs.
// Each frame can be conducted by switching between 2 name tables.

// VDP can be accessed circa 123500 times x second. 
// There are approx. 9880 accessess available per frame.
// Both name tables must be written per frame, this leaves 8344 access per frame.
// 8344 access are 521 characters per full image, or 173 per third-screen.
// Counting overhead, we consider that a maximum of 128 characters can be changed between frames for each third-screen.

// The main design limitation is that characters that are changed between frames must not be in use!
// A solution for this is to limit the number of sprites in use to 128 per frame, and consider each image as something independent.
// A dynamic solution would consider only the changes between frames. 

// Also, tiles can be represented either by a single MSX1 tile, or two. 
// Two tiles provide better color reproduction, but use twice the amount of resources.


// Limit factors:
//   U_n = Tiles in use in the frame n
//   T_n = Tile changes between frame n-1 and n
//   U_n < 256
//   T_n < 128
//   U_n + T_(n+1) < 256


// We do a N step algorithm.
// 1st step algorithm considers each frame independently, with a limit of U_n<=128.
// 

// 1st step:
// We build a set of tiles, by adding all tiles of the mini-screen (i.e., 256, and
// the centes resulting of clustering those into 64 tiles). 
//
// We represent each prospective image tile is represented by a combination of factors:
// A single MSX tile (normal, and dithered).
// A number of combinations of two MSX tiles (normal, and dithered).
// 
// All generated tiles are collected. So what.





// We accept more 1 representation, and create a tile pool of "prospective tiles". 

// We also cluster the source mini-screen tiles into 64 tentative tiles (i.e., by using 
// spectral clustering.), and represent


// 2nd step:
// We cluster the source mini-screen into 64 tentative miniclusters, 
// and represent each using the best combo of 2 tile MSX, or a single MSX tile.
// we add all those tiles as "prospective tiles".

// Nnd step:
// We search for repeated tiles, and tiles that are currently in use in the following AND previous frame. 
// Those tiles do not count upon the tile limit, and thus free spots. 
// The miniclusters with the most variance are split once to make use of the newly available tiles.
*/

struct Tpalette {
	
	const std::vector<cv::Vec3b> colors = { // Note, those are BGR
		{   0,    0,    0},
		{ 120,  220,   94},
		{  66,  200,   33},
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
		{ 202,  202,  202},
		{ 255,  255,  255}
	};

	std::vector<cv::Vec3f> colorsLin;
	std::vector<std::array<cv::Vec3b,2>> palette;
	std::vector<std::array<cv::Vec3f,2>> paletteLin;
	std::vector<uint8_t> paletteCode;
	std::map<uint8_t,std::array<cv::Vec3b,2>> paletteMap;
	
	std::vector<std::pair<size_t,size_t>> dualPalette;
	
	Tpalette() {
		for (size_t i=0; i<colors.size(); i++) {
			colorsLin.emplace_back(CS.sRGB2Lin(colors[i]));
			for (size_t j=i+1; j<colors.size(); j++) {
				palette   .emplace_back(std::array<cv::Vec3b,2>{colors[i],colors[j]});
				paletteLin.emplace_back(std::array<cv::Vec3f,2>{CS.sRGB2Lin(colors[i]),CS.sRGB2Lin(colors[j])});
				paletteCode.emplace_back(16*(j  + 1) + i  + 1);			
				paletteMap[paletteCode.back()] = palette.back();
			}
		}

		for (size_t p0=0; p0<palette.size(); p0++) {
			for (size_t p1=p0; p1<palette.size(); p1++) {
				dualPalette.emplace_back(p0,p1);
			}
		}
	}
};

static Tpalette palette;

typedef std::array<uint8_t,8>  U8x8;
struct MSX1Tile {
	
	U8x8 pattern;
	U8x8 color;

	cv::Mat3b img() const {
				
		cv::Mat3b ret(8,8);
		for (size_t i=0; i<8; i++) {
			auto &p = palette.paletteMap[color[i]];
					
			for (size_t j=0; j<8; j++) 
				ret(i,7-j) = p[!!(pattern[i] & (1<<j))];
		}
		return ret;
	}
	
	MSX1Tile(const U8x8 &pattern, const U8x8 &color) : pattern(pattern), color(color) {}
	 
	MSX1Tile(uint8_t p=0, uint8_t c=0) {
		
		pattern.fill(p);
		color.fill(c);
	}
	
	bool operator<(const MSX1Tile &rhs) const { 
		if (pattern==rhs.pattern) return color < rhs.color;
		return pattern < rhs.pattern;
	}
	
	bool operator==(const MSX1Tile &rhs) const { return pattern==rhs.pattern and color==rhs.color; }

};


template<typename T>
struct TiledMat : public cv::Mat_<T> {
	
	using cv::Mat_<T>::Mat_;
	
	cv::Mat_<T> tile(int i, int j) {

		return (*this)(cv::Rect(j*8,i*8,8,8));
	}
	
	
	struct Range {
		struct RangeIterator {
			TiledMat tm;
			int row=0, col=0;
			cv::Mat_<T> operator*() { return tm.tile(row,col); };
			RangeIterator &operator++() { 
				col++;
				
				if (col == (tm.cols>>3)) {
					row++;
					col=0;
				}
				return *this;
			};
			bool operator!=(const RangeIterator &rhs) { return row != rhs.row or col != rhs.col; } 
		};
		
		RangeIterator b, e;
		
		RangeIterator begin() { return b;};
		RangeIterator end() { return e;};
	};
	Range tiles() {
		Range ret;
		ret.b.tm = *this;
		ret.e.tm = *this;
		ret.e.row = (this->rows>>3);
		return ret;
	}
};

//typedef cv::Matx<cv::Vec3f,8,8> Tile3f;
typedef TiledMat<cv::Vec3f> TiledMat3f;
typedef TiledMat<cv::Vec3b> TiledMat3b;





static std::pair<MSX1Tile, MSX1Tile> quantizeTile105(cv::Mat3f linTile) {



	MSX1Tile tile0, tile1;
	
	auto compPixel = [](const cv::Vec3f &a, const cv::Vec3f &b) {

		cv::Vec3f c = a-b;
		return c[0]*c[0] + c[1]*c[1] + c[2]*c[2];
	};

	for (int ii=0; ii<8; ii++) { 

		double bestErrorLine = 1e100;

		for (auto &p105 : palette.dualPalette) {

			auto &P0 = palette.paletteLin[ii%2?p105.first:p105.second];
			auto &P1 = palette.paletteLin[ii%2?p105.second:p105.first];

			uint8_t pattern0 = 0, pattern1 = 0;
			
			float errorLine = 0;	
			for (int jj=0; jj<8; jj++) {
				
				cv::Vec3f c00 = (P0[0] + P1[0])*0.5;
				cv::Vec3f c01 = (P0[0] + P1[1])*0.5;
				cv::Vec3f c10 = (P0[1] + P1[0])*0.5;
				cv::Vec3f c11 = (P0[1] + P1[1])*0.5;
				
				cv::Vec3f cSrcLin = linTile(ii,jj);
				float v00 = compPixel(c00,cSrcLin);
				float v01 = compPixel(c01,cSrcLin);
				float v10 = compPixel(c10,cSrcLin);
				float v11 = compPixel(c11,cSrcLin);

				float minError = std::min(std::min(v00,v01),std::min(v10,v11));
				errorLine += minError;
				
				if        (v00 == minError) {
					pattern0 = (pattern0<<1);
					pattern1 = (pattern1<<1);
				} else if (v01 == minError) {
					pattern0 = (pattern0<<1);
					pattern1 = (pattern1<<1) + 1;
				} else if (v10 == minError) {
					pattern0 = (pattern0<<1) + 1;
					pattern1 = (pattern1<<1);
				} else if (v11 == minError) {
					pattern0 = (pattern0<<1) + 1;
					pattern1 = (pattern1<<1) + 1;
				}
			}
				
			if (errorLine<bestErrorLine) {
				bestErrorLine = errorLine;
				tile0.pattern[ii] = pattern0; 
				tile1.pattern[ii] = pattern1;
				tile0.color[ii] = palette.paletteCode[ii%2?p105.first:p105.second];
				tile1.color[ii] = palette.paletteCode[ii%2?p105.second:p105.first];
			}
		}
	}
	
	return {tile0, tile1};
}



struct ClusteredSourceImage {
	

	
	struct Node {

		struct Leaf {
			int img, row, col;
			cv::Mat3f lGBR;
		};
		
		size_t sz = 0;
		std::shared_ptr<Leaf> leaf;
		std::shared_ptr<Node> left, right;
		
		std::pair<MSX1Tile, MSX1Tile> quantizedTiles;
		
		std::vector<float> jpgBlock = TC.getBlock(getsRGB());
		
		cv::Mat3b getsRGB() { return CS.lin2sRGB(getLin()); };
		cv::Mat3f getLin() { return getAggregateLin()*(1./sz); }
		cv::Mat3f getAggregateLin() {
			if (leaf) return leaf->lGBR;
			return left->getAggregateLin() + right->getAggregateLin();			
		}
		
		void runOnLeafs(std::function<void(Leaf &)> f) {
			if (leaf) return f(*leaf);
			left->runOnLeafs(f); right->runOnLeafs(f);
		}
		
		void quantize() { setQuantizedTiles( quantizeTile105(this->getLin()) );	}

		void setQuantizedTiles(const std::pair<MSX1Tile, MSX1Tile> &v) {
			quantizedTiles = v;
			if (not leaf) {
				left->setQuantizedTiles(v); 
				right->setQuantizedTiles(v);
			}
		}
		
		Node(int img, int row, int col, cv::Mat3f lin) : 
			sz(1), leaf(std::make_shared<Leaf>(Leaf{img, row, col, lin})) 
		{}

		Node(std::shared_ptr<Node> a, std::shared_ptr<Node> b) : sz(a->sz+b->sz), left(a), right(b) {}
	};

};


#include <dirent.h>
static std::vector<std::string> getAllFilenames(std::string path, std::string type="") {
	
	std::vector<std::string> r;
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir (path.c_str())) == NULL)
		return r;
	
	while ((ent = readdir (dir)) != NULL)
		if (std::string(ent->d_name).size()>=type.size() and std::string(&ent->d_name[std::string(ent->d_name).size()-type.size()])==type)
			r.push_back(path+"/"+ent->d_name);

	closedir (dir);
	std::sort(r.begin(),r.end());
	return r;
}



static std::vector<std::shared_ptr<ClusteredSourceImage::Node>> 
	mergeNodes(
		std::vector<std::shared_ptr<ClusteredSourceImage::Node>> nodes,
		float minMerge = 1,
		float maxMerge = 1000,
		size_t maxNodes = 64) {

	size_t nNodes = nodes.size();

	std::vector<int> erased(nodes.size()*2,0);
	std::multimap<float, std::pair<size_t,size_t>> distances;
	
//	std::cerr << nodes.size() << std::endl;
	
	#pragma omp parallel for ordered schedule(dynamic,1)
	for (size_t i=0; i<nodes.size(); i++) {
		
		if (erased[i]) continue;
		for (size_t j=i+1; j<nodes.size(); j++) {
			
			if (erased[j]) continue;
			float dist = TC.compareBlock(nodes[i]->jpgBlock, nodes[j]->jpgBlock);
			float distCompound = dist*(100.+(nodes[i]->sz)*(nodes[j]->sz))/100.;
//			float distCompound = dist;

			if (dist<1) {
				#pragma omp critical
				if (not erased[i] and not erased[j]) {
					nodes[i] = std::make_shared<ClusteredSourceImage::Node>(nodes[i],nodes[j]);
					erased[j] = 1;
					nNodes--;
//					std::cerr << ".";
				}
				break;
			}

			if (distCompound<maxMerge) {
				#pragma omp critical
				if (not erased[i] and not erased[j]) {
					distances.emplace(distCompound,	std::make_pair(i,j));
				}
			}
		}
	}


	while (nNodes>1 and not distances.empty()) {
				
		auto f = distances.begin();
		double minDiff = f->first;
		size_t ii=f->second.first, jj=f->second.second;
		
		distances.erase(distances.begin());
		if (erased[ii] or erased[jj]) continue;
				
		if (nNodes<=maxNodes and minDiff>minMerge) break; 
		
		nodes.push_back(std::make_shared<ClusteredSourceImage::Node>(nodes[ii],nodes[jj]));
		erased[ii]=erased[jj]=1;
		nNodes--;

		#pragma omp parallel for
		for (size_t i=0; i<nodes.size()-1; i++) {
			if (erased[i]==0) {
				size_t j = nodes.size()-1;
				float dist = TC.compareBlock(nodes[i]->jpgBlock, nodes[j]->jpgBlock)*(nodes[i]->sz)*(nodes[j]->sz);
				if (dist<maxMerge) {
					#pragma omp critical
					distances.emplace(dist,	std::make_pair(i,j));
				}
			}
		}
	}
	
	std::vector<std::shared_ptr<ClusteredSourceImage::Node>> ret;
	for (size_t i=0; i<nodes.size(); i++) {
		if (erased[i]==0) {
			ret.push_back(nodes[i]);
		}
	}
	return ret;
	
}


std::ofstream header_common;

template<class ...Ts>
static std::string strprintf(std::string fmt, Ts... args) {

	char name[100];
	sprintf(name,fmt.c_str(),args...);
	return name;
}

template<typename T, class ...Ts>
static std::string writeDataBlock(T begin, T end, std::string path, std::string fmt, Ts... args) {
	
	std::string name = strprintf(fmt, args...);
	
	std::ofstream oss(path+name+".c");
	
	oss << "#include <stdint.h>" << std::endl;
	
	oss << "const uint8_t " << name << "[] = {" << std::endl;
	int c=0,sz=0;
	char msg[100];
	while (begin!=end) {
		sprintf(msg,"0x%02X,", *begin++);
		oss << msg;
		c++;
		sz++;
		if (c==16) {
			oss << std::endl;
			c=0;
		}
	}
	oss << "};" << std::endl;
	header_common << "USING_PAGE_D("<<name<<");" << std::endl;
	header_common << "extern const uint8_t " << name << "[" << sz << "];" << std::endl;
	return name;
}

template<typename T>
static std::string writeRawDataBlock(T begin, T end) {
	
	std::ostringstream oss;
	int c=0,sz=0;
	char msg[100];
	while (begin!=end) {
		sprintf(msg,"0x%02X,", *begin++);
		oss << msg;
		c++;
		sz++;
		if (c==16) {
			oss << std::endl;
			c=0;
		}
	}
	return oss.str();
}


int main(int argc, char *argv[]) {
	
	
	if (argc!=2) {
		std::cout << "Use: ./mkGif_v01 name" << std::endl;
		return -1;
	}

	std::string name = std::string(argv[1]);
	
	std::vector<cv::Mat3b> images;
	for (auto f : getAllFilenames("res/gifv01/"+name,".png")) {
		
		//if (images.size()>5) break;
		
		cv::Mat3b img = cv::imread(f);
		
		cv::Mat3f imgf = cv::Mat3f(img)*(1/255.);
		//for (auto &c : imgf) { c[0] = sqrt(c[0]); c[1] = sqrt(c[1]); c[2] = sqrt(c[2]); }
		//imgf = imgf(cv::Rect(imgf.cols/5,0*imgf.rows/5,3*imgf.cols/5,3*imgf.rows/5)).clone();
		
		double scale = std::max(24*8./imgf.rows,32*8./imgf.cols);
		//scale *= 0.6;
		
//		cv::imshow("real_original", imgf.clone());
		
//		cv::resize(img, img, cv::Size(), scale, scale, cv::INTER_AREA);
		cv::resize(imgf, imgf, cv::Size(), scale, scale, cv::INTER_LANCZOS4);

		//imgf = imgf(cv::Rect(0,0,imgf.cols&0xFFF8,imgf.rows&0xFFF8)).clone();
		
		imgf = imgf(cv::Rect(0,0,32*8,24*8)).clone();
		
		cv::Mat3b img3b = cv::Mat3b(imgf*255); 
		
		images.push_back(img3b);
	}

	
	
	std::vector<std::shared_ptr<ClusteredSourceImage::Node>> allNodes;
	std::vector<std::vector<std::vector<std::shared_ptr<ClusteredSourceImage::Node>>>> imageNodes;
	
	for (size_t k=0; k<images.size(); k++) {

		auto img3b = images[k];			
		imageNodes.push_back(std::vector<std::vector<std::shared_ptr<ClusteredSourceImage::Node>>>(3));
		
		for (int l=0; l<img3b.rows; l+=64) {
			
			TiledMat3b bgr = img3b(cv::Rect(0,l,img3b.cols,std::min(64,img3b.rows-l))).clone();
			TiledMat3f lin = CS.sRGB2Lin(bgr);

			int trows = bgr.rows/8;
			int tcols = bgr.cols/8;


			auto &nodes = imageNodes[k][l/64];
			#pragma omp parallel for
			for (int i0=0; i0<trows; i0++) {
				for (int j0=0; j0<tcols; j0++) {
					
					auto newNode = std::make_shared<ClusteredSourceImage::Node>(k,i0+l/8,j0,lin.tile(i0,j0));
					
					#pragma omp critical
					{
						bool mustInsert = true;
						for (auto &n : nodes) {
							double diff = TC.compareBlock(n->jpgBlock, newNode->jpgBlock);
							if (diff < 1e-5)  {
								n = std::make_shared<ClusteredSourceImage::Node>(n,newNode);
								mustInsert = false;
								break;
							}
						}

						if (mustInsert)
						for (auto &n : nodes) {
							double diff = TC.compareBlock(n->jpgBlock, newNode->jpgBlock);
							if (diff < 1e-1)  {
								n = std::make_shared<ClusteredSourceImage::Node>(n,newNode);
								mustInsert = false;
								break;
							}
						}
						
						if (mustInsert)
							nodes.emplace_back(newNode);
					}
				}
			}

			nodes = mergeNodes(nodes,16,512,64);
			
			for (auto &n: nodes)
				allNodes.push_back(n);
			
		}
	}		

	//allNodes = mergeNodes(allNodes,16,512,600000);
	
	#pragma omp parallel for
	for (size_t i = 0; i<allNodes.size(); i++)
		allNodes[i]->quantize();
			
	std::vector<std::vector<std::pair<MSX1Tile, MSX1Tile>>> storages(3);
	std::vector<std::vector<int>> storagePerFrame(images.size(),std::vector<int>(3,0));
	std::vector<cv::Mat1i> tiledImages;
	for (size_t k=0; k<images.size(); k++) {

		auto img3b = images[k];			
		tiledImages.push_back(cv::Mat1i(img3b.rows/8,img3b.cols/8));
		
		for (int l=0; l<img3b.rows; l+=64) {

			auto &storage = storages[l/64];
			auto &nodes = imageNodes[k][l/64];
			
			std::vector<std::pair<MSX1Tile, MSX1Tile>> nodeQuantizations(nodes.size());
			std::vector<size_t> nodeStorageLocation(nodes.size());

			for (size_t i = 0; i<nodes.size(); i++) {
				auto &&node = nodes[i];
				nodeQuantizations[i] = node->quantizedTiles;
				
				{
					size_t location = storage.size();
					for (int l = int(storage.size())-1; l>=0 and l>= int(storage.size())-64; l--) {
						if (nodeQuantizations[i] == storage[l]) {
							location = l;
							break;
						}
					}
					if (location == storage.size())
						storage.push_back(nodeQuantizations[i]);
					nodeStorageLocation[i] = location;
				}
			}
			
			{
				std::deque<std::pair<int,int>> nodeStorageLocationDeque;
				for (size_t i = 0; i<nodes.size(); i++)
					nodeStorageLocationDeque.push_back(std::pair<int,int>(nodeStorageLocation[i],i));
					
				std::sort(nodeStorageLocationDeque.begin(), nodeStorageLocationDeque.end());
				while (nodeStorageLocationDeque.front().first+64<int(storage.size())) {

					int i = nodeStorageLocationDeque.front().second;
					nodeStorageLocationDeque.pop_front();
					nodeStorageLocationDeque.push_back(std::pair<int,int>(storage.size(),i));
					nodeStorageLocation[i] = storage.size();
					storage.push_back(nodeQuantizations[i]);
				}
			}
			
			#pragma omp parallel for
			for (size_t i = 0; i<nodes.size(); i++) {
				auto &&node = nodes[i];
				//auto &pt = nodeQuantizations[i];
				auto &pt = storage[nodeStorageLocation[i]];
			
				cv::Mat3b sRGB = CS.lin2sRGB((CS.sRGB2Lin(pt.first.img())+CS.sRGB2Lin(pt.second.img()))*0.5);
				
				node->runOnLeafs([&](ClusteredSourceImage::Node::Leaf &l){
					sRGB.copyTo(images[l.img](cv::Rect(8*l.col,8*l.row,8,8)));

					tiledImages[l.img](l.row,l.col) = nodeStorageLocation[i];
				});
			}
		}
		
		storagePerFrame[k][0] = storages[0].size();
		storagePerFrame[k][1] = storages[1].size();
		storagePerFrame[k][2] = storages[2].size();

		std::cerr << storages[0].size() << "\t" << storages[1].size() << "\t" << storages[2].size() << "\t" << std::endl;
		
//		cv::imshow("q_full", images[k]);
//		cv::waitKey(10);
	}



	std::ofstream source_main("tmp/res/gifv01/"+name+".c");
	std::ofstream header_main("tmp/inc/res/gifv01/"+name+".h");
		
	header_common.open("tmp/res/gifv01/"+name+"_gen.h");
	header_common << "#pragma once" << std::endl;
	header_common << "#include <gif_v01.h>" << std::endl;
		
		
	source_main << "#include \"" << name << "_gen.h\"" << std::endl;
	//source_main << "#include \"" << name << ".h\"" << std::endl;
	source_main << "void " << name << "_play(void);" << std::endl;
	source_main << "void " << name << "_play(void) {" << std::endl;
	source_main << "    gif_v01_initVideo();" << std::endl;
		
	header_main << "#pragma once" << std::endl;
	header_main << "#include <gif_v01.h>" << std::endl;
	header_main << "void " << name << "_play();" << std::endl;	
	
	for (int k=0; k<int(images.size()); k++) {
		
		auto img3b = images[k];			

		char frameNr[100]; sprintf(frameNr,"%05d",k);
		std::ofstream file_frame_main("tmp/res/gifv01/"+name+frameNr+".c");

		header_common << "void " << name << frameNr << "();" << std::endl;

		header_common << "USING_PAGE_C("<<name << frameNr<<");" << std::endl;
		source_main << "    " << "fast_load_page_c(SEGMENT_TO_PAGE_C("<<name<<frameNr<<")); " << name << frameNr << "();" << std::endl;
		
		file_frame_main << "#include \"" << name << "_gen.h\"" << std::endl;
		

		file_frame_main << "void " << name << frameNr << "() {" << std::endl;
		file_frame_main << "    gif_v01_initSendFrame();" << std::endl;

		for (int l=0; l<img3b.rows; l+=64) {

			std::vector<uint8_t> P0, P1, C0, C1;
			
			auto &storage = storages[l/64];
			for (int i=(k?storagePerFrame[k-1][l/64]:0); i<storagePerFrame[k][l/64]; i++) {
				for (auto &v: storage[i].first.pattern ) P0.push_back(v);
				for (auto &v: storage[i].second.pattern) P1.push_back(v);
				for (auto &v: storage[i].first.color   ) C0.push_back(v);
				for (auto &v: storage[i].second.color  ) C1.push_back(v);
			}
			
			std::string SP0, SP1, SC0, SC1;
			if (P0.size()) SP0 = writeDataBlock(P0.begin(), P0.end(), "tmp/res/gifv01/", name+"%05d"+"_%05d"+"_P0",k,l/64);
			if (P1.size()) SP1 = writeDataBlock(P1.begin(), P1.end(), "tmp/res/gifv01/", name+"%05d"+"_%05d"+"_P1",k,l/64);
			if (C0.size()) SC0 = writeDataBlock(C0.begin(), C0.end(), "tmp/res/gifv01/", name+"%05d"+"_%05d"+"_C0",k,l/64);
			if (C1.size()) SC1 = writeDataBlock(C1.begin(), C1.end(), "tmp/res/gifv01/", name+"%05d"+"_%05d"+"_C1",k,l/64);
			
//				if (P0.size()) file_frame_main << "    fast_load_page_d(SEGMENT_D(" << SP0 << ")); gif_v01_addP0_section" << (l/64) << "(" << SP0 << ", " << P0.size()/8 << ");" << std::endl;
//				if (P1.size()) file_frame_main << "    fast_load_page_d(SEGMENT_D(" << SP1 << ")); gif_v01_addP1_section" << (l/64) << "(" << SP1 << ", " << P1.size()/8 << ");" << std::endl;
//				if (C0.size()) file_frame_main << "    fast_load_page_d(SEGMENT_D(" << SC0 << ")); gif_v01_addC0_section" << (l/64) << "(" << SC0 << ", " << C0.size()/8 << ");" << std::endl;
//				if (C1.size()) file_frame_main << "    fast_load_page_d(SEGMENT_D(" << SC1 << ")); gif_v01_addC1_section" << (l/64) << "(" << SC1 << ", " << C1.size()/8 << ");" << std::endl;

			int s0 = (k?storagePerFrame[k-1][l/64]:0)%128; // current pattern index
			int e0 = std::min(int(s0 + (P0.size()/8)),128)-s0;
			int e1 = std::max(int(s0 + (P0.size()/8) - 128),0);
			
			if (P0.size()) { file_frame_main << "    fast_load_page_d(SEGMENT_TO_PAGE_D("<<SP0<<")); gif_v01_copy_vpn_x8( MODE2_ADDRESS_PG+(128*8*(2*"<<(l/64)<< "+0))+8*" << s0 << " , " << SP0 << ", " << e0 << "); ";  if (e1) file_frame_main << " gif_v01_copy_vpn_x8( MODE2_ADDRESS_PG+(128*8*(2*"<<(l/64)<< "+0)), " << SP0 << "+8*" << e0 << ", " << e1 << ");"; file_frame_main << std::endl; }
			if (P1.size()) { file_frame_main << "    fast_load_page_d(SEGMENT_TO_PAGE_D("<<SP1<<")); gif_v01_copy_vpn_x8( MODE2_ADDRESS_PG+(128*8*(2*"<<(l/64)<< "+1))+8*" << s0 << " , " << SP1 << ", " << e0 << "); ";  if (e1) file_frame_main << " gif_v01_copy_vpn_x8( MODE2_ADDRESS_PG+(128*8*(2*"<<(l/64)<< "+1)), " << SP1 << "+8*" << e0 << ", " << e1 << ");"; file_frame_main << std::endl; }
			if (C0.size()) { file_frame_main << "    fast_load_page_d(SEGMENT_TO_PAGE_D("<<SC0<<")); gif_v01_copy_vpn_x8( MODE2_ADDRESS_CT+(128*8*(2*"<<(l/64)<< "+0))+8*" << s0 << " , " << SC0 << ", " << e0 << "); ";  if (e1) file_frame_main << " gif_v01_copy_vpn_x8( MODE2_ADDRESS_CT+(128*8*(2*"<<(l/64)<< "+0)), " << SC0 << "+8*" << e0 << ", " << e1 << ");"; file_frame_main << std::endl; }
			if (C1.size()) { file_frame_main << "    fast_load_page_d(SEGMENT_TO_PAGE_D("<<SC1<<")); gif_v01_copy_vpn_x8( MODE2_ADDRESS_CT+(128*8*(2*"<<(l/64)<< "+1))+8*" << s0 << " , " << SC1 << ", " << e0 << "); ";  if (e1) file_frame_main << " gif_v01_copy_vpn_x8( MODE2_ADDRESS_CT+(128*8*(2*"<<(l/64)<< "+1)), " << SC1 << "+8*" << e0 << ", " << e1 << ");"; file_frame_main << std::endl; }

				 
		}
			
		file_frame_main << "    gif_v01_pre_sendPN();" << std::endl;
		
		std::vector<int> skipv(3);
		std::vector<std::string> SPNv(3);
		std::vector<std::deque<uint8_t>> PNv(3),PNoldv(3);
		for (int l=0; l<img3b.rows; l+=64) {
			std::deque<uint8_t> &PN = PNv[l/64];
			std::deque<uint8_t> &PNold = PNoldv[l/64];
			std::string &SPN = SPNv[l/64];
			int &skip = skipv[l/64];
			
			auto &tp = tiledImages[k-1];
			auto &ti = tiledImages[k];
			for (int i0=8*(l/64); i0<ti.rows and i0<8*(1+(l/64)); i0++) {
				for (int j0=0; j0<ti.cols; j0++) {
					PNold.push_back((k?tp(i0,j0):0)%128);
					PN.push_back(ti(i0,j0)%128);
				}
			}
			skip=0;
			if (k) while (not PN.empty() and not PNold.empty() and PN.front()==PNold.front()) { skip++; PN.pop_front(); PNold.pop_front(); }
			if (k) while (not PN.empty() and not PNold.empty() and PN.back()==PNold.back()) { PN.pop_back(); PNold.pop_back(); }
			
			if (PN.size()) SPN = writeDataBlock(PN.begin(), PN.end(), "tmp/res/gifv01/", name+"%05d"+"_%05d"+"_PN",k,l/64);
			if (PN.size() == 256 ) file_frame_main << "    fast_load_page_d(SEGMENT_TO_PAGE_D(" << SPN << ")); gif_v01_addPN_section" << (l/64) << "_256(" << SPN << ");" << std::endl;
			else if (PN.size()) file_frame_main << "    fast_load_page_d(SEGMENT_TO_PAGE_D(" << SPN << ")); gif_v01_addPN_section" << (l/64) << "(" << SPN << ", " <<skip << ", " << PN.size() << ");" << std::endl;
		}

		file_frame_main << "    gif_v01_mid_sendPN();" << std::endl;

		for (int l=0; l<img3b.rows; l+=64) {
			std::deque<uint8_t> &PN = PNv[l/64];
			std::string &SPN = SPNv[l/64];
			int &skip = skipv[l/64];
			
			if (PN.size() == 256 ) file_frame_main << "    fast_load_page_d(SEGMENT_TO_PAGE_D(" << SPN << ")); gif_v01_addPN_section" << (l/64) << "_256(" << SPN << ");" << std::endl;
			else if (PN.size()) file_frame_main << "    fast_load_page_d(SEGMENT_TO_PAGE_D(" << SPN << ")); gif_v01_addPN_section" << (l/64) << "(" << SPN << ", " <<skip << ", " << PN.size() << ");" << std::endl;
		}

		file_frame_main << "    gif_v01_post_sendPN();" << std::endl;

		file_frame_main << "    gif_v01_endSendFrame();" << std::endl;
		file_frame_main << "}" << std::endl;
		

	}
	

	
	source_main << "    gif_v01_deinitVideo();" << std::endl;
	source_main << "}" << std::endl;		

	return 0;
}
