#ifndef THEA_SIMPLE_ITERATOR_H
#define THEA_SIMPLE_ITERATOR_H

namespace thea {

template <class T> class SimpleIterator {
public:
  virtual ~SimpleIterator() = default;
  virtual bool next() = 0;
  virtual T item() = 0;
  virtual bool end() = 0;
};

} // namespace thea

#endif
