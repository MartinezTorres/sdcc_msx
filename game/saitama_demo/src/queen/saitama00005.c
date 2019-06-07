#include "saitama_gen.h"
static void saitama00005_sendPatterns(){
fast_load_page_d(SEGMENT_D(saitamapatternStorage00000));
gif_v00_setTargetAddress(792);
gif_v00_setSourceAddress(4216);
gif_v00_copy2();
fast_load_page_d(SEGMENT_D(saitamapatternStorage00000));
gif_v00_setTargetAddress(1816);
gif_v00_setSourceAddress(4224);
gif_v00_copy2();
fast_load_page_d(SEGMENT_D(saitamacolorStorage00000));
gif_v00_setTargetAddress(10008);
gif_v00_setSourceAddress(3776);
gif_v00_copy2();
fast_load_page_d(SEGMENT_D(saitamacolorStorage00000));
gif_v00_setTargetAddress(10008);
gif_v00_setSourceAddress(3776);
gif_v00_copy2();
}
static void saitama00005_sendPatternNames() {
fast_load_page_d(SEGMENT_D(saitama00000line00));
gif_v00_fillNameTableRow(saitama00000line00);
fast_load_page_d(SEGMENT_D(saitama00003line01));
gif_v00_fillNameTableRow(saitama00003line01);
fast_load_page_d(SEGMENT_D(saitama00003line02));
gif_v00_fillNameTableRow(saitama00003line02);
fast_load_page_d(SEGMENT_D(saitama00005line03));
gif_v00_fillNameTableRow(saitama00005line03);
fast_load_page_d(SEGMENT_D(saitama00005line04));
gif_v00_fillNameTableRow(saitama00005line04);
fast_load_page_d(SEGMENT_D(saitama00005line05));
gif_v00_fillNameTableRow(saitama00005line05);
fast_load_page_d(SEGMENT_D(saitama00005line06));
gif_v00_fillNameTableRow(saitama00005line06);
fast_load_page_d(SEGMENT_D(saitama00005line07));
gif_v00_fillNameTableRow(saitama00005line07);
fast_load_page_d(SEGMENT_D(saitama00004line08));
gif_v00_fillNameTableRow(saitama00004line08);
fast_load_page_d(SEGMENT_D(saitama00004line09));
gif_v00_fillNameTableRow(saitama00004line09);
fast_load_page_d(SEGMENT_D(saitama00004line10));
gif_v00_fillNameTableRow(saitama00004line10);
fast_load_page_d(SEGMENT_D(saitama00004line11));
gif_v00_fillNameTableRow(saitama00004line11);
fast_load_page_d(SEGMENT_D(saitama00004line12));
gif_v00_fillNameTableRow(saitama00004line12);
fast_load_page_d(SEGMENT_D(saitama00004line13));
gif_v00_fillNameTableRow(saitama00004line13);
fast_load_page_d(SEGMENT_D(saitama00004line14));
gif_v00_fillNameTableRow(saitama00004line14);
fast_load_page_d(SEGMENT_D(saitama00004line15));
gif_v00_fillNameTableRow(saitama00004line15);
fast_load_page_d(SEGMENT_D(saitama00002line16));
gif_v00_fillNameTableRow(saitama00002line16);
fast_load_page_d(SEGMENT_D(saitama00002line17));
gif_v00_fillNameTableRow(saitama00002line17);
fast_load_page_d(SEGMENT_D(saitama00001line18));
gif_v00_fillNameTableRow(saitama00001line18);
fast_load_page_d(SEGMENT_D(saitama00004line19));
gif_v00_fillNameTableRow(saitama00004line19);
fast_load_page_d(SEGMENT_D(saitama00000line20));
gif_v00_fillNameTableRow(saitama00000line20);
fast_load_page_d(SEGMENT_D(saitama00001line21));
gif_v00_fillNameTableRow(saitama00001line21);
fast_load_page_d(SEGMENT_D(saitama00003line22));
gif_v00_fillNameTableRow(saitama00003line22);
fast_load_page_d(SEGMENT_D(saitama00001line23));
gif_v00_fillNameTableRow(saitama00001line23);
}
void saitama00005() {
  saitama00005_sendPatterns();
  while (frames_left) HALT();
  frames_left = 1;
  if (hz==50) {
      gif_v00_setTargetAddress(MODE2_ADDRESS_PN0);
      gif_v00_setNameTableOffset(0);
      saitama00005_sendPatternNames();
      while (frames_left) HALT();
      frames_left = 3;
      gif_v00_setTargetAddress(MODE2_ADDRESS_PN1);
      gif_v00_setNameTableOffset(128);
      saitama00005_sendPatternNames();
  } else {
      gif_v00_setTargetAddress(MODE2_ADDRESS_PN0);
      gif_v00_setNameTableOffset(0);
      saitama00005_sendPatternNames();
      while (frames_left) HALT();
      frames_left = 4;
      gif_v00_setTargetAddress(MODE2_ADDRESS_PN1);
      gif_v00_setNameTableOffset(128);
      saitama00005_sendPatternNames();
  }
}
