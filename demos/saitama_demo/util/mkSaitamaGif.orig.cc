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

// Pure grayscale palettes:
// 


struct TsRGB {
	
	std::array<int, 256> TsRGB2lin;
	std::array<uint8_t,4096*1> Tlin2sRGB;

	TsRGB() {
	
		for (size_t i=0; i<256; i++) {
			
			double srgb = (i+0.5)/255.999999999;
			double lin = 0;
			if (srgb <= 0.0405) {
				lin = srgb/12.92;
			} else {
				lin = std::pow((srgb+0.055)/(1.055),2.4);
			}
			TsRGB2lin[i]=lin*4095.999999999;
		}
		
		for (size_t i=0; i<4096*1; i++) {

			double lin = (i+0.5)/(4096*1-1e-6);
			double srgb = 0;
			if (lin <= 0.0031308) {
				srgb = lin*12.92;
			} else {
				srgb = 1.055*std::pow(lin,1/2.4)-0.055;
			}
			Tlin2sRGB[i]=srgb*255.999999999;
		}
	}

	cv::Vec3i toLin(cv::Vec3b a) const {
		
		return {
			TsRGB2lin[a[0]],
			TsRGB2lin[a[1]],
			TsRGB2lin[a[2]]
		};
	}

	cv::Vec3b tosRGB(cv::Vec3i a) const {
	
		auto cap = [](int i){ return std::min(std::max(i,0),4095); };
		return {
			Tlin2sRGB[cap(a[0])],
			Tlin2sRGB[cap(a[1])],
			Tlin2sRGB[cap(a[2])],
		};
	}

	cv::Vec3b add(cv::Vec3b a, cv::Vec3b b) const {
		return tosRGB((toLin(a) + toLin(b))/2);
	}

	cv::Mat3b add(cv::Mat3b A, cv::Mat3b B) const {
	
		for (int i=0; i<A.rows; i++)
			for (int j=0; j<A.cols; j++) 
				A(i,j) = add(A(i,j), B(i,j));

		return A;
	}
};

static TsRGB sRGB;

struct Tpalette {
	
	const std::vector<cv::Vec3b> colors = { // Note, those are BGR
		{   0,    0,    0},
/*		{  66,  200,   33},
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
		{ 202,  202,  202},
		{ 255,  255,  255}	*/
		{ 186,   91,  201},
		{ 255,  255,  255}
	};

	std::vector<cv::Vec3i> colorsLin;
	std::vector<std::array<cv::Vec3b,2>> palette;
	std::vector<std::array<cv::Vec3i,2>> paletteLin;
	std::vector<uint8_t> paletteCode;
	std::map<uint8_t,std::array<cv::Vec3b,2>> paletteMap;
	
	std::vector<std::pair<size_t,size_t>> dualPalette;
	
