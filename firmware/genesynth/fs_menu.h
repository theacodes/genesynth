#ifndef THEA_FS_MENU_H
#define THEA_FS_MENU_H

#include <SdFat.h>

#include "src/theacommon/abstract_menu_system.h"
#include "src/theacommon/fs_iterators.h"

namespace thea {
namespace fs_menu {

class FileSystemMenu : public thea::menu::AbstractMenu {
public:
  FileSystemMenu(U8G2 *u8g2);
  ~FileSystemMenu(){};

  void display();
  void up();
  void down();
  virtual void forward() = 0;
  void reset();
  void set_directory(SdFile *root);
  SdFile selected_file();

private:
  U8G2 *u8g2;
  int selected = 0;
  int max = 255;
  char current_options[MENU_PAGE_SIZE][127] = {};
  thea::fs::SdFatIterator iterator;

  void clear_options();
  void populate_options();
};

class FileSelectMenu : public FileSystemMenu {
public:
  FileSelectMenu(U8G2 *u8g2);
  ~FileSelectMenu(){};

  void set_top_directory(SdFile &folder);
  void push_directory(SdFile &folder);
  bool pop_directory();
  SdFile *current_directory();

  bool back();
  virtual void forward();

private:
  SdFile folder_stack[6];
  // Zeroth item is the root folder and is never popped, should be set by
  // `set_top_directory`.
  size_t folder_stack_ptr = 0;
};

} // namespace fs_menu
} // namespace thea

#endif
