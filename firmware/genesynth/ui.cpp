#include "ui.h"
#include "Arduino.h"
#include "SdFat.h"
#include "abstract_menu_system.h"
#include "ambient_ui.h"
#include "buttons.h"
#include "fs_menu.h"
#include "patch_loader.h"
#include "synth.h"
#include <U8g2lib.h>

#ifdef U8G2_HAVE_HW_SPI
#include <SPI.h>
#endif

namespace thea {
namespace ui {

#define DISPLAY_RATE 66666 // 1/15th of a second.
unsigned long last_display_time = micros();
const char *manu_menu_options[4] = {"Load patch", "Save patch", "Polyphony", "<3"};
const int manu_menu_options_len = 4;

class IdleMenu : public thea::menu::AbstractMenu {
public:
  IdleMenu(U8G2 *u8g2, thea::menu::MenuController &menu_ctrl) : u8g2(u8g2), menu_ctrl(menu_ctrl) {}

  virtual void display() {
    thea::ambient_ui::display(*u8g2, thea::synth::patch, thea::synth::last_write_option,
                              thea::synth::last_patch_modify_time);
  }

  virtual void forward() { menu_ctrl.advance(main_menu); }

  virtual bool back() {
    forward();
    return false;
  }
  virtual void up() { forward(); }
  virtual void down() { forward(); }

  void set_main_menu(AbstractMenu *main_menu) { this->main_menu = main_menu; }

private:
  U8G2 *u8g2;
  thea::menu::MenuController &menu_ctrl;
  thea::menu::AbstractMenu *main_menu = nullptr;
};

class MainMenu : public thea::menu::StringOptionsMenu {
public:
  MainMenu(U8G2 *u8g2, thea::menu::MenuController &menu_ctrl)
      : thea::menu::StringOptionsMenu(u8g2, manu_menu_options, manu_menu_options_len), menu_ctrl(menu_ctrl) {}

  virtual void forward() {
    if (selected == 0) {
      menu_ctrl.advance(folder_select_menu);
    }
  }

  void set_folder_select_menu(AbstractMenu *folder_select_menu) { this->folder_select_menu = folder_select_menu; }

private:
  thea::menu::MenuController &menu_ctrl;
  thea::menu::AbstractMenu *folder_select_menu = nullptr;
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

void file_select_callback(SdFile selected) {
  char name[127];
  selected.getName(name, 127);
  Serial.printf("Selected: %s\n", name);

  thea::patch_loader::load_from_sd_file(selected, &selected_folder, &thea::synth::patch);
  thea::synth::update_patch();

  menu_ctrl.unwind();
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

void wait_for_serial_loop(U8G2 &u8g2) {
  u8g2.firstPage();
  do {
    u8g2.setCursor(0, 0);
    u8g2.printf("Waiting for");
    u8g2.setCursor(0, 9);
    u8g2.printf("serial...");
  } while (u8g2.nextPage());

  while (!Serial.dtr()) {
    delay(10);
  }
}

void init(bool wait_for_serial) {
  /* Initialize display */
  u8g2.begin();
  u8g2.setPowerSave(0);
  u8g2.setFontPosTop();
  u8g2.setFont(u8g2_font_amstrad_cpc_extended_8f);

  /* Waits for the serial monitor to be opened. */
  if (wait_for_serial) {
    wait_for_serial_loop(u8g2);
  }

  /* Initialize filesystem access */
  sd.begin();
  fs_root.openRoot(sd.vol());

  /* Wire up the menu hierarchy */
  idle_menu.set_main_menu(&main_menu);
  menu_ctrl.set_root(&idle_menu);
  main_menu.set_folder_select_menu(&folder_menu);
  folder_menu.reset();
  folder_menu.set_callback(&folder_select_callback);
  file_menu.set_callback(&file_select_callback);

  /* Wire up hardware buttons to the menu system */
  thea::buttons::on_button_press(&button_press_callback);
  thea::buttons::on_button_release(&button_release_callback);
}

void loop(void) {
  auto now = micros();
  // Don't display more often than needed.
  if (now < last_display_time + DISPLAY_RATE) {
    return;
  }

  u8g2.firstPage();
  do {
    menu_ctrl.display();
  } while (u8g2.nextPage());

  last_display_time = micros();
}

} // namespace ui
} // namespace thea
