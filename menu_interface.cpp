#include "menu_interface.h"
#include "Arduino.h"
#include "SdFat.h"
#include "fs_iterators.h"
#include "simple_iterator.h"

namespace thea {
namespace menu_interface {

// class TestMenuItemIterator : MenuItemIterator {
// public:
//     TestMenuItemIterator() : value(0) { }

//     virtual bool next() {
//         value++;
//         return true;
//     }

//     virtual const char* item_name() {
//         return options[value];
//     }

//     virtual unsigned int item_value() {
//         return value;
//     }

//     virtual bool end() {
//         return value == 5;
//     }

// private:
//     unsigned int value = 0;
//     static char* options[];
// };

// char* TestMenuItemIterator::options[5] = {
//     "one",
//     "two",
//     "three",
//     "four",
//     "five"
// };

void init() {
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

} // namespace menu_inteface
} // namespace thea
