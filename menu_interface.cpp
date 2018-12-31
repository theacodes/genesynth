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

// class TestMenuItemIterator : MenuItemIterator {
// public:
//     TestMenuItemIterator() : value(0) { }

//     virtual bool next() {
//         value++;
//         return true;
//     }

//     virtual const char* item_name() {
//         return options[value];
//     }

//     virtual unsigned int item_value() {
//         return value;
//     }

//     virtual bool end() {
//         return value == 5;
//     }

// private:
//     unsigned int value = 0;
//     static char* options[];
// };

char *options[12] = {"one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten", "eleven", "twelve"};
int options_len = 12;

class MenuController {
public:
  MenuController(U8G2 *u8g2) : u8g2(u8g2) {}

  void display() {
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

  void up() {
    selected--;
    if (selected < 0)
      selected = 0;
  }

  void down() {
    selected++;
    if (selected >= options_len)
      selected = options_len - 1;
  }

  void back() {
    // TODO
  }

  void forward() {
    // TODO
  }

private:
  int selected = 0;
  U8G2 *u8g2;

  static const int page_size = 6;
  static const int font_height = 9;
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
auto menu_ctrl = MenuController(&u8g2);

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
