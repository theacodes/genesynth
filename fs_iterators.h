#ifndef THEA_FS_ITERATOR_H
#define THEA_FS_ITERATOR_H

#include "SdFat.h"
#include "simple_iterator.h"

namespace thea {
namespace fs {

class SdFatIterator : thea::SimpleIterator<SdFile> {
public:
  SdFatIterator(SdFile *dir) : dir(dir), file(), _index() { hit_end = !file.openNext(dir, O_READ); }

  virtual bool next() {
    if (file.isOpen())
      file.close();
    bool success = file.openNext(dir, O_READ);
    hit_end = !success;
    _index++;
    return success;
  }

  unsigned int index() { return _index; }

  virtual SdFile item() { return file; }

  virtual bool end() { return hit_end; }

private:
  SdFile *dir;
  SdFile file;
  bool hit_end = false;
  unsigned int _index = 0;
};

} // namspace fs
} // namespace thea

#endif