	Tpalette() {
		for (size_t i=0; i<colors.size(); i++) {
			colorsLin.emplace_back(sRGB.toLin(colors[i]));
			for (size_t j=i+1; j<colors.size(); j++) {
				palette   .emplace_back(std::array<cv::Vec3b,2>{colors[i],colors[j]});
				paletteLin.emplace_back(std::array<cv::Vec3i,2>{sRGB.toLin(colors[i]),sRGB.toLin(colors[j])});
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
struct Tile {
	
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

auto bgr2yuv = [](const cv::Vec3b p) {
	
	uint8_t y = (29*p[0]+150*p[1]+77*p[2])>>8;
	return cv::Vec3b(y, (128*256+125*(p[0]-y))>>8, (128*256+224*(p[2]-y))>>8);
};

auto compyuv = [](cv::Vec3b p0, cv::Vec3b p2) {
	
	p0 = bgr2yuv(p0);
	p2 = bgr2yuv(p2);
	
	double color2gray_ratio = 1;

	
	if (1) return	(p0[0]-p2[0])*(p0[0]-p2[0]) +
			color2gray_ratio*(p2[0]/256.)*(p0[1]-p2[1])*(p0[1]-p2[1]) +
			color2gray_ratio*(p2[0]/256.)*(p0[2]-p2[2])*(p0[2]-p2[2]);

	return	std::abs(p0[0]-p2[0]) +
			color2gray_ratio*(p2[0]/256.)*std::abs(p0[1]-p2[1]) +
			color2gray_ratio*(p2[0]/256.)*std::abs(p0[2]-p2[2]);


	

};

auto comp = compyuv;

static std::pair<cv::Mat2i, std::map<size_t, Tile>> quantizeFast( const cv::Mat3b src, cv::Mat3b target) {

	std::map<size_t, Tile> tiles;
	cv::Mat2i tileMap(src.rows/8,src.cols/8);
	
	cv::Mat3i srcLin(src.rows,src.cols);
	for (int i=0; i<src.rows; i++) 
		for (int j=0; j<src.cols; j++)
			srcLin(i,j) = sRGB.toLin(src(i,j));

	{ // Find best pair of tiles for each 8x8 block
		
		std::map<Tile, size_t> differentTiles;
		for (size_t i=0; i<palette.colors.size()-1; i++)
			differentTiles[tiles[i] = Tile(0,240+i+1)] = i;
		differentTiles[tiles[palette.colors.size()-1] = Tile(255,254)] = palette.colors.size()-1;
		
		std::vector<cv::Vec3i> basicTilesColorLin;
		for (size_t t=0; t<palette.colors.size(); t++) 
			basicTilesColorLin.push_back(sRGB.toLin(tiles[t].img()(0,0)));
		
		for (int i=0; i<src.rows; i+=8) { 
			std::cerr << i << " out of " << src.rows << std::endl;
			for (int j=0; j<src.cols; j+=8) {

//				Floyd–Steinberg dithering

				int L0_bestBasicTile = -1;
				Tile   L0_bestCustomTile;
				double L0_bestError = 1e100;
				cv::Mat3i L0_quantError(8,8,cv::Vec3i(0,0,0));

				for (int t=-1; t<int(basicTilesColorLin.size()); t++) {
					
					Tile   L1_bestCustomTile;
					double L1_bestError = 0;
					cv::Mat3i L1_quantError(8,8,cv::Vec3i(0,0,0));					
					
					cv::Mat3i quantErrorLastRow(8,1,cv::Vec3i(0,0,0));					
					for (int ii=0; ii<8; ii++) { // Selects row;

						uint8_t L2_color;
						uint8_t L2_pattern;
						double  L2_bestError = 1e100;
						cv::Mat3i L2_quantErrorRow(8,1,cv::Vec3i(0,0,0));
						
						for (size_t p = 0; p < palette.paletteLin.size(); p++) {

							auto &palLin = palette.paletteLin[p];
							double  L3_error = 0;
							uint8_t L3_pattern = 0;
							cv::Mat3i L3_quantErrorRow(8,1,cv::Vec3i(0,0,0));

							cv::Vec3i quantErrorCarry(0,0,0);
							for (int jj=0; jj<8; jj++) {
								
								cv::Vec3i c0 = palette.paletteLin[p][0];
								cv::Vec3i c1 = palette.paletteLin[p][1]; 
								if (t>=0) {
									c0 = (c0 + basicTilesColorLin[t])/2;
									c1 = (c1 + basicTilesColorLin[t])/2;
								}
								
								cv::Vec3i cSrcLin  = srcLin(i+ii,j+jj);
								          cSrcLin += quantErrorCarry        *7/16;
								if (jj>0) cSrcLin += quantErrorLastRow(jj-1)*1/16;
										  cSrcLin += quantErrorLastRow(jj  )*5/16;
								if (jj<7) cSrcLin += quantErrorLastRow(jj+1)*3/16;

								cv::Vec3b cSrc = sRGB.tosRGB(cSrcLin);								
								double v0 = comp(sRGB.tosRGB(c0), cSrc);
								double v1 = comp(sRGB.tosRGB(c1), cSrc);
								
								double minError = std::min(v0,v1);
								L3_error += minError;
								
								if (v0 < v1) {
									L3_pattern = (L3_pattern<<1);
									L3_quantErrorRow(jj) = cSrcLin-c0;
								} else {
									L3_pattern = (L3_pattern<<1) + 1;
									L3_quantErrorRow(jj) = cSrcLin-c1;
								}
								quantErrorCarry = L3_quantErrorRow(jj);								
							}
							
							if (L3_error<L2_bestError) {
								L2_bestError = L3_error;
								L2_pattern = L3_pattern;
								L2_color = palette.paletteCode[p];
								L2_quantErrorRow = L3_quantErrorRow.clone();
							}
						}

						
						quantErrorLastRow = L2_quantErrorRow.clone();
						L1_bestError += L2_bestError;
						L1_quantError(cv::Rect(0,ii,8,1)) = L2_quantErrorRow;
						//L2_quantErrorRow.copyTo(L1_quantError.row(ii));
						
						L1_bestCustomTile.pattern[ii] = L2_pattern;
						L1_bestCustomTile.color[ii]   = L2_color;

					}
				
//					std::cout << "k " << L1_bestError << " " << L0_bestError << " " << t << std::endl;
					if (L1_bestError < L0_bestError) {

						L0_bestBasicTile = t;
						L0_bestCustomTile = L1_bestCustomTile;
						L0_bestError = L1_bestError;
						L0_quantError = L1_quantError.clone();
					}
				}
				

				cv::Vec2i &selectedTiles = tileMap(i/8,j/8);
				if (not differentTiles.count(L0_bestCustomTile))
					differentTiles[L0_bestCustomTile] = tiles.size();	
				selectedTiles[0] = differentTiles[L0_bestCustomTile];
				tiles[selectedTiles[0]] = L0_bestCustomTile;
				
				if (L0_bestBasicTile != -1) {
					selectedTiles[1] = L0_bestBasicTile;
				} else {
					selectedTiles[1] = selectedTiles[0];
				}
			}
		}
	}
	
	for (int i=0; i<src.rows; i+=8)
		for (int j=0; j<src.cols; j+=8)
			sRGB.add(
				tiles[tileMap(i/8,j/8)[0]].img(),
				tiles[tileMap(i/8,j/8)[1]].img())
				.copyTo(target(cv::Rect(j,i,8,8)));

	return std::make_pair(tileMap, tiles);

}


static std::pair<cv::Mat2i, std::map<size_t, Tile>> quantize105dither( const cv::Mat3b src, cv::Mat3b target, cv::Mat3i &imageQuantError, double ditherFactor=1.0) {

	std::map<size_t, Tile> tiles;
	cv::Mat2i tileMap(src.rows/8,src.cols/8);
	
	cv::Mat3i srcLin = cv::Mat3i(src);
	for (auto &p : srcLin)
		p = sRGB.toLin(p);
		
	if (imageQuantError.size() == srcLin.size()) 
		srcLin += imageQuantError * ditherFactor;

	{ // Find best pair of tiles for each 8x8 block
		
		std::map<Tile, size_t> differentTiles;
//		for (size_t i=0; i<palette.colors.size()-1; i++)
//			differentTiles[tiles[i] = Tile(0,240+i+1)] = i;
//		differentTiles[tiles[palette.colors.size()-1] = Tile(255,254)] = palette.colors.size()-1;
		
		#pragma omp parallel for
		for (int i=0; i<src.rows; i+=8) { 
			
			#pragma omp critical
			std::cerr << i << " out of " << src.rows << std::endl;
			for (int j=0; j<src.cols; j+=8) {

				cv::Vec2i &selectedTiles = tileMap(i/8,j/8);

				Tile tile0, tile1;
				double errorTile = 0;
				for (int ii=0; ii<8; ii++) { // Selects row;

//					Floyd–Steinberg dithering

					double bestErrorLine = 1e100;
					std::vector<cv::Vec3i> bestQuantError;
					std::vector<cv::Vec3i> quantError(8);
					for (auto &p105 : palette.dualPalette) {

						auto &P0 = palette.paletteLin[p105.first];
						auto &P1 = palette.paletteLin[p105.second];

						double errorLine = 0;
						uint8_t pattern0 = 0, pattern1 = 0;
						for (int jj=0; jj<8; jj++) {
							
							double v00, v01, v10, v11;
														
							cv::Vec3i c00 = (P0[0] + P1[0])/2;
							cv::Vec3i c01 = (P0[0] + P1[1])/2;
							cv::Vec3i c10 = (P0[1] + P1[0])/2;
							cv::Vec3i c11 = (P0[1] + P1[1])/2;
							
							cv::Vec3i cSrcLin  = srcLin(i+ii,j+jj);
							if (jj) cSrcLin += quantError[jj-1]*7/16;


							//cv::Vec3i cTargetLin = srcLin(i+ii,j+jj);
							cv::Vec3b cTarget = sRGB.tosRGB(cSrcLin);
								
							v00 = 0.996*comp(sRGB.tosRGB(c00), cTarget);
							v01 = 0.997*comp(sRGB.tosRGB(c01), cTarget);
							v10 = 0.998*comp(sRGB.tosRGB(c10), cTarget);
							v11 = 0.999*comp(sRGB.tosRGB(c11), cTarget);
							
							double minError = std::min(std::min(v00,v01),std::min(v10,v11));
							errorLine += minError;
							
							if        (v00 == minError) {
								pattern0 = (pattern0<<1);
								pattern1 = (pattern1<<1);
								quantError[jj] = cSrcLin-c00;
							} else if (v01 == minError) {
								pattern0 = (pattern0<<1);
								pattern1 = (pattern1<<1) + 1;
								quantError[jj] = cSrcLin-c01;
							} else if (v10 == minError) {
								pattern0 = (pattern0<<1) + 1;
								pattern1 = (pattern1<<1);
								quantError[jj] = cSrcLin-c10;
							} else if (v11 == minError) {
								pattern0 = (pattern0<<1) + 1;
								pattern1 = (pattern1<<1) + 1;
								quantError[jj] = cSrcLin-c11;
							}
							quantError[jj] *= ditherFactor;
						}
						
						if (errorLine<bestErrorLine) {
							bestErrorLine = errorLine;
							bestQuantError = quantError;
							tile0.pattern[ii] = pattern0; 
							tile1.pattern[ii] = pattern1;
							tile0.color[ii] = palette.paletteCode[p105.first];
							tile1.color[ii] = palette.paletteCode[p105.second];
						}
					}
					
					if (i+ii+1<src.rows) for (int jj=0; jj<8; jj++) {

						if (j+jj-1>=0)       srcLin(i+ii+1,j+jj-1) += bestQuantError[jj] * 3 / 16;
											 srcLin(i+ii+1,j+jj  ) += bestQuantError[jj] * 5 / 16;
						if (j+jj+1<src.cols) srcLin(i+ii+1,j+jj+1) += bestQuantError[jj] * 1 / 16;
					}
					if (j+7+1<src.cols)      srcLin(i+ii  ,j+ 7+1) += bestQuantError[ 7] * 7 / 16;
					
					errorTile += bestErrorLine;
				}
				
				#pragma omp critical
				{
					if (not differentTiles.count(tile0))
						differentTiles[tile0] = tiles.size();	
					selectedTiles[0] = differentTiles[tile0];
					tiles[selectedTiles[0]] = tile0;
					
					if (not differentTiles.count(tile1))
						differentTiles[tile1] = tiles.size();	
					selectedTiles[1] = differentTiles[tile1];
					tiles[selectedTiles[1]] = tile1;
				}
			}
		}
	}
	
	{	// Summarize in order to have maximum 256 tiles
		std::map<Tile, size_t> tt;
		tt.clear();
		for (auto &t : tiles) 
			tt[t.second]++;

		std::cout << "Size: " << tt.size() << " " << tiles.size() << std::endl;
		
		while (tt.size()>256) {

			std::map<double, std::pair<int, int>> tileComp;
			for (auto &t0 : tiles) 
				for (auto &t1 : tiles) 
					if (t0.first<t1.first)
						tileComp[cv::norm(cv::Mat3d(t1.second.img())-cv::Mat3d(t0.second.img()))] = std::make_pair(t0.first,t1.first);
							
			tiles.erase(tileComp.begin()->second.second);


			{
				std::vector<std::pair<int,int>> affectedBlocks;
				std::map<size_t, std::reference_wrapper<Tile>> affectedTiles;

				for (int i=0; i<tileMap.rows; i++) {
					for (int j=0; j<tileMap.cols; j++) {
						if (tileMap(i,j)[0]==tileComp.begin()->second.second or 
							tileMap(i,j)[1]==tileComp.begin()->second.second) {
							
							if (tileMap(i,j)[0]==tileComp.begin()->second.second)
								tileMap(i,j)[0]=tileComp.begin()->second.first;

							if (tileMap(i,j)[1]==tileComp.begin()->second.second)
								tileMap(i,j)[1]=tileComp.begin()->second.first;
							
							if (tileMap(i,j)[0]>15) {
								affectedTiles.emplace(tileMap(i,j)[0], tiles[tileMap(i,j)[0]]);
								affectedBlocks.emplace_back(i,j);
							}
							if (tileMap(i,j)[1]>15) {
								affectedTiles.emplace(tileMap(i,j)[1], tiles[tileMap(i,j)[1]]);
								affectedBlocks.emplace_back(i,j);
							}
						}
					}
				}
				
				std::cerr << affectedBlocks.size() << " " << affectedTiles.size() << std::endl;	
				
				
				for (auto &&at : affectedTiles) {
					
					int tileIdx = at.first;
					Tile &tile = at.second.get();
					
					double bestErrorLine = 1e100;
					
					for (int ii=0; ii<8; ii++) { // Select tile row
							
						for (size_t p0=0; p0<palette.palette.size(); p0++) {

							double errorLine = 0;
							uint8_t pattern = 0;
							
							for (int jj=0; jj<8; jj++) { // Select tile col
								
								double v0 = 0., v1 = 0.;

								for (auto &&block : affectedBlocks) {
									
									auto colora0 = tiles[tileMap(block.first,block.second)[0]].img()(ii,jj);
									auto colorb0 = tiles[tileMap(block.first,block.second)[1]].img()(ii,jj);
									auto colora1 = colora0;
									auto colorb1 = colorb0;
									
									if (tileMap(block.first,block.second)[0]==tileIdx) {
										colora0 = palette.palette[p0][0];
										colora1 = palette.palette[p0][1];
									}
									if (tileMap(block.first,block.second)[1]==tileIdx) {
										colorb0 = palette.palette[p0][0];
										colorb1 = palette.palette[p0][1];
									}
										
									v0 += comp(sRGB.tosRGB((sRGB.toLin(colora0) + sRGB.toLin(colorb0))/2), src(block.first*8+ii,block.second*8+jj));
									v1 += comp(sRGB.tosRGB((sRGB.toLin(colora1) + sRGB.toLin(colorb1))/2), src(block.first*8+ii,block.second*8+jj));
								}
									
								double minError = std::min(v0,v1);
								errorLine += minError;
									
								if        (v0 == minError) {
									pattern = (pattern<<1);
								} else if (v1 == minError) {
									pattern = (pattern<<1) + 1;
								}
							}
							
							if (errorLine<bestErrorLine) {
								bestErrorLine = errorLine;
								tile.pattern[ii] = pattern; 
								tile.color[ii] = palette.paletteCode[p0];
							}
						}
					}
				}					
			}


			tt.clear();
			for (auto &t : tiles) 
				tt[t.second]++;
				
			std::cout << "Size: " << tt.size() << " " << tiles.size() << " " << tileComp.begin()->first <<  std::endl;

		};
	}
	
	
	
	for (int i=0; i<src.rows; i+=8)
		for (int j=0; j<src.cols; j+=8)
			sRGB.add(tiles[tileMap(i/8,j/8)[0]].img(),tiles[tileMap(i/8,j/8)[1]].img()).copyTo(target(cv::Rect(j,i,8,8)));

	cv::Mat3i targetLin = cv::Mat3i(target);
	for (auto &p : targetLin)
		p = sRGB.toLin(p);
		
	imageQuantError = srcLin - targetLin;


	return std::make_pair(tileMap, tiles);

}

static std::pair<cv::Mat2i, std::map<size_t, Tile>> quantize105( const cv::Mat3b src, cv::Mat3b target) {

	std::map<size_t, Tile> tiles;
	cv::Mat2i tileMap(src.rows/8,src.cols/8);
	
	cv::Mat3i srcLin = cv::Mat3i(src);
	for (auto &p : srcLin)
		p = sRGB.toLin(p);

	{ // Find best pair of tiles for each 8x8 block
		
		std::map<Tile, size_t> differentTiles;
		for (size_t i=0; i<palette.colors.size()-1; i++)
			differentTiles[tiles[i] = Tile(0,240+i+1)] = i;
		differentTiles[tiles[palette.colors.size()-1] = Tile(255,254)] = palette.colors.size()-1;
		
		#pragma omp parallel for
		for (int i=0; i<src.rows; i+=8) { 
			
			#pragma omp critical
			std::cerr << i << " out of " << src.rows << std::endl;
			for (int j=0; j<src.cols; j+=8) {

				cv::Vec2i &selectedTiles = tileMap(i/8,j/8);

				Tile tile0, tile1;
				double errorTile = 0;
				for (int ii=0; ii<8; ii++) { // Selects row;

					double bestErrorLine = 1e100;
					for (auto &p105 : palette.dualPalette) {

						auto &P0 = palette.paletteLin[p105.first];
						auto &P1 = palette.paletteLin[p105.second];

						double errorLine = 0;
						uint8_t pattern0 = 0, pattern1 = 0;
						for (int jj=0; jj<8; jj++) {
							
							double v00, v01, v10, v11;
														
							cv::Vec3i c00 = (P0[0] + P1[0])/2;
							cv::Vec3i c01 = (P0[0] + P1[1])/2;
							cv::Vec3i c10 = (P0[1] + P1[0])/2;
							cv::Vec3i c11 = (P0[1] + P1[1])/2;

							cv::Vec3i cTargetLin = srcLin(i+ii,j+jj);
							cv::Vec3b cTarget = sRGB.tosRGB(cTargetLin);
								
							v00 = 0.996*comp(sRGB.tosRGB(c00), cTarget);
							v01 = 0.997*comp(sRGB.tosRGB(c01), cTarget);
							v10 = 0.998*comp(sRGB.tosRGB(c10), cTarget);
							v11 = 0.999*comp(sRGB.tosRGB(c11), cTarget);
							
							double minError = std::min(std::min(v00,v01),std::min(v10,v11));
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
							tile0.color[ii] = palette.paletteCode[p105.first];
							tile1.color[ii] = palette.paletteCode[p105.second];
						}
					}
					errorTile += bestErrorLine;
				}
				
				#pragma omp critical
				{
					if (not differentTiles.count(tile0))
						differentTiles[tile0] = tiles.size();	
					selectedTiles[0] = differentTiles[tile0];
					tiles[selectedTiles[0]] = tile0;
					
					if (not differentTiles.count(tile1))
						differentTiles[tile1] = tiles.size();	
					selectedTiles[1] = differentTiles[tile1];
					tiles[selectedTiles[1]] = tile1;
				}
			}
		}
	}
	
	{	// Summarize in order to have maximum 256 tiles
		std::map<Tile, size_t> tt;
		tt.clear();
		for (auto &t : tiles) 
			tt[t.second]++;

		std::cout << "Size: " << tt.size() << " " << tiles.size() << std::endl;
		
		while (tt.size()>256) {

			std::map<double, std::pair<int, int>> tileComp;
			for (auto &t0 : tiles) 
				for (auto &t1 : tiles) 
					if (t0.first<t1.first)
						tileComp[cv::norm(cv::Mat3d(t1.second.img())-cv::Mat3d(t0.second.img()))] = std::make_pair(t0.first,t1.first);
							
			tiles.erase(tileComp.begin()->second.second);


			{
				std::vector<std::pair<int,int>> affectedBlocks;
				std::map<size_t, std::reference_wrapper<Tile>> affectedTiles;

				for (int i=0; i<tileMap.rows; i++) {
					for (int j=0; j<tileMap.cols; j++) {
						if (tileMap(i,j)[0]==tileComp.begin()->second.second or 
							tileMap(i,j)[1]==tileComp.begin()->second.second) {
							
							if (tileMap(i,j)[0]==tileComp.begin()->second.second)
								tileMap(i,j)[0]=tileComp.begin()->second.first;

							if (tileMap(i,j)[1]==tileComp.begin()->second.second)
								tileMap(i,j)[1]=tileComp.begin()->second.first;
							
							if (tileMap(i,j)[0]>15) {
								affectedTiles.emplace(tileMap(i,j)[0], tiles[tileMap(i,j)[0]]);
								affectedBlocks.emplace_back(i,j);
							}
							if (tileMap(i,j)[1]>15) {
								affectedTiles.emplace(tileMap(i,j)[1], tiles[tileMap(i,j)[1]]);
								affectedBlocks.emplace_back(i,j);
							}
						}
					}
				}
				
				std::cerr << affectedBlocks.size() << " " << affectedTiles.size() << std::endl;	
				
				
				for (auto &&at : affectedTiles) {
					
					int tileIdx = at.first;
					Tile &tile = at.second.get();
					
					double bestErrorLine = 1e100;
					
					for (int ii=0; ii<8; ii++) { // Select tile row
							
						for (size_t p0=0; p0<palette.palette.size(); p0++) {

							double errorLine = 0;
							uint8_t pattern = 0;
							
							for (int jj=0; jj<8; jj++) { // Select tile col
								
								double v0 = 0., v1 = 0.;

								for (auto &&block : affectedBlocks) {
									
									auto colora0 = tiles[tileMap(block.first,block.second)[0]].img()(ii,jj);
									auto colorb0 = tiles[tileMap(block.first,block.second)[1]].img()(ii,jj);
									auto colora1 = colora0;
									auto colorb1 = colorb0;
									
									if (tileMap(block.first,block.second)[0]==tileIdx) {
										colora0 = palette.palette[p0][0];
										colora1 = palette.palette[p0][1];
									}
									if (tileMap(block.first,block.second)[1]==tileIdx) {
										colorb0 = palette.palette[p0][0];
										colorb1 = palette.palette[p0][1];
									}
										
									v0 += comp(sRGB.tosRGB((sRGB.toLin(colora0) + sRGB.toLin(colorb0))/2), src(block.first*8+ii,block.second*8+jj));
									v1 += comp(sRGB.tosRGB((sRGB.toLin(colora1) + sRGB.toLin(colorb1))/2), src(block.first*8+ii,block.second*8+jj));
								}
									
								double minError = std::min(v0,v1);
								errorLine += minError;
									
								if        (v0 == minError) {
									pattern = (pattern<<1);
								} else if (v1 == minError) {
									pattern = (pattern<<1) + 1;
								}
							}
							
							if (errorLine<bestErrorLine) {
								bestErrorLine = errorLine;
								tile.pattern[ii] = pattern; 
								tile.color[ii] = palette.paletteCode[p0];
							}
						}
					}
				}					
			}


			tt.clear();
			for (auto &t : tiles) 
				tt[t.second]++;
				
			std::cout << "Size: " << tt.size() << " " << tiles.size() << " " << tileComp.begin()->first <<  std::endl;

		};
	}
	
	
	
	for (int i=0; i<src.rows; i+=8)
		for (int j=0; j<src.cols; j+=8)
			sRGB.add(tiles[tileMap(i/8,j/8)[0]].img(),tiles[tileMap(i/8,j/8)[1]].img()).copyTo(target(cv::Rect(j,i,8,8)));

	return std::make_pair(tileMap, tiles);

}

int main(int argc, char *argv[]) {
	
	
/*			{   0,    0,    0},
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
		{ 202,  202,  202},
		{ 255,  255,  255}	*/
	
	TsRGB ts;
	
	std::clog << "Pallette 0: " << std::endl;
	std::clog <<  ts.add(cv::Vec3b(  0,    0,    0), {   0,    0,    0}) << std::endl;
	std::clog <<  ts.add(cv::Vec3b(  0,    0,    0), { 202,  202,  202}) << std::endl;
	std::clog <<  ts.add(cv::Vec3b(255,  255,  255), {   0,    0,    0}) << std::endl;
	std::clog <<  ts.add(cv::Vec3b(255,  255,  255), { 202,  202,  202}) << std::endl;


	std::clog << "Pallette 1: " << std::endl;
	std::clog <<  ts.add(cv::Vec3b(202,  202,  202), {   0,    0,    0}) << std::endl;
	std::clog <<  ts.add(cv::Vec3b(202,  202,  202), { 202,  202,  202}) << std::endl;
	std::clog <<  ts.add(cv::Vec3b(255,  255,  255), {   0,    0,    0}) << std::endl;
	std::clog <<  ts.add(cv::Vec3b(255,  255,  255), { 202,  202,  202}) << std::endl;

	std::clog << "Pallette 3: " << std::endl;
	std::clog <<  ts.add(cv::Vec3b( 59,  176,   33), {   0,    0,    0}) << std::endl;
	std::clog <<  ts.add(cv::Vec3b( 59,  176,   33), { 120,  220,   94}) << std::endl;
	std::clog <<  ts.add(cv::Vec3b(255,  255,  255), {   0,    0,    0}) << std::endl;
	std::clog <<  ts.add(cv::Vec3b(255,  255,  255), { 120,  220,   94}) << std::endl;

	std::clog << "Pallette 3: " << std::endl;
	std::clog <<  ts.add(cv::Vec3b( 59,  176,   33), { 120,  220,   94}) << std::endl;
	std::clog <<  ts.add(cv::Vec3b( 59,  176,   33), { 255,  255,  255}) << std::endl;
	std::clog <<  ts.add(cv::Vec3b(255,  255,  255), { 120,  220,   94}) << std::endl;
	std::clog <<  ts.add(cv::Vec3b(255,  255,  255), { 255,  255,  255}) << std::endl;

	std::clog << "Pallette 3: " << std::endl;
	std::clog <<  ts.add(cv::Vec3b( 59,  176,   33), { 120,  220,   94}) << std::endl;
	std::clog <<  ts.add(cv::Vec3b( 59,  176,   33), { 202,  202,  202}) << std::endl;
	std::clog <<  ts.add(cv::Vec3b(255,  255,  255), { 120,  220,   94}) << std::endl;
	std::clog <<  ts.add(cv::Vec3b(255,  255,  255), { 202,  202,  202}) << std::endl;
	//exit(-1);
	
	if (argc!=2) {
		std::cout << "Use: ./mkGif <file.gif>" << std::endl;
		return -1;
	}

	cv::VideoCapture cap(argv[1]);
	
	std::map<int,cv::Mat3i> imageQuantErrorCarry;
	
	cv::Mat3b img;
	while (cap.read(img)) {
		
		cv::Mat3d imgd = cv::Mat3d(img);
		//imgd = (imgd-202)*1.25 + 202;
		//imgd = (1.1*imgd)-0.1*255;
		
		//imgd = (imgd-224)*1.25+224;
		
		//for (auto &c : imgd) c[0]=250;
		img = cv::Mat3b(imgd);

//		img = img(cv::Rect(img.cols*0.1,img.rows*0.1,img.cols*0.8,img.rows*0.8)).clone();

		//img = img(cv::Rect(img.cols*0.45,img.rows*0.4,img.cols*0.5,img.rows*0.5)).clone();
		
		double scale = 2*96./img.rows;
		
		std::clog << img.cols << " " << img.rows << std::endl;
		
		cv::imshow("real_original", img.clone());
		
//		cv::resize(img, img, cv::Size(), scale, scale, cv::INTER_AREA);
		cv::resize(img, img, cv::Size(), scale, scale, cv::INTER_LANCZOS4);

		img = cv::Mat3b( (cv::Mat3f(img)-224)*1.25+224 );
			
		img = img(cv::Rect(0,0,img.cols&0xFFF8,img.rows&0xFFF8)).clone();
		//img = dither(img);
		std::cerr << img.rows << " " << img.cols << std::endl;
			
		cv::Mat3b work = img.clone();
		cv::Mat3b target;

		cv::resize(work, target, cv::Size(), 4, 4, cv::INTER_NEAREST);
		cv::imshow("original", target);
		
		
		if(1) {
			
			for (int l=0; l<img.rows; l+=64)  
				quantizeFast(img(cv::Rect(0,l,img.cols,std::min(64,img.rows-l))), work(cv::Rect(0,l,img.cols,std::min(64,img.rows-l))));
//				quantize105dither(img(cv::Rect(0,l,img.cols,std::min(64,img.rows-l))), work(cv::Rect(0,l,img.cols,std::min(64,img.rows-l))), imageQuantErrorCarry[l], 0.2);

//			cv::imwrite("out.png",work);
			cv::resize(work, target, cv::Size(), 4, 4, cv::INTER_NEAREST);
			cv::imshow("q_full", target);		
		}
		cv::waitKey(30);
	}
	cv::waitKey(0);
	return 0;
}
