////////////////////////////////////////////////////////////////////////
// Linker for Konami 5 megaroms
//
// Manuel Martinez (salutte@gmail.com)
//
// FLAGS: -std=c++14 -O0

#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <set>
#include <vector>
#include <functional>
#include <algorithm>

namespace { // MiniLog
	class Log {
		int level;
		std::stringstream * const sstr;
	public:
		static int &reportLevel() { static int rl = 0; return rl; };
		static int reportLevel(int l) { return reportLevel()=l; };
		Log (int level) : level(level), sstr(level>=reportLevel()?new std::stringstream():nullptr) {}
		~Log() {
			if (sstr) {
				if (level==0) std::cerr << "\x1b[34;1m";
				if (level==1 or level==-2) std::cerr << "\x1b[32;1m";
				if (level>=2 or level==-1) std::cerr << "\x1b[31;1m";
				std::cerr << "L" << level << " ";				

				std::cerr << sstr->str() << std::endl;
				std::cerr << "\x1b[0m";
				delete sstr;
			}
		}
		template<typename T> Log &operator<<(const T &v) { if (sstr) *sstr << v; return *this; }
		void operator<<(std::nullptr_t) {}
	};
}


struct AR { //ASSERT READ;
	
	std::string expected;
	AR(std::string expected) : expected(expected) {}
	
	friend std::istream & operator>>(std::istream &is, const AR& ar) {
		std::string read;
		is >> read;
		if (read != ar.expected) 
			throw std::runtime_error("Read: " + read + ", expected: " + ar.expected);
		return is;
	}
};

struct HEX { //READ HEXADECIMAL VALUE
	
	enum FORMAT {
		
		PLAIN, TWO_NIBBLES
	};
	
	struct HEX2DEC : std::vector<int> {
		HEX2DEC() {
			resize(256,0);
			for (int i='0'; i<='9'; i++)
				at(i) = i-'0';
				
			for (int i='A'; i<='F'; i++)
				at(i) = 10+ i-'A';

			for (int i='a'; i<='f'; i++)
				at(i) = 10+ i-'a';
		}
	};
	
	static size_t Hex2Dec(int v) {
		static HEX2DEC HD;
		return HD[v];
	}
	
	size_t &value;
	FORMAT format;
	
	HEX(size_t &value, FORMAT format) : value(value), format(format) {}
	
	friend std::istream &operator>>(std::istream &is, HEX&& hex) {
		
		hex.value = 0;
		
		if (hex.format==TWO_NIBBLES) {
			
			std::string s;
			if (not (is >> s)) return is;
			if (s.size()!=2) throw std::runtime_error("Not an hex byte");
			
			hex.value += Hex2Dec(s[0])<<4;
			hex.value += Hex2Dec(s[1])<<0;

		} else if (hex.format==PLAIN) {
			
			std::string s;
			if (not (is >> s)) throw std::runtime_error("Could not read expected value");

			for (auto &c : s)
				hex.value = hex.value * 16 + Hex2Dec(c);
		}
		return is;
	}
};

struct HEX2 : public HEX { HEX2(size_t &value) : HEX(value, HEX::TWO_NIBBLES) {} };

struct REL {
	
	
	struct AREA {
		
		std::string name;
		size_t size;
		size_t addr;
		size_t rom_addr;
		enum { ABSOLUTE, RELATIVE} type;
		
	};

	struct SYMBOL {
		
		std::string name;
		size_t addr;
		enum { DEF, REF} type;
		std::string areaName;
		
		size_t absoluteAddress;
	};
	
	std::string filename, name;
	std::vector<AREA> areas;
	std::vector<SYMBOL> symbols;
	
	bool enabled = false;
	int page = 0;
	int segment = 0;
};

////////////////////////////////////////////////////////////////////////
// STRUCTURE:
// MAIN BANKS: segments 0 and 1 are fixed to 0x4000 to 0x8000
// CODE BANK: fixed to 0x8000 to 0xA000
// DATA BANK: fixed to 0xA000 to 0xC000

////////////////////////////////////////////////////////////////////////
// BANKED CALL: 

// CASE A: FROM CODE BANK TO MAIN BANK -> nothing special is needed
// CASE C: FROM MAIN BANK TO CODE BANK -> load segment first, then normal call and return.
// CASE D: FROM CODE BANK TO DIFFERENT CODE BANK -> unsupported.

