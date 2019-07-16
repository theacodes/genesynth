#include "fs_menu.h"

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

namespace thea {
namespace fs_menu {

FileSystemMenu::FileSystemMenu(U8G2 *u8g2) : u8g2(u8g2), iterator(nullptr) {
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

void FileSystemMenu::reset() {
  selected = 0;
  max = 255;
  populate_options();
};

void FileSystemMenu::set_directory(SdFile *root) {
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

/* FileSelectMenu */

FileSelectMenu::FileSelectMenu(U8G2 *u8g2) : FileSystemMenu(u8g2){};

void FileSelectMenu::set_top_directory(SdFile &folder) {
  folder_stack[0] = SdFile(folder);
  set_directory(&folder_stack[0]);
  reset();
}

void FileSelectMenu::push_directory(SdFile &folder) {
  // TODO: Bounds check.
  folder_stack_ptr++;
  folder_stack[folder_stack_ptr] = SdFile(folder);
  set_directory(&folder_stack[folder_stack_ptr]);
  reset();
}

bool FileSelectMenu::pop_directory() {
  if (folder_stack_ptr >= 1) {
    folder_stack_ptr--;
    set_directory(&folder_stack[folder_stack_ptr]);
    reset();
    /* Don't pop the menu. */
    return false;
  }
  /* They hit back on the root folder, pop the menu. */
  return true;
}

SdFile *FileSelectMenu::current_directory() { return &folder_stack[folder_stack_ptr]; }

bool FileSelectMenu::back() { return pop_directory(); }

void FileSelectMenu::forward() {
  SdFile selected = selected_file();
  if (selected.isDir()) {
    push_directory(selected);
  }
}

} // namespace fs_menu
} // namespace thea
