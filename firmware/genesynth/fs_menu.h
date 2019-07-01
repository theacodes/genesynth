#ifndef THEA_FS_MENU_H
#define THEA_FS_MENU_H

#include <SdFat.h>

#include "src/theacommon/abstract_menu_system.h"
#include "src/theacommon/fs_iterators.h"

namespace thea {
namespace fs_menu {

class FileSystemMenu : public thea::menu::AbstractMenu {
public:
  typedef void (*selection_callback)(SdFile);
  FileSystemMenu(U8G2 *u8g2, SdFile *root);
  ~FileSystemMenu(){};

  void display();
  void up();
  void down();
  void forward();
  void reset();
  void set_root(SdFile *root);
  SdFile selected_file();
  inline void set_callback(selection_callback callback) { this->callback = callback; }

private:
  U8G2 *u8g2;
  int selected = 0;
  int max = 255;
  char current_options[MENU_PAGE_SIZE][127] = {};
  thea::fs::SdFatIterator iterator;
  selection_callback callback = nullptr;

  void clear_options();
  void populate_options();
};
} // namespace fs_menu
} // namespace thea

#endif
