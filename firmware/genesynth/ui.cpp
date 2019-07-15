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

/* External global state */

thea::TaskManager *taskmgr;
void set_task_manager(thea::TaskManager *taskmgr) { thea::ui::taskmgr = taskmgr; }

/* Helpers */

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

/* Menu classes */

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

const char *note_mode_menu_options[] = {"Poly", "Mono", "Unison"};
const size_t note_mode_menu_options_len = sizeof(note_mode_menu_options) / sizeof(char *);

class NoteModeMenu : public thea::menu::StringOptionsMenu {
public:
  NoteModeMenu(U8G2 *u8g2) : thea::menu::StringOptionsMenu(u8g2, note_mode_menu_options, note_mode_menu_options_len) {
    selected = thea::synth::get_note_mode();
  }
  ~NoteModeMenu() {}

  virtual void forward() { thea::synth::set_note_mode(thea::synth::NoteMode(selected)); }
};

const char *main_menu_options[] = {"Load patch", "Polyphony", "Stats", "<3"};
const size_t main_menu_options_len = sizeof(main_menu_options) / sizeof(char *);

class MainMenu : public thea::menu::StringOptionsMenu {
public:
  MainMenu(U8G2 *u8g2, thea::menu::MenuController &menu_ctrl)
      : thea::menu::StringOptionsMenu(u8g2, main_menu_options, main_menu_options_len), menu_ctrl(menu_ctrl) {}
  ~MainMenu() {}

  virtual void forward() {
    auto submenu = submenus[selected];

    if (submenu == nullptr)
      return;

    menu_ctrl.advance(submenu);
  }

  thea::menu::AbstractMenu *submenus[main_menu_options_len];

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

class PatchLoadMenu : public thea::fs_menu::FileSystemMenu {
public:
  PatchLoadMenu(U8G2 *u8g2, thea::menu::MenuController &menu_ctrl)
      : FileSystemMenu(u8g2, nullptr), menu_ctrl(menu_ctrl){};
  ~PatchLoadMenu(){};

  void set_top_directory(SdFile &file) {
    folder_stack[0] = SdFile(file);
    set_root(&folder_stack[0]);
    reset();
  }

  bool back() {
    if (folder_stack_ptr >= 1) {
      folder_stack_ptr--;
      set_root(&folder_stack[folder_stack_ptr]);
      reset();
      printf("Navigating up, depth: %i\n", folder_stack_ptr);
      /* Don't pop the menu. */
      return false;
    }
    /* They hit back on the root folder, pop the menu. */
    return true;
  };

  void forward() {
    SdFile selected = selected_file();
    char name[127];
    selected.getName(name, 127);
    Serial.printf("Selected: %s, depth: %i\n", name, folder_stack_ptr);

    // Is this a tfi patch file? If so, load it and leave the menu.
    char *dot = strrchr(name, '.');
    if (dot && !strcmp(dot, ".tfi")) {
      thea::synth::load_patch(selected, &folder_stack[folder_stack_ptr]);

      // If double-pressed, exit.
      if (last_file_select_time > millis() - double_press_time_ms) {
        menu_ctrl.unwind();
        return;
      }

      last_file_select_time = millis();
      return;
    }

    // Otherwise if this is a directory open it and push the stack.
    if (!selected.isDir()) {
      return;
    }

    // TODO: Bounds check.
    folder_stack_ptr++;
    folder_stack[folder_stack_ptr] = SdFile(selected);
    set_root(&folder_stack[folder_stack_ptr]);
    reset();
  };

private:
  thea::menu::MenuController &menu_ctrl;
  SdFile folder_stack[5];
  // Zeroth item is the root folder and is never popped, should be set by
  // `set_top_directory`.
  size_t folder_stack_ptr = 0;
  const unsigned double_press_time_ms = 1000;
  unsigned long last_file_select_time = 0;
};

/* Local global state and callbacks */

U8G2_INITIALIZATION;

thea::menu::MenuController menu_ctrl;
IdleMenu idle_menu(&u8g2, menu_ctrl);
NoteModeMenu note_mode_menu(&u8g2);
MainMenu main_menu(&u8g2, menu_ctrl);
PatchLoadMenu patch_load_menu(&u8g2, menu_ctrl);
StatsMenu stats_menu(&u8g2);
EasterEggMenu easter_egg_menu(&u8g2);

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
  SdFile fs_root;
  fs_root.openRoot(thea::filesystem::sd().vol());
  patch_load_menu.set_top_directory(fs_root);

  /* Wire up the menu hierarchy */
  idle_menu.set_main_menu(&main_menu);
  menu_ctrl.set_root(&idle_menu);
  main_menu.submenus[0] = &patch_load_menu;
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