// HOW TO USE:
// Place a "USING(moduleName)" statement in the prologue of the code, 
// this defines an external symbol which will be used by the linker (prevents a warning).
// 
// The SEGMENT(moduleName) macro returns the segment where the moduleName is stored.
// this macro actually compiles to a call to a phony name. THis call is intercepted
// by the linker who knows where the module is placed.
//
// The load_code_segment(segment) function loads the segment at direction 0x8000, 
// and returns the previous segment loaded at 0x8000.
// The restore_code_segment(segment) loads the segment at direction 0x8000.


////////////////////////////////////////////////////////////////////////
// BANKED DATA ACCESS: 
// Allows using data stored at 0xA000 to 0xC000 segment

// HOW TO USE:
// Use the USING macro, as with the code.
//
// The load_data_segment(segment) function loads the segment at direction 0xA000, 
// and returns the previous segment loaded at 0xA000.
// The restore_data_segment(segment) loads the segment at direction 0xA000.

////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
	
	Log::reportLevel(10);
	
	std::string romName = "out.rom";
	for (int i=1; i<argc; i++) 
		if (std::string(argv[i]).size()-std::string(argv[i]).find(".rom") == 4)
			romName = std::string(argv[i]);
	
	
	std::set<std::string> known_areas = { "_CODE", "_DATA", "_GSINIT", "_GSFINAL", "_HEADER0" };
	std::vector<REL> rels;
	
	// PREPROCESS ALL INPUT FILES
	for (int i=1; i<argc; i++) {
		
		if (std::string(argv[i]).size()-std::string(argv[i]).find(".rel") != 4) continue;
		Log(1) << "Processing: " << argv[i];
		
		std::ifstream isf(argv[i]);
		std::string line;
		
		REL rel;
		rel.filename = argv[i];
		
		Log(0) << " File: " << argv[i];
		
		while (std::getline(isf, line)) {
			
			std::istringstream isl(line);
			std::string type;
			isl >> type;

			if (type=="XL2") { // DEFAULT
			} else if (type=="M") { // NOT REQUIRED
				
				isl >> rel.name;
				for (auto &r : rels)
					if (r.name == rel.name)
						throw std::runtime_error("Duplicate module ID: " + rel.name);
				
				//std::cout << "Module: " << rel.name << std::endl;
				
			} else if (type=="O") { // NOT NEEDED
			} else if (type=="H") { // NOT NEEDED
			} else if (type=="S") {
				
				REL::SYMBOL symbol;
				std::string st = "   ";
				
				isl >> symbol.name >> st[0] >> st[1] >> st[2] >> HEX(symbol.addr, HEX::PLAIN);
				
					
				if (st=="Def") {
					symbol.type = REL::SYMBOL::DEF;
				} else if (st=="Ref") {
					symbol.type = REL::SYMBOL::REF;
				} else throw std::runtime_error("Symbol type unepxepcted");
				
				if (not rel.areas.empty())
					symbol.areaName = rel.areas.back().name;
				
				rel.symbols.push_back(symbol);
				
			} else if (type=="A") {
				
				REL::AREA area;
				
				size_t flags;
				isl >> area.name >> AR("size") >> HEX(area.size,HEX::PLAIN) 
					>> AR("flags") >> flags 
					>> AR("addr") >> HEX(area.addr,HEX::PLAIN);
				
				if (flags==0) {
					area.type = REL::AREA::RELATIVE;
				} else if (flags==8) {
					area.type = REL::AREA::ABSOLUTE;
				} else throw std::runtime_error("Unexpected flag");
				
				
				if (area.size>0) Log(1) << "Found area: " << area.name << " of size: " << area.size;
				if (area.size>0 and known_areas.count(area.name) == 0) throw std::runtime_error("Area " + area.name +" unknown");
				
				if (area.name=="_HEADER0") {
					rel.enabled=true;
					rel.page=0;
				}

				rel.areas.push_back(area);
				
			} else if (type=="T") { // NOT NOW
			} else if (type=="R") { // NOT NOW
			} else if (not type.empty()) {
				
				throw std::runtime_error("Unrecognized type: " + type);
			}
		}
	
		rels.push_back(rel);
		if (rel.enabled)
			std::swap(rels.front(),rels.back());
	}

	// FAST ERROR CHECKING
	if (rels.empty()) throw std::runtime_error("No files to parse");
	
	
	// ENABLE ALL REQUIRED FILES / MODULES
	for (;;) {
		
		bool updated = false;

		std::map<std::string,int> referencedSymbols;
		std::set<std::string> definedSymbols;

		for (auto &rel : rels) {
			if (not rel.enabled) continue;
			for (auto &sym : rel.symbols) {
				if (sym.type!=REL::SYMBOL::REF) continue;
				if (sym.name.find("_K5_SEGMENT_TO_PAGE_")==0) {
					std::string moduleName = sym.name.substr(std::string("_K5_SEGMENT_TO_PAGE_X_").size());
					for (auto &rel2 : rels) {
						if (rel2.enabled) continue;
						if (rel2.name != moduleName) continue;
						rel2.enabled = true;
						updated = true;
					}
					continue;
				}
				referencedSymbols[sym.name] = 0;
			}
		}

		for (auto &rel : rels) {
			for (auto &sym : rel.symbols) {
				if (sym.type!=REL::SYMBOL::DEF) continue;
					
				if (not rel.enabled and referencedSymbols.count(sym.name)) {
					rel.enabled = true;
					updated = true;
				}
				
				if (rel.enabled and referencedSymbols.count(sym.name)) {
					if (definedSymbols.count(sym.name)) throw std::runtime_error("Symbol: " + sym.name + "defined multiple times");
					definedSymbols.insert(sym.name);
					referencedSymbols[sym.name]++;
				}
			}
		}
		
		for (auto &ref : referencedSymbols)
			if (ref.second==0)
				throw std::runtime_error("Referenced Symbol: " + ref.first + " not defined");
				
		if (not updated) break;
	}

	
	{
		std::map<std::string,REL *> modulesByName;

		for (auto &&rel : rels)
			if (rel.enabled)
				modulesByName.emplace(rel.name,&rel);
				
		for (auto &&mod : modulesByName)
			Log(3) << mod.first;

		for (auto &&rel : rels) {
			if (not rel.enabled) continue;
			for (auto &sym : rel.symbols) {
				if (sym.type!=REL::SYMBOL::REF) continue; 
				if (sym.name.find("_K5_SEGMENT_TO_PAGE_")!=0) continue;
				
				int page = sym.name.substr(std::string("_K5_SEGMENT_TO_PAGE_").size(),1)[0]-'A';
				std::string moduleName = sym.name.substr(std::string("_K5_SEGMENT_TO_PAGE_X_").size());
				
				if (modulesByName.count(moduleName)==0)
					throw std::runtime_error("Module " + moduleName + " unknown");
				
				if (modulesByName[moduleName]->page==0)
					modulesByName[moduleName]->page = page;
					
				if (modulesByName[moduleName]->page != page)
					throw std::runtime_error("Module " + moduleName + " required at different pages");
			}
		}

		for (auto &&rel : rels) {
			if (not rel.enabled) continue;
			for (auto &sym : rel.symbols) {
				if (sym.type!=REL::SYMBOL::REF) continue; 
				if (sym.name.find("_K5_SEGMENT_TO_PAGE_")!=0) continue;
				
				std::string moduleName = sym.name.substr(std::string("_K5_SEGMENT_TO_PAGE_X_").size());
				
				if (modulesByName[moduleName]->page == rel.page)
					std::cerr << "Warning: Module " << rel.name << " is loading " << moduleName << " in its own page" << std::endl;
			}
		}
	}
	
	// ALLOCATE ALL NON BANKABLE AREAS
	size_t rom_ptr = 0x4000;
	size_t ram_ptr = 0xC000;
	{
		for (auto &rel : rels) {
			if (not rel.enabled) continue;
			for (auto &area:  rel.areas) {
				if (area.name!="_HEADER0") continue;
				if (rom_ptr>0x4000) throw std::runtime_error(area.name + " defined more than once: " + rel.filename);
				if (area.type != REL::AREA::ABSOLUTE) throw std::runtime_error(area.name + " not absolute: " + rel.filename);
				if (area.addr != 0x4000) throw std::runtime_error("HEADER not at 0x4000: " + rel.filename);

				area.rom_addr = area.addr - 0x4000;
				rom_ptr += area.size;
			}
		}
		
		for (auto &rel : rels) {
			if (not rel.enabled) continue;
			if (not rel.page==0) continue;
			for (auto &area:  rel.areas) {
				if (area.name!="_CODE") continue;
				if (area.type != REL::AREA::RELATIVE) throw std::runtime_error(area.name + " not relative: " + rel.filename);

				area.addr = rom_ptr;
				area.rom_addr = area.addr - 0x4000;
				rom_ptr += area.size;

				Log(1) << "Placed: " << rel.name << " at: 0x" << std::hex << area.addr << std::dec << " (" << area.size << " bytes) in page: " << rel.page << " and segment: " << rel.segment;
			}
		}

		for (auto &&target_area : std::vector<std::string>{"_GSINIT", "_GSFINAL"}) {
			for (auto &rel : rels) {
				if (not rel.enabled) continue;
				for (auto &area:  rel.areas) {
					if (area.name!=target_area) continue;
					if (area.type != REL::AREA::RELATIVE) throw std::runtime_error(area.name + " not relative: " + rel.filename);

					area.addr = rom_ptr;
					area.rom_addr = area.addr - 0x4000;
					rom_ptr += area.size;
				}
			}
		}

		for (auto &rel : rels) {
			if (not rel.enabled) continue;
			for (auto &area:  rel.areas) {
				if (area.name!="_DATA") continue;
				if (area.type != REL::AREA::RELATIVE) throw std::runtime_error(area.name + " not relative: " + rel.filename);

				area.addr = ram_ptr;
				area.rom_addr = 0;
				ram_ptr += area.size;
			}
		}		
	}

	// ALLOCATE BANKABLE CODE AREAS
	{	
		std::vector<std::pair<size_t,std::reference_wrapper<REL>>> bankableRels;
		
		for (auto &rel : rels) {
			if (not rel.enabled) continue;
			if (not rel.page!=0) continue;
			for (auto &area:  rel.areas) {
				if (area.name!="_CODE") continue;
				if (area.type != REL::AREA::RELATIVE) throw std::runtime_error(area.name + " not relative: " + rel.filename);
				
				bankableRels.emplace_back(area.size,std::ref(rel));
				
				if (area.size>0x2000) throw std::runtime_error("File: " + rel.name + " too large to fit a segment");
			}
		}
		
		std::sort(bankableRels.begin(), bankableRels.end(), 
		[](const std::pair<size_t,std::reference_wrapper<REL>> &rhs,const std::pair<size_t,std::reference_wrapper<REL>> &lhs){
			return rhs.first < lhs.first;
		});
		std::reverse(bankableRels.begin(), bankableRels.end());

		std::vector<size_t> segments;
		segments.push_back(0x6000-rom_ptr);
		
		for (auto &prel: bankableRels) {
			size_t i;
			for (i=0; i<segments.size() and segments[i]<prel.first; i++);
			if (i==segments.size()) 
				segments.push_back(0x2000);

			for (auto &a : prel.second.get().areas) {
				if (a.name=="_CODE") {
					a.addr = 0x2000*(2+prel.second.get().page) + 0x2000 - segments[i]; 
					a.rom_addr = 0x2000*i + 0x2000-segments[i];
				}
			}
			
			segments[i] -= prel.first;
			
			prel.second.get().segment = i;
			
			for (auto &a : prel.second.get().areas)
				if (a.name=="_CODE")
					Log(2) << "Module: " << prel.second.get().name << " addressed at: 0x" << std::hex << a.addr << std::dec << " (" << prel.first << " bytes) in page: " << prel.second.get().page << " and segment " << prel.second.get().segment;
		}
	}
	
	
	// Generate area map
	{
		std::ofstream off(romName + ".areas.map");
		off << "AREA MAP: " << std::endl;
		off << "# SG #  MAP #  ROM  # SIZE #   NAME   #   PAGE A   #   PAGE B   #   PAGE C   #   PAGE D   #" << std::endl;
		off << "###########################################################################################" << std::endl;
		for (size_t i=0; i<256; i++) {
			
			std::multimap<size_t, std::string> lines;
			
			for (auto &rel : rels) {
				if (rel.segment != (int)i) continue;
				if (not rel.enabled) continue;
				for (auto &area:  rel.areas) {
					if (area.size==0) continue;
					if (area.name=="_DATA") continue;
						
					std::ostringstream oss;
					
					char s[200];
					snprintf(s,199,"#%3X # %04lX # %05lX # %04lX # %8.8s #",rel.segment, area.addr, area.rom_addr, area.size, area.name.substr(1).c_str());	
					oss << s;
					for (int j=0; j<rel.page; j++) oss << "            #";
					snprintf(s,199," %10.10s #",rel.name.c_str());
					oss << s;	
					for (int j=rel.page+1; j<4; j++) oss << "            #";
					lines.emplace(area.addr,oss.str());
					
				}	
			}
			for (auto &&s : lines)
				off << s.second << std::endl;
			if (not lines.empty()) 
				off << "###########################################################################################" << std::endl;

		}
	}

	// Generate symbols map
	{
		std::ofstream off(romName + ".symbols.map");
		off << "Symbols MAP: " << std::endl;
		off << "# SG #  MAP #  ROM  #  MODULE  #        PAGE A        #        PAGE B        #        PAGE C        #        PAGE D        #" << std::endl;
		off << "############################################################################################################################" << std::endl;
		for (size_t i=0; i<256; i++) {
			
			std::multimap<size_t, std::string> lines;
			
			for (auto &rel : rels) {
				if (rel.segment != (int)i) continue;
				if (not rel.enabled) continue;
				for (auto &area:  rel.areas) {
					if (area.size==0) continue;
					if (area.name=="_DATA") continue;

					for (auto &symbol : rel.symbols) {
						if (symbol.type != REL::SYMBOL::DEF) continue;
						if (symbol.areaName != area.name) continue;
						
						std::ostringstream oss;
						
						char s[200];
						snprintf(s,199,"#%3X # %04lX # %05lX # %-8.8s #",rel.segment, area.addr + symbol.addr, area.rom_addr + symbol.addr, rel.name.c_str());	
						oss << s;
						for (int j=0; j<rel.page; j++) oss << "                      #";
						snprintf(s,199," %-20.20s #",symbol.name.c_str());
						oss << s;	
						for (int j=rel.page+1; j<4; j++) oss << "                      #";
						lines.emplace(area.addr,oss.str());
					
					}
				}	
			}
			for (auto &&s : lines)
				off << s.second << std::endl;
			if (not lines.empty()) 
				off << "############################################################################################################################" << std::endl;

		}
	}

	Log(2) << "Allocated: " << (rom_ptr-0x4000) << " bytes of ROM";
	if (rom_ptr>0x6000) throw std::runtime_error("Main segment ROM doesn't fit 8KB");

	Log(2) << "Allocated: " << (ram_ptr-0xC000) << " bytes of RAM";		
	if (ram_ptr>0xF000) throw std::runtime_error("RAM usage is larger than 12KB");
	
	// DO LABEL SYMBOL ADDRESSES
	std::map<std::string,size_t> symbolsAddress;
	for (auto &rel : rels) {
		if (not rel.enabled) continue;
		std::map<std::string,size_t> areaAddress;
		for (auto &area:  rel.areas) 
			areaAddress[area.name] = area.addr;
			
		for (auto &symbol : rel.symbols) {
			if (symbol.type == REL::SYMBOL::DEF) {
				symbolsAddress[symbol.name] = areaAddress[symbol.areaName] + symbol.addr;
				symbol.absoluteAddress = symbolsAddress[symbol.name];
				if (symbol.name[0]!='.') 
					Log(2) << "Symbol: " << symbol.name << " defined at: 0x" << std::hex << symbol.absoluteAddress << std::dec << " at page: " << rel.page;
			}
		}
	}
	
	// DO EXTRACT THE CODE
	std::vector<uint8_t> rom(0x20000,0xff);
	for (auto &rel : rels) {		
		if (not rel.enabled) continue;
		
		std::ifstream isf(rel.filename);
		std::string line;
		
		size_t current_area=0;
		std::vector<int> area_addr;
		std::vector<int> area_rom_addr;
		for (auto &area : rel.areas) {
			if (area.type == REL::AREA::RELATIVE) {
				area_addr.push_back(area.addr); 
				area_rom_addr.push_back(area.rom_addr); 
			} else {
				if (area.size)
					Log(3) << "Module: " << rel.name << " Area: " << area.name << " " << area.addr << " " << area.rom_addr;
				area_addr.push_back(0);
				area_rom_addr.push_back(-0x4000);
			}
		}
			
		size_t last_t_pos=0;
		std::vector<uint8_t> T;
		
		
		while (std::getline(isf, line)) {
			
			std::istringstream isl(line);
			std::string type;
			isl >> type;

			if (type=="XL2") { // DEFAULT
			} else if (type=="M") { // NOT HERE
			} else if (type=="O") { // NOT NEEDED
			} else if (type=="H") { // NOT NEEDED
			} else if (type=="S") { // NOT HERE
			} else if (type=="A") { // NOT HERE
			} else if (type=="T") { // HERE
				
				size_t xx0, xx1;
				isl >> HEX(xx0,HEX::TWO_NIBBLES) >> HEX(xx1,HEX::TWO_NIBBLES);
				last_t_pos = xx1*0x100 + xx0 + area_rom_addr[current_area];
				
				T.clear();
				size_t nn;
				while (isl >> HEX(nn,HEX::TWO_NIBBLES))
					T.push_back(nn);
				
			} else if (type=="R") { // HERE

				size_t aa0, aa1;
				isl >> AR("00") >> AR("00") >> HEX2(aa0) >> HEX2(aa1);
				current_area = aa1*0x100 + aa0;

				size_t n1, n2, xx0, xx1;
				size_t n2Adjust = 2;
				while (isl >> HEX2(n1) >> HEX2(n2) >> HEX2(xx0) >> HEX2(xx1)) {

					enum { 
						R3_WORD=0x00, R3_BYTE=0x01, 
						R3_AREA=0x00, R3_SYM =0x02, 
						R3_NORM=0x00, R3_PCR =0x04, 
						R3_BYT1=0x00, R3_BYTX=0x08, 
						R3_SGND=0x00, R3_USGN=0x10,
						R3_LSB =0x00, R3_MSB =0x80
					};
					
					size_t idx = xx1*0x100 + xx0;
					size_t address;
					
					if (n2 <n2Adjust) 
						throw std::runtime_error("n2 < n2Adjust??");
					n2-=n2Adjust;

					
					if ( n1 & R3_SYM ) {
						
						if (symbolsAddress.count(rel.symbols[idx].name)!=0)  {

							address = symbolsAddress[rel.symbols[idx].name];
						} else if (rel.symbols[idx].name.find("_K5_SEGMENT_TO_PAGE_")==0) {
							
							std::string requested_module = 
								rel.symbols[idx].name.substr(
									std::string("_K5_SEGMENT_TO_PAGE_").size()+2
								);
							//Log(10) << "Looking for module: " << requested_module;
							for (auto &rel2 : rels) {
								if (rel2.name == requested_module) {
									address = rel2.segment;
									Log(0) << "FOUND! for module: " << requested_module << " " << rel2.name << " " << rel2.segment;
								}
							}
						
						} else {
							throw std::runtime_error("Undefined symbol: " + rel.symbols[idx].name); 
						}
						
						Log(3) << rel.symbols[idx].name << " " << std::hex << address;
						
						n1 -= R3_SYM;
					} else  {
					
						address = area_addr[idx];
					}
					
					
					if        (n1 == R3_WORD ) {

						address += T[n2+0] + T[n2+1]*0x100;
						
						T[n2+0] = address & 0xFF;
						T[n2+1] = address >> 8;
					
					} else if (n1 == R3_BYTE + R3_BYTX + R3_LSB) {
						
						address += T[n2+0] + T[n2+1]*0x100;

						for (size_t i=n2+1; i<T.size(); i++) 
							T[i-1] = T[i];
						T.pop_back();

						T[n2+0] = address & 0xFF;
						
						n2Adjust++;

					} else if (n1 == R3_BYTE + R3_BYTX + R3_MSB) {
						
						address += T[n2+0] + T[n2+1]*0x100;

						for (size_t i=n2+1; i<T.size(); i++) 
							T[i-1] = T[i];
						T.pop_back();

						T[n2+0] = (address>>8) & 0xFF;
						
						n2Adjust++;


					} else {
						Log(3) << "N1: 0x"<< std::hex << n1 << std::dec;
						throw std::runtime_error("Unsupported relocation flag combination");
					}
				}	


				if (T.size())
					while (rom.size() < last_t_pos + T.size()) 
						rom.resize(rom.size()+0x2000,0xff);
				
				//if (T.size()) Log(0) << rel.name << " " << std::hex << last_t_pos;

//				if (T.size()) Log(4) << std::hex << (last_t_pos - 0x2000*(rel.segment - rel.page)) << " " << rel.segment << " " << rel.page;
					
				
//				for (auto &t : T) rom[last_t_pos++] = t;
				for (auto &t : T) rom[last_t_pos++] = t;

			} else if (not type.empty()) {
				
				std::runtime_error("Unrecognized type: " + type);
			}
		}
	}


	// DO WRITE THE ROM
	{
		std::ofstream off(romName);
		off.write((const char *)&rom[0x0000],rom.size()-0x0000);
	}
		
	return 0;
}
