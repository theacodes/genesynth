#include "menu_ui.h"
#include "Arduino.h"
#include "SdFat.h"
#include "abstract_menu_system.h"
#include "buttons.h"
#include "fs_menu.h"
#include <U8g2lib.h>

#ifdef U8G2_HAVE_HW_SPI
#include <SPI.h>
#endif

namespace thea {
namespace menu_ui {

const char *root_options[4] = {"Load patch", "Save patch", "Polyphony", "<3"};
const int root_options_len = 4;
const char *sub_options[10] = {"one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten"};
const int sub_options_len = 10;

class RootMenu : public thea::menu::StringOptionsMenu {
public:
  RootMenu(U8G2 *u8g2, thea::menu::MenuController &menu_ctrl)
      : thea::menu::StringOptionsMenu(u8g2, root_options, root_options_len), menu_ctrl(menu_ctrl),
        sub_menu(u8g2, sub_options, sub_options_len) {}

  virtual void forward() {
    if (selected == 1) {
      sub_menu.reset();
      menu_ctrl.advance(&sub_menu);
    }
  }

private:
  thea::menu::MenuController &menu_ctrl;
  thea::menu::StringOptionsMenu sub_menu;
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

SdFatSdio sd;
SdFile root;
thea::menu::MenuController menu_ctrl;
RootMenu menu(&u8g2, menu_ctrl);
thea::fs_menu::FileSystemMenu folder_menu(&u8g2, &root);
SdFile selected_folder;
thea::fs_menu::FileSystemMenu file_menu(&u8g2, &selected_folder);

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

void button_release_callback(int button) {}

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

} // namespace menu_ui
} // namespace thea
