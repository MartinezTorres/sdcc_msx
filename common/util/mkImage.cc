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
 
static const std::vector<cv::Vec3d> colors = {
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


static cv::Vec3d rgb2lab(cv::Vec3d bgr) {

	double r = bgr[2]/255.;
	double g = bgr[1]/255.;
	double b = bgr[0]/255.;
	
	r = 100*(r>0.04045?std::pow((r+0.055)/1.055,2.4):r/12.92);
	g = 100*(g>0.04045?std::pow((g+0.055)/1.055,2.4):g/12.92);
	b = 100*(b>0.04045?std::pow((b+0.055)/1.055,2.4):b/12.92);

	double x = (0.4124*r+0.3576*g+0.1805*b)/0.95047;
	double y = (0.2126*r+0.7152*g+0.0722*b)/1.00000;
	double z = (0.0193*r+0.1192*g+0.9505*b)/1.08883;

	x = (x>0.008856?std::pow(x,1/3.):(7.787*x)+16/116.);
	y = (y>0.008856?std::pow(y,1/3.):(7.787*y)+16/116.);
	z = (z>0.008856?std::pow(z,1/3.):(7.787*z)+16/116.);

	return cv::Vec3d(116*y-16, 500*(x-y), 200*(y-z));
}

// from:
// https://github.com/THEjoezack/ColorMine/blob/master/ColorMine/ColorSpaces/Comparisons/Cie94Comparison.cs

static double compareCie94(cv::Vec3d bgrA, cv::Vec3d bgrB){
	
	cv::Vec3d labA = rgb2lab(bgrA);
	cv::Vec3d labB = rgb2lab(bgrB);

	double deltaL = labA[0]-labB[0];
	double deltaA = labA[1]-labB[1];
	double deltaB = labA[2]-labB[2];

	double c1 = std::sqrt(labA[1]*labA[1]+labA[2]*labA[2]);
	double c2 = std::sqrt(labB[1]*labB[1]+labB[2]*labB[2]);
	double deltaC = c1-c2;
	double deltaH = deltaA*deltaA+deltaB*deltaB-deltaC*deltaC;
		deltaH = (deltaH<0?0:std::sqrt(deltaH));
	double sc = 1.0+0.045*c1;
	double sh = 1.0+0.015*c1;
	double deltaLKlsl = deltaL/1.0;
	double deltaCkcsc = deltaC/sc;
	double deltaHkhsh = deltaH/sh;
	double i = deltaLKlsl*deltaLKlsl + deltaCkcsc*deltaCkcsc + deltaHkhsh*deltaHkhsh;
	return i<0?0:std::sqrt(i);
}

static double compareBGR(cv::Vec3d bgrA, cv::Vec3d bgrB){ return cv::norm(bgrA-bgrB); }

static double compareLab(cv::Vec3d bgrA, cv::Vec3d bgrB){ return cv::norm(rgb2lab(bgrA)-rgb2lab(bgrB)); }

typedef std::function<double(cv::Vec3d,cv::Vec3d)> Fcomp;

static std::pair<double,std::array<uint8_t,8>> colorize(
	const cv::Vec3b src[8], 
	cv::Vec3b target[8], 
	std::vector<cv::Vec3d> &palette,
	Fcomp compare = compareBGR ) {
	
	std::pair<double,std::array<uint8_t,8>> ret;
	
	ret.first = 0;
	for (size_t i=0; i<8; i++) {
		
		double bestValue = 1e100;
		for (size_t k=0; k<palette.size(); k++) {
			double val = compare(src[i],palette[k]);
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
	std::vector<std::vector<cv::Vec3d>> &palettes,	
	Fcomp fast = compareBGR,
	Fcomp refinement = compareCie94) {

	std::map<double,std::pair<size_t,std::array<uint8_t,8>>> scores;
	
	for (size_t j=0; j<palettes.size(); j++) {
		
		std::pair<double,std::array<uint8_t,8>> sp = colorize(src,target,palettes[j],fast);
		scores[sp.first] = std::make_pair(j,sp.second);
	}
	
	{
		std::vector<size_t> bestPalettes;
		for (auto && s : scores) bestPalettes.push_back(s.second.first);
		
		scores.clear();
		for (size_t k=0; k<bestPalettes.size() and k<100; k++) {
			size_t j = bestPalettes[k];
			
			std::pair<double,std::array<uint8_t,8>> sp = colorize(src,target,palettes[j],refinement);
			scores[sp.first] = std::make_pair(j,sp.second);
		}
	}
			
	colorize(src,target,palettes[scores.begin()->second.first],refinement);

	return scores.begin()->second;
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
	
	std::vector<std::vector<cv::Vec3d>> msx1palette;
	for (size_t i=0; i<colors.size(); i++)
		for (size_t j=i+1; j<colors.size(); j++)
			msx1palette.emplace_back(std::vector<cv::Vec3d>{colors[i],colors[j]});

	std::vector<std::vector<cv::Vec3d>> msx1dualpalette;
	for (size_t i=0; i<colors.size(); i++) {
		for (size_t j=0; j<colors.size(); j++) {
			for (size_t ii=0; ii<colors.size(); ii++) {
				for (size_t jj=0; jj<colors.size(); jj++) {
					if (i+jj < i+ii) continue;
					if (j+ii < i+jj) continue;
					if (j+jj < j+ii) continue;
					msx1dualpalette.emplace_back(std::vector<cv::Vec3d>{
						0.5*(colors[i]+colors[ii]),
						0.5*(colors[i]+colors[jj]),
						0.5*(colors[j]+colors[ii]),
						0.5*(colors[j]+colors[jj]),
					});
				}
			}
		}
	}
	
	cv::Mat3b work = img.clone();
	cv::Mat3b target;
	volatile bool alive = true;
	volatile int key=0;
	std::thread t{[&](){
		while (alive) {
			cv::resize(work, target, cv::Size(), 4, 4, cv::INTER_NEAREST);
			cv::imshow("target", target);
			key = cv::waitKey(100);
		}
	}};
			
			
	
	int status = 0;
	while (true) {
		
		std::this_thread::sleep_for(100ms);
		
		if (key == -1) continue;

		status = ((key&255)-'0')%10;
		std::cout << status << std::endl;

		for (int i=0; i<img.rows; i+=8) {
			for (int j=0; j<img.cols; j+=8) {
				for (int ii=0; ii<8; ii++) {
					if (status==1) quantize(&img(i+ii,j), &work(i+ii,j), msx1palette, compareBGR, compareBGR);
					if (status==2) quantize(&img(i+ii,j), &work(i+ii,j), msx1palette, compareBGR, compareLab);
					if (status==3) quantize(&img(i+ii,j), &work(i+ii,j), msx1palette, compareBGR, compareCie94);
					if (status==4) quantize(&img(i+ii,j), &work(i+ii,j), msx1dualpalette, compareBGR, compareBGR);
					if (status==5) quantize(&img(i+ii,j), &work(i+ii,j), msx1dualpalette, compareLab, compareLab);
					if (status==6) quantize(&img(i+ii,j), &work(i+ii,j), msx1dualpalette, compareBGR, compareLab);
					if (status==7) quantize(&img(i+ii,j), &work(i+ii,j), msx1dualpalette, compareBGR, compareCie94);
				}
			}
		}
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
