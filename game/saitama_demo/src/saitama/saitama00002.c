#include "saitama_gen.h"
static void saitama00002_sendPatterns(){
}
static void saitama00002_sendPatternNames() {
fast_load_page_d(SEGMENT_D(saitama00001line00));
gif_v00_fillNameTableRow(saitama00001line00);
fast_load_page_d(SEGMENT_D(saitama00001line01));
gif_v00_fillNameTableRow(saitama00001line01);
fast_load_page_d(SEGMENT_D(saitama00001line02));
gif_v00_fillNameTableRow(saitama00001line02);
fast_load_page_d(SEGMENT_D(saitama00001line03));
gif_v00_fillNameTableRow(saitama00001line03);
fast_load_page_d(SEGMENT_D(saitama00001line04));
gif_v00_fillNameTableRow(saitama00001line04);
fast_load_page_d(SEGMENT_D(saitama00001line05));
gif_v00_fillNameTableRow(saitama00001line05);
fast_load_page_d(SEGMENT_D(saitama00001line06));
gif_v00_fillNameTableRow(saitama00001line06);
fast_load_page_d(SEGMENT_D(saitama00001line07));
gif_v00_fillNameTableRow(saitama00001line07);
fast_load_page_d(SEGMENT_D(saitama00001line08));
gif_v00_fillNameTableRow(saitama00001line08);
fast_load_page_d(SEGMENT_D(saitama00001line09));
gif_v00_fillNameTableRow(saitama00001line09);
fast_load_page_d(SEGMENT_D(saitama00001line10));
gif_v00_fillNameTableRow(saitama00001line10);
fast_load_page_d(SEGMENT_D(saitama00001line11));
gif_v00_fillNameTableRow(saitama00001line11);
fast_load_page_d(SEGMENT_D(saitama00001line12));
gif_v00_fillNameTableRow(saitama00001line12);
fast_load_page_d(SEGMENT_D(saitama00001line13));
gif_v00_fillNameTableRow(saitama00001line13);
fast_load_page_d(SEGMENT_D(saitama00001line14));
gif_v00_fillNameTableRow(saitama00001line14);
fast_load_page_d(SEGMENT_D(saitama00001line15));
gif_v00_fillNameTableRow(saitama00001line15);
fast_load_page_d(SEGMENT_D(saitama00001line16));
gif_v00_fillNameTableRow(saitama00001line16);
fast_load_page_d(SEGMENT_D(saitama00001line17));
gif_v00_fillNameTableRow(saitama00001line17);
fast_load_page_d(SEGMENT_D(saitama00001line18));
gif_v00_fillNameTableRow(saitama00001line18);
fast_load_page_d(SEGMENT_D(saitama00001line19));
gif_v00_fillNameTableRow(saitama00001line19);
fast_load_page_d(SEGMENT_D(saitama00001line20));
gif_v00_fillNameTableRow(saitama00001line20);
fast_load_page_d(SEGMENT_D(saitama00001line21));
gif_v00_fillNameTableRow(saitama00001line21);
fast_load_page_d(SEGMENT_D(saitama00001line22));
gif_v00_fillNameTableRow(saitama00001line22);
fast_load_page_d(SEGMENT_D(saitama00001line23));
gif_v00_fillNameTableRow(saitama00001line23);
}
void saitama00002() {
  saitama00002_sendPatterns();
  while (frames_left) HALT();
  frames_left = 1;
  if (hz==50) {
      gif_v00_setTargetAddress(MODE2_ADDRESS_PN0);
      gif_v00_setNameTableOffset(0);
      saitama00002_sendPatternNames();
      while (frames_left) HALT();
      frames_left = 3;
      gif_v00_setTargetAddress(MODE2_ADDRESS_PN1);
      gif_v00_setNameTableOffset(128);
      saitama00002_sendPatternNames();
  } else {
      gif_v00_setTargetAddress(MODE2_ADDRESS_PN1);
      gif_v00_setNameTableOffset(128);
      saitama00002_sendPatternNames();
      while (frames_left) HALT();
      frames_left = 4;
      gif_v00_setTargetAddress(MODE2_ADDRESS_PN0);
      gif_v00_setNameTableOffset(0);
      saitama00002_sendPatternNames();
  }
}
