////////////////////////////////////////////////////////////////////////
// Image to MSX tiles
//
// Manuel Martinez (salutte@gmail.com)
//
// FLAGS: -std=gnu++14 -g `pkg-config opencv --cflags --libs` -O3 -lpthread -fopenmp -lgomp

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>
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


struct ClusteredSourceImage {
	

	
	struct Node {

		struct Leaf {
			int i,j;
			cv::Mat3f lGBR;
		};
		
		size_t sz = 0;
		std::shared_ptr<Leaf> leaf;
		std::shared_ptr<Node> left, right;
		
		std::vector<float> jpgBlock = TC.getBlock(getsRGB());
		
		cv::Mat3b getsRGB() { return CS.lin2sRGB(getLin()); };
		cv::Mat3f getLin() { return getAggregateLin()*(1./sz); }
		cv::Mat3f getAggregateLin() {
			if (leaf) return leaf->lGBR;
			return left->getAggregateLin() + right->getAggregateLin();			
		}
		
		Node(int rows, int cols, cv::Mat3f lin) : 
			sz(1), leaf(std::make_shared<Leaf>(Leaf{rows,cols,lin})) 
		{}

		Node(std::shared_ptr<Node> a, std::shared_ptr<Node> b) : sz(a->sz+b->sz), left(a), right(b) {}
	};

};




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



static void quantizeSpectral( const cv::Mat3b src, TiledMat3b target) {

	if (src.rows % 8) throw std::runtime_error("Rows not multiple of 8");
	if (src.cols % 8) throw std::runtime_error("Rows not multiple of 8");
	int trows = src.rows/8;
	int tcols = src.cols/8;

	TiledMat3b bgr = src.clone();
	TiledMat3f lin = CS.sRGB2Lin(src);


	std::vector<std::shared_ptr<ClusteredSourceImage::Node>> nodes;
	for (int i0=0; i0<trows; i0++) {
		for (int j0=0; j0<tcols; j0++) {
			
			auto newNode = std::make_shared<ClusteredSourceImage::Node>(i0,j0,lin.tile(i0,j0));
			
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
				nodes.emplace_back(std::make_shared<ClusteredSourceImage::Node>(i0,j0,lin.tile(i0,j0)));
		}
	}
	
	while (nodes.size()>1) {
		
		double minDiff = 1e10;
		size_t ii=0, jj=0;
		for (size_t i=0; i<nodes.size(); i++) {
			for (size_t j=i+1; j<nodes.size(); j++) {
				double diff = TC.compareBlock(nodes[i]->jpgBlock, nodes[j]->jpgBlock)*(nodes[i]->sz)*(nodes[j]->sz);
				if (diff < minDiff) {
					minDiff = diff;
					ii=i;
					jj=j;
				}
			}
		}
		if (nodes.size()<64 and minDiff>5) break; 

		nodes[ii] = std::make_shared<ClusteredSourceImage::Node>(nodes[ii],nodes[jj]);
		std::swap(nodes[jj], nodes[nodes.size()-1]);
		nodes.pop_back();
		//std::cout << minDiff << " " << nodes.size() << std::endl;
	}

	static std::map<MSX1Tile,int> allTiles;
	std::map<MSX1Tile,int> newTiles, usedTiles;
	#pragma omp parallel for
	for (int i=0; i<trows; i++) {
		for (int j=0; j<tcols; j++) {
			
			double minDiff = 1e10;
			for (auto &n : nodes) {
				double diff = TC.compare(n->getsRGB(), bgr.tile(i,j));
				if (diff < minDiff) {
					minDiff = diff;
					cv::Mat3f t = n->getLin();
					std::pair<MSX1Tile, MSX1Tile> p = quantizeTile105(t);
					
					CS.lin2sRGB((CS.sRGB2Lin(p.first.img())+CS.sRGB2Lin(p.second.img()))*0.5).copyTo(target.tile(i,j));
					
					
					#pragma omp critical
					{
						if (allTiles[p.first]==0) newTiles[p.first]++;
						if (allTiles[p.second]==0) newTiles[p.second]++;

						usedTiles[p.first]++;
						usedTiles[p.second]++;
						
						allTiles[p.first]++;
						allTiles[p.second]++;
					}
					//n->getsRGB().copyTo(target.tile(i,j));
				}
			}
		}
	}
	
	std::cout << "Used: " << double(newTiles.size())/double(usedTiles.size()) << " " << usedTiles.size() << " " << allTiles.size() << std::endl;

}


int main(int argc, char *argv[]) {
	
	
	if (argc!=2) {
		std::cout << "Use: ./mkGif <file.gif>" << std::endl;
		return -1;
	}

	cv::VideoCapture cap(argv[1]);
	
	std::map<int,cv::Mat3i> imageQuantErrorCarry;
	
	bool first = true;
	cv::Mat3b img;
	while (cap.read(img)) {
		
		
		
		cv::Mat3f imgf = cv::Mat3f(img)*(1/255.);

		//imgf = imgf(cv::Rect(imgf.cols/5,0*imgf.rows/5,3*imgf.cols/5,3*imgf.rows/5)).clone();

		
		double scale = std::min(2*96./imgf.rows,256./imgf.cols);
		//scale *= 0.6;
		
		
		cv::imshow("real_original", imgf.clone());
		

		
//		cv::resize(img, img, cv::Size(), scale, scale, cv::INTER_AREA);
		cv::resize(imgf, imgf, cv::Size(), scale, scale, cv::INTER_LANCZOS4);

			
		imgf = imgf(cv::Rect(0,0,imgf.cols&0xFFF8,imgf.rows&0xFFF8)).clone();		

		
		if(1) {
			
			cv::Mat3b img3b = cv::Mat3b(imgf*255); 
			cv::Mat3b work3b = img3b.clone()*0; 
			
			for (int l=0; l<imgf.rows; l+=64)  
				quantizeSpectral(
					img3b(cv::Rect(0,l,img3b.cols,std::min(64,img3b.rows-l))).clone(), 
					work3b(cv::Rect(0,l,img3b.cols,std::min(64,img3b.rows-l))));

			cv::resize(work3b, work3b, cv::Size(), 4, 4, cv::INTER_NEAREST);
			cv::imshow("q_full", work3b);		
		}
		if (first) {
			cv::waitKey(0);
			first = false;
		}
		cv::waitKey(30);
	}
	cv::waitKey(0);
	return 0;
}
