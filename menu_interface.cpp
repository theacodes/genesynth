#include "menu_interface.h"
#include "Arduino.h"
#include "SdFat.h"
#include "buttons.h"
#include "fs_iterators.h"
#include "simple_iterator.h"
#include <U8g2lib.h>

#ifdef U8G2_HAVE_HW_SPI
#include <SPI.h>
#endif

#define MENU_PAGE_SIZE 6
#define MENU_FONT_HEIGHT 9
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

namespace thea {
namespace menu_interface {

class AbstractMenu {
public:
  virtual ~AbstractMenu(){};
  virtual void display();
  virtual void up(){};
  virtual void down(){};
  virtual void forward(){};
  virtual void reset(){};
};

void draw_menu_options(U8G2 *u8g2, const char *const options[], int len, int selected) {
  for (int i = 0; i < len; i++) {
    auto value = options[i];
    int y = i * MENU_FONT_HEIGHT;
    u8g2->setCursor(0, y);

    if (i == selected) {
      u8g2->setDrawColor(1);
      u8g2->drawBox(0, y, 128, MENU_FONT_HEIGHT);
      u8g2->setDrawColor(0);
    } else {
      u8g2->setDrawColor(1);
    }
    u8g2->printf(value);

    // reset draw color
    u8g2->setDrawColor(1);
  }
}

class StringOptionsMenu : public AbstractMenu {
public:
  StringOptionsMenu(U8G2 *u8g2, const char *const *options, int options_len)
      : u8g2(u8g2), options(options), options_len(options_len) {}

  virtual void display() {
    int page = selected / MENU_PAGE_SIZE;
    int page_start = page * MENU_PAGE_SIZE;

    draw_menu_options(u8g2, options + page_start, options_len - page_start, selected - page_start);
  }

  virtual void up() {
    selected--;
    if (selected < 0)
      selected = 0;
  }

  virtual void down() {
    selected++;
    if (selected >= options_len)
      selected = options_len - 1;
  }

  virtual void forward() {
    // TODO
  }

  virtual void reset() { selected = 0; }

protected:
  U8G2 *u8g2;
  int selected = 0;
  const char *const *options;
  int options_len;
};

class MenuController {
public:
  MenuController() {}

  void display() {
    if (current_menu() == nullptr)
      return;

    current_menu()->display();
  }

  void up() {
    if (current_menu() == nullptr)
      return;

    current_menu()->up();
  }

  void down() {
    if (current_menu() == nullptr)
      return;

    current_menu()->down();
  }

  void forward() {
    if (current_menu() == nullptr)
      return;

    current_menu()->forward();
  }

  void back() { pop(); }

  void advance(AbstractMenu *next) { stack[++stack_ptr] = next; }

  void pop() {
    if (stack_ptr != 0) {
      stack_ptr--;
    }
  }

  AbstractMenu *current_menu() { return stack[stack_ptr]; }

private:
  AbstractMenu *stack[5] = {};
  int stack_ptr = 0;
};

class FileSystemMenu : public AbstractMenu {
public:
  typedef void (*selection_callback)(SdFile);

  FileSystemMenu(U8G2 *u8g2, SdFile *root) : u8g2(u8g2), iterator(root) {
    clear_options();
    strncpy(current_options[0], "I haven't been", 14);
    strncpy(current_options[1], "reset()", 7);
  }

  virtual void display() {
    int page = selected / MENU_PAGE_SIZE;
    int page_start = page * MENU_PAGE_SIZE;

    const char *options[MENU_PAGE_SIZE];
    for (int i = 0; i < MENU_PAGE_SIZE; i++) {
      options[i] = current_options[i];
    }

    draw_menu_options(u8g2, options, MENU_PAGE_SIZE, selected - page_start);
  };

  virtual void up() {
    if (selected > 0)
      selected--;

    populate_options();
  };

  virtual void down() {
    selected++;
    if (selected >= max) {
      selected = max - 1;
    }
    populate_options();
  };

  virtual void forward() {
    if (callback != nullptr) {
      callback(selected_file());
    }
  };

  virtual void reset() {
    selected = 0;
    max = 255;
    populate_options();
  };

