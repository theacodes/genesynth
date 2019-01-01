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

const char *manu_menu_options[4] = {"Load patch", "Save patch", "Polyphony", "<3"};
const int manu_menu_options_len = 4;

class IdleMenu : public thea::menu::AbstractMenu {
public:
  IdleMenu(U8G2 *u8g2, thea::menu::MenuController &menu_ctrl) : u8g2(u8g2), menu_ctrl(menu_ctrl) {}

  virtual void display() {
    u8g2->setCursor(0, 0);
    u8g2->printf("Idle menu");
  }

  virtual void forward() {
    // TODO;
  }

  virtual void up() { forward(); }
  virtual void down() { forward(); }

private:
  U8G2 *u8g2;
  thea::menu::MenuController &menu_ctrl;
};

class MainMenu : public thea::menu::StringOptionsMenu {
public:
  MainMenu(U8G2 *u8g2, thea::menu::MenuController &menu_ctrl)
      : thea::menu::StringOptionsMenu(u8g2, manu_menu_options, manu_menu_options_len), menu_ctrl(menu_ctrl) {}

  virtual void forward() {
    if (selected == 1) {
    }
  }

private:
  thea::menu::MenuController &menu_ctrl;
};

U8G2_SH1106_128X64_NONAME_2_4W_HW_SPI u8g2(/* rotation=*/U8G2_R2, /* cs=*/10, /* dc=*/9, /* reset=*/8);
SdFatSdio sd;

thea::menu::MenuController menu_ctrl;
IdleMenu idle_menu(&u8g2, menu_ctrl);
MainMenu main_menu(&u8g2, menu_ctrl);

SdFile fs_root;
thea::fs_menu::FileSystemMenu folder_menu(&u8g2, &fs_root);
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
  /* Initialize display */
  u8g2.begin();
  u8g2.setPowerSave(0);
  u8g2.setFontPosTop();
  u8g2.setFont(u8g2_font_amstrad_cpc_extended_8f);

  /* Waits for the serial monitor to be opened. */
  u8g2.setCursor(0, 0);
  u8g2.printf("Waiting for serial");
  while (!Serial.dtr()) {
    delay(10);
  }

  menu_ctrl.set_root(&idle_menu);
  menu_ctrl.advance(&main_menu);

  sd.begin();
  fs_root.openRoot(sd.vol());
  folder_menu.reset();
  folder_menu.set_callback(&folder_select_callback);
  menu_ctrl.advance(&folder_menu);

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
