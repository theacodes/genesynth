#ifndef THEA_DISPLAY_H
#define THEA_DISPLAY_H

#include "ym2612.h"

namespace thea {
namespace display {

enum Screen {
  THEA,
  DEBUG,
  NOTES,
  OPEDIT,
  ENVEDIT,
};

void init();
void loop();
void show(Screen screen, unsigned long duration);

} // namespace thea
} // namespace display

#endif