  void set_root(SdFile *root) {
    iterator.close();
    iterator = thea::fs::SdFatIterator(root);
  }

  void set_callback(selection_callback callback) { this->callback = callback; }

  SdFile selected_file() {
    iterator.rewind();
    iterator.fast_forward(selected);
    return iterator.item();
  }

private:
  U8G2 *u8g2;
  int selected = 0;
  int max = 255;
  char current_options[MENU_PAGE_SIZE][127] = {};
  thea::fs::SdFatIterator iterator;
  selection_callback callback = nullptr;

  void clear_options() {
    for (int i = 0; i < MENU_PAGE_SIZE; i++) {
      memset(current_options[i], 0, 127);
    }
  }

  void populate_options() {
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
};

const char *root_options[4] = {"Load patch", "Save patch", "Polyphony", "<3"};
const int root_options_len = 4;
const char *sub_options[10] = {"one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten"};
const int sub_options_len = 10;

class RootMenu : public StringOptionsMenu {
public:
  RootMenu(U8G2 *u8g2, MenuController &menu_ctrl)
      : StringOptionsMenu(u8g2, root_options, root_options_len), menu_ctrl(menu_ctrl),
        sub_menu(u8g2, sub_options, sub_options_len) {}

  virtual void forward() {
    if (selected == 1) {
      sub_menu.reset();
      menu_ctrl.advance(&sub_menu);
    }
  }

private:
  MenuController &menu_ctrl;
  StringOptionsMenu sub_menu;
};

void test_fs_iterators() {
  SdFatSdio sd;
  sd.begin();

  SdFile root;
  root.openRoot(sd.vol());

  char item_name[127];

  for (auto it = thea::fs::SdFatIterator(&root); !it.end(); it.next()) {
    it.item().getName(item_name, 127);
    Serial.printf("Item: %s, index: %i\n", item_name, it.index());

    SdFile subdir = it.item();

    Serial.printf("Listing subdir:\n");

    for (auto it = thea::fs::SdFatIterator(&subdir); !it.end(); it.next()) {
      it.item().getName(item_name, 127);
      Serial.printf("> Item: %s, index: %i\n", item_name, it.index());
    }
  }
}

U8G2_SH1106_128X64_NONAME_2_4W_HW_SPI u8g2(/* rotation=*/U8G2_R2, /* cs=*/10, /* dc=*/9, /* reset=*/8);

void button_release_callback(int button) {}

SdFatSdio sd;
SdFile root;
MenuController menu_ctrl;
RootMenu menu(&u8g2, menu_ctrl);
FileSystemMenu folder_menu(&u8g2, &root);
SdFile selected_folder;
FileSystemMenu file_menu(&u8g2, &selected_folder);

void folder_select_callback(SdFile selected) {
  char name[127];
  selected.getName(name, 127);
  Serial.printf("Selected: %s\n", name);
  selected_folder = selected;
  file_menu.set_root(&selected_folder);
  file_menu.reset();
  menu_ctrl.advance(&file_menu);
}

void button_press_callback(int button) {
  switch (button) {
  case 0:
    menu_ctrl.up();
    break;
  case 1:
    menu_ctrl.forward();
    break;
  case 2:
    menu_ctrl.down();
    break;
  case 3:
    menu_ctrl.back();
    break;
  default:
    break;
  }
}

void init() {
  /* Waits for the serial monitor to be opened. */
  while (!Serial.dtr()) {
    delay(10);
  }

  // test_fs_iterators();
  menu_ctrl.advance(&menu);

  sd.begin();
  root.openRoot(sd.vol());
  folder_menu.reset();
  folder_menu.set_callback(&folder_select_callback);
  menu_ctrl.advance(&folder_menu);

  u8g2.begin();
  u8g2.setPowerSave(0);
  u8g2.setFontPosTop();
  u8g2.setFont(u8g2_font_amstrad_cpc_extended_8f);

  thea::buttons::on_button_press(&button_press_callback);
  thea::buttons::on_button_release(&button_release_callback);
}

void loop(void) {
  u8g2.firstPage();
  do {
    menu_ctrl.display();
  } while (u8g2.nextPage());
}

} // namespace menu_interface
} // namespace thea
