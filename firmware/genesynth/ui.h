#ifndef THEA_UI_H
#define THEA_UI_H

#include "src/theacommon/tasks.h"

namespace thea {
namespace ui {

void init(bool wait_for_serial);
void set_task_manager(thea::TaskManager *taskmgr);
void loop();

} // namespace ui
} // namespace thea

#endif
