#ifndef THEA_BUTTONS_H
#define THEA_BUTTONS_H

namespace thea {
namespace buttons {

typedef void (*callback)(int);

void on_button_press(callback);
void on_button_release(callback);
bool is_pressed(int button);
void init();
void loop();

} // namespace buttons
} // namespace thea

#endif
