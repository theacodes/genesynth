#include <Arduino.h>
#include <SdFat.h>
#include <U8g2lib.h>

#ifdef U8G2_HAVE_HW_SPI
#include <SPI.h>
#endif

#include "ambient_ui.h"
#include "fs_menu.h"
#include "hardware_constants.h"
#include "src/theacommon/abstract_menu_system.h"
#include "src/theacommon/buttons.h"
#include "synth.h"
#include "ui.h"

namespace thea {
namespace ui {

thea::TaskManager *taskmgr;
#define MAIN_MENU_OPTIONS_LEN 5
const char *main_menu_options[MAIN_MENU_OPTIONS_LEN] = {"Load patch", "Save patch", "Polyphony", "Stats", "<3"};

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

class StatsMenu : public thea::menu::AbstractMenu {
public:
  StatsMenu(U8G2 *u8g2) : u8g2(u8g2) {}

  virtual void display() {
    for (auto i = 0; i < 6; i++) {
      auto task = taskmgr->tasks[i];
      if (task == nullptr)
        continue;

      u8g2->setCursor(0, 18 * i);
      u8g2->printf("%s: %.0fms\n", task->name, task->average_execution_time);
      u8g2->setCursor(0, (18 * i) + 9);
      u8g2->printf("%ims %ims\n", task->last_execution_time, task->max_execution_time);
    }
  }

private:
  U8G2 *u8g2;
};

class MainMenu : public thea::menu::StringOptionsMenu {
public:
  MainMenu(U8G2 *u8g2, thea::menu::MenuController &menu_ctrl)
      : thea::menu::StringOptionsMenu(u8g2, main_menu_options, MAIN_MENU_OPTIONS_LEN), menu_ctrl(menu_ctrl) {}

  virtual void forward() {
    auto submenu = submenus[selected];

    if (submenu == nullptr)
      return;

    menu_ctrl.advance(submenu);
  }

  thea::menu::AbstractMenu *submenus[MAIN_MENU_OPTIONS_LEN];

private:
  thea::menu::MenuController &menu_ctrl;
};

U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI u8g2(/* rotation=*/U8G2_R2, /* cs=*/DISPLAY_CS, /* dc=*/DISPLAY_DC,
                                           /* reset=*/DISPLAY_RESET);
SdFatSdio sd;

thea::menu::MenuController menu_ctrl;
IdleMenu idle_menu(&u8g2, menu_ctrl);
MainMenu main_menu(&u8g2, menu_ctrl);
StatsMenu stats_menu(&u8g2);

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

  thea::synth::load_patch(selected, &selected_folder);

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

void set_task_manager(thea::TaskManager *taskmgr) { thea::ui::taskmgr = taskmgr; }

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
  folder_menu.reset();
  folder_menu.set_callback(&folder_select_callback);
  file_menu.set_callback(&file_select_callback);
  main_menu.submenus[0] = &folder_menu;
  main_menu.submenus[3] = &stats_menu;

  /* Wire up hardware buttons to the menu system */
  thea::buttons::on_button_press(&button_press_callback);
  thea::buttons::on_button_release(&button_release_callback);
}

void loop(void) {
  u8g2.clearBuffer();
  menu_ctrl.display();
  u8g2.sendBuffer();
}

} // namespace ui
} // namespace thea
