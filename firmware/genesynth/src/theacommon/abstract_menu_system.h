#ifndef THEA_ABSTRACT_MENU_SYSTEM_H
#define THEA_ABSTRACT_MENU_SYSTEM_H

#include <U8g2lib.h>

namespace thea {
namespace menu {

#define MENU_PAGE_SIZE 6
#define MENU_FONT_HEIGHT 9

inline void draw_menu_options(U8G2 *u8g2, const char *const options[], int len, int selected) {
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

class AbstractMenu {
public:
  virtual ~AbstractMenu(){};
  virtual void display() = 0;
  virtual void up(){};
  virtual void down(){};
  virtual void forward(){};
  /* Return false to prevent the menu from being pop()ed */
  virtual bool back() { return true; };
  virtual void reset(){};
};

class MenuController {
public:
  MenuController() {}

  inline void display() {
    if (current_menu() == nullptr)
      return;

    current_menu()->display();
  }

  inline void up() {
    if (current_menu() == nullptr)
      return;

    current_menu()->up();
  }

  inline void down() {
    if (current_menu() == nullptr)
      return;

    current_menu()->down();
  }

  inline void forward() {
    if (current_menu() == nullptr)
      return;

    current_menu()->forward();
  }

  inline void back() {
    if (current_menu() == nullptr || current_menu()->back()) {
      pop();
    }
  }

  inline void advance(AbstractMenu *next) { stack[++stack_ptr] = next; }

  inline void set_root(AbstractMenu *root) { stack[0] = root; }

  inline void pop() {
    if (stack_ptr != 0) {
      stack_ptr--;
    }
  }

  inline void unwind() { stack_ptr = 0; }

  AbstractMenu *current_menu() { return stack[stack_ptr]; }

private:
  AbstractMenu *stack[5] = {};
  int stack_ptr = 0;
};

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

  virtual void forward() {}

  virtual void reset() { selected = 0; }

protected:
  U8G2 *u8g2;
  int selected = 0;
  const char *const *options;
  int options_len;
};

} // namespace menu
} // namespace thea

#endif
