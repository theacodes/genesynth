#include "fs_menu.h"

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

namespace thea {
namespace fs_menu {

FileSystemMenu::FileSystemMenu(U8G2 *u8g2, SdFile *root) : u8g2(u8g2), iterator(root) {
  clear_options();
  strncpy(current_options[0], "I haven't been", 14);
  strncpy(current_options[1], "reset()", 7);
}

void FileSystemMenu::display() {
  int page = selected / MENU_PAGE_SIZE;
  int page_start = page * MENU_PAGE_SIZE;

  const char *options[MENU_PAGE_SIZE];
  for (int i = 0; i < MENU_PAGE_SIZE; i++) {
    options[i] = current_options[i];
  }

  thea::menu::draw_menu_options(u8g2, options, MENU_PAGE_SIZE, selected - page_start);
};

void FileSystemMenu::up() {
  if (selected > 0)
    selected--;

  populate_options();
};

void FileSystemMenu::down() {
  selected++;
  if (selected >= max) {
    selected = max - 1;
  }
  populate_options();
};

void FileSystemMenu::forward() {
  if (callback != nullptr) {
    callback(selected_file());
  }
};

void FileSystemMenu::reset() {
  selected = 0;
  max = 255;
  populate_options();
};

void FileSystemMenu::set_root(SdFile *root) {
  iterator.close();
  iterator = thea::fs::SdFatIterator(root);
}

SdFile FileSystemMenu::selected_file() {
  iterator.rewind();
  iterator.fast_forward(selected);
  return iterator.item();
}

void FileSystemMenu::clear_options() {
  for (int i = 0; i < MENU_PAGE_SIZE; i++) {
    memset(current_options[i], 0, 127);
  }
}

void FileSystemMenu::populate_options() {
  iterator.rewind();
  int page = selected / MENU_PAGE_SIZE;
  iterator.fast_forward(page * MENU_PAGE_SIZE);
  for (int i = 0; i < MENU_PAGE_SIZE; i++) {
    if (iterator.end()) {
      memset(current_options[i], 0, 127);
      max = MIN(max, page * MENU_PAGE_SIZE + i);
    }
    iterator.item().getName(current_options[i], 127);
    iterator.next();
  }
}

} // namespace fs_menu
} // namespace thea
