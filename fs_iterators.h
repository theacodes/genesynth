#ifndef THEA_FS_ITERATOR_H
#define THEA_FS_ITERATOR_H

#include "SdFat.h"
#include "simple_iterator.h"

#include "Arduino.h"

namespace thea {
namespace fs {

class SdFatIterator : thea::SimpleIterator<SdFile &> {
public:
  SdFatIterator(SdFile *dir) : dir(dir), file(), _index() { rewind(); }

  virtual bool next() {
    if (hit_end)
      return false;

    if (file.isOpen())
      file.close();
    hit_end = !file.openNext(dir, O_READ);
    _index++;
    return !hit_end;
  }

  unsigned int index() { return _index; }

  virtual SdFile &item() { return file; }

  virtual bool end() { return hit_end; }

  void rewind() {
    if (file.isOpen())
      file.close();

    dir->rewind();
    hit_end = !file.openNext(dir, O_READ);
    _index = 0;
  }

  void fast_forward(int num) {
    for (int i = 0; i < num; i++) {
      if (!next())
        break;
    }
  }

  void close() {
    if (file.isOpen())
      file.close();
  }

private:
  SdFile *dir;
  SdFile file;
  bool hit_end = false;
  unsigned int _index = 0;
};

} // namspace fs
} // namespace thea

#endif
