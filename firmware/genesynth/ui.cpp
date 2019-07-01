#include <Arduino.h>
#include <SdFat.h>
#include <U8g2lib.h>

#ifdef U8G2_HAVE_HW_SPI
#include <SPI.h>
#endif

#include "ambient_ui.h"
#include "filesystem.h"
#include "fs_menu.h"
#include "hardware_constants.h"
#include "src/theacommon/abstract_menu_system.h"
#include "src/theacommon/buttons.h"
#include "src/theacommon/thea_easter_egg.h"
#include "synth.h"
#include "ui.h"

namespace thea {
namespace ui {

thea::TaskManager *taskmgr;
#define MAIN_MENU_OPTIONS_LEN 4
const char *main_menu_options[MAIN_MENU_OPTIONS_LEN] = {"Load patch", "Polyphony", "Stats", "<3"};
#define NOTE_MODE_MENU_OPTIONS_LEN 3
const char *note_mode_menu_options[NOTE_MODE_MENU_OPTIONS_LEN] = {"Poly", "Mono", "Unison"};

class IdleMenu : public thea::menu::AbstractMenu {
public:
  IdleMenu(U8G2 *u8g2, thea::menu::MenuController &menu_ctrl) : u8g2(u8g2), menu_ctrl(menu_ctrl) {}
  ~IdleMenu() {}

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
  ~StatsMenu() {}

  virtual void display() {
    if (screen % 2 == 0) {
      for (auto i = 0; i < 6; i++) {
        auto task = taskmgr->tasks[i];
        if (task == nullptr)
          continue;

        u8g2->setCursor(0, 18 * i);
        u8g2->printf("%s: %.0fuS\n", task->name, task->average_execution_time);
        u8g2->setCursor(0, (18 * i) + 9);
        u8g2->printf("%iuS %iuS\n", task->last_execution_time, task->max_execution_time);
      }
    } else {
      auto latency = thea::ym2612::get_latency();
      u8g2->setCursor(0, 0);
      u8g2->printf("YM2612 Latency:\n");
      u8g2->setCursor(0, 9);
      u8g2->printf("Avg: %.0fuS\n", latency.average);
      u8g2->setCursor(0, 9 * 2);
      u8g2->printf("Max: %iuS\n", latency.max);
      u8g2->setCursor(0, 9 * 3);
      u8g2->printf("Last: %iuS\n", latency.last);
      u8g2->setCursor(0, 9 * 4);
      u8g2->printf("Bytes: %i\n", latency.bytes_written);
      u8g2->setCursor(0, 9 * 5);
      u8g2->printf("Timed out?: %i\n", latency.hit_max_wait_cycles);
    }
  }

  virtual void up() { screen--; }
  virtual void down() { screen++; }

private:
  U8G2 *u8g2;
  int screen = 0;
};

class NoteModeMenu : public thea::menu::StringOptionsMenu {
public:
  NoteModeMenu(U8G2 *u8g2) : thea::menu::StringOptionsMenu(u8g2, note_mode_menu_options, NOTE_MODE_MENU_OPTIONS_LEN) {
    selected = thea::synth::get_note_mode();
  }
  ~NoteModeMenu() {}

  virtual void forward() { thea::synth::set_note_mode(thea::synth::NoteMode(selected)); }
};

class MainMenu : public thea::menu::StringOptionsMenu {
public:
  MainMenu(U8G2 *u8g2, thea::menu::MenuController &menu_ctrl)
      : thea::menu::StringOptionsMenu(u8g2, main_menu_options, MAIN_MENU_OPTIONS_LEN), menu_ctrl(menu_ctrl) {}
  ~MainMenu() {}

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

class EasterEggMenu : public thea::menu::AbstractMenu {
public:
  EasterEggMenu(U8G2 *u8g2) : u8g2(u8g2), dt(1000000) {}
  ~EasterEggMenu() {}

  virtual void display() { thea::show_thea(u8g2, dt); }

  virtual void forward() { dt += 100000; }
  virtual void up() { dt += 100000; }
  virtual void down() { dt -= 100000; }

private:
  U8G2 *u8g2;
  unsigned long dt;
};

U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI u8g2(/* rotation=*/U8G2_R2, /* cs=*/DISPLAY_CS, /* dc=*/DISPLAY_DC,
                                           /* reset=*/DISPLAY_RESET);

thea::menu::MenuController menu_ctrl;
IdleMenu idle_menu(&u8g2, menu_ctrl);
NoteModeMenu note_mode_menu(&u8g2);
MainMenu main_menu(&u8g2, menu_ctrl);
StatsMenu stats_menu(&u8g2);
EasterEggMenu easter_egg_menu(&u8g2);

SdFile fs_root;
thea::fs_menu::FileSystemMenu folder_menu(&u8g2, &fs_root);
SdFile selected_folder;
thea::fs_menu::FileSystemMenu file_menu(&u8g2, &selected_folder);

void folder_select_callback(SdFile selected) {
  char name[127];
  selected.getName(name, 127);
  Serial.printf("Selected: %s\n", name);

  // If the *same* folder was selected, we don't need to do this. Otherwise,
  // set the root and reset. This preserves where the user is in the menu if
  // they come back.
  if (selected_folder.dirIndex() != selected.dirIndex()) {
    selected_folder = selected;
    file_menu.set_root(&selected_folder);
    file_menu.reset();
  }
  menu_ctrl.advance(&file_menu);
}

#define DOUBLE_PRESS_TIME_MS 1000
unsigned long last_file_select_time = 0;

void file_select_callback(SdFile selected) {
  // If double-pressed, exit.
  if (last_file_select_time > millis() - DOUBLE_PRESS_TIME_MS) {
    menu_ctrl.unwind();
    return;
  }
  last_file_select_time = millis();

  char name[127];
  selected.getName(name, 127);
  Serial.printf("Selected: %s\n", name);

  thea::synth::load_patch(selected, &selected_folder);
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
  fs_root.openRoot(thea::filesystem::sd().vol());

  /* Wire up the menu hierarchy */
  idle_menu.set_main_menu(&main_menu);
  menu_ctrl.set_root(&idle_menu);
  folder_menu.reset();
  folder_menu.set_callback(&folder_select_callback);
  file_menu.set_callback(&file_select_callback);
  main_menu.submenus[0] = &folder_menu;
  main_menu.submenus[1] = &note_mode_menu;
  main_menu.submenus[2] = &stats_menu;
  main_menu.submenus[3] = &easter_egg_menu;

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
