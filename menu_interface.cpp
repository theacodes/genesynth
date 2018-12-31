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

class StringOptionsMenu : public AbstractMenu {
public:
  StringOptionsMenu(U8G2 *u8g2, char **options, int options_len)
      : u8g2(u8g2), options(options), options_len(options_len) {}

  virtual void display() {
    /* Draw the options */
    int page = selected / page_size;
    int page_start = page * page_size;

    for (int i = 0; i < page_size; i++) {
      auto index = page_start + i;

      if (index >= options_len)
        break;

      auto value = options[index];

      int y = i * font_height;
      u8g2->setCursor(0, y);

      if (index == selected) {
        u8g2->setDrawColor(1);
        u8g2->drawBox(0, y, 128, font_height);
        u8g2->setDrawColor(0);
      } else {
        u8g2->setDrawColor(1);
      }
      u8g2->printf(value);

      // reset draw color
      u8g2->setDrawColor(1);
    }
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
  char **options;
  int options_len;

  static const int page_size = 6;
  static const int font_height = 9;
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

  void advance(AbstractMenu *next) {
    stack[++stack_ptr] = next;
    // Serial.printf("Advanced, stack %i", stack_ptr);
  }

  void pop() {
    if (stack_ptr != 0) {
      stack_ptr--;
    }

    Serial.printf("Popped, stack %i", stack_ptr);
  }

  AbstractMenu *current_menu() { return stack[stack_ptr]; }

private:
  AbstractMenu *stack[5] = {};
  int stack_ptr = 0;
};

char *root_options[10] = {"Nothing", "SubMenu", "More Nothing"};
int root_options_len = 10;
char *sub_options[10] = {"one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten"};
int sub_options_len = 10;

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

auto menu_ctrl = MenuController();
auto menu = RootMenu(&u8g2, menu_ctrl);

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

void button_release_callback(int button) {}

void init() {
  // test_fs_iterators();
  menu_ctrl.advance(&menu);

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
