#include <Arduino.h>

#include "note_stack.h"

namespace thea {

void NoteStack::push(int note, float pitch) {
  NoteStackItem new_item;
  new_item.note = note;
  new_item.pitch = pitch;

  // Case 1: Stack is not full. Add item to the end of the stack.
  if (len < NOTE_STACK_MAX_SIZE) {
    stack[len] = new_item;
    len++;
    return;
  }

  // Case 2: Stack is full, pop the first note off, shift the rest, and insert
  // the note at the end of the stack.
  for (int i = 0; i < NOTE_STACK_MAX_SIZE - 1; i++) {
    stack[i] = stack[i + 1];
  }
  stack[len - 1] = new_item;
  return;
}

void NoteStack::pop(int note) {
  // Find the note, then shift all remaining items down one.
  for (int i = 0; i < len; i++) {
    if (stack[i].note != note)
      continue;

    for (int j = i; j < len - 1; j++) {
      stack[j] = stack[j + 1];
    }

    stack[len - 1] = NoteStackItem();
    len--;
  }
}

const NoteStackItem NoteStack::top() { return stack[len - 1]; }

bool NoteStack::is_empty() { return len == 0; }

void NoteStack::clear() {
  for (int i = 0; i < NOTE_STACK_MAX_SIZE; i++) {
    stack[i] = NoteStackItem();
  }
  len = 0;
}

void NoteStack::print() {
  Serial.printf("Note stack: ");
  for (int i = 0; i < NOTE_STACK_MAX_SIZE; i++) {
    Serial.printf("%i, ", stack[i].note);
  }
  Serial.printf("\n");
}

void test_note_stack() {
  // NoteStack stack;

  // stack.print();
  // stack.push(40);
  // stack.push(41);
  // stack.push(42);
  // stack.push(43);
  // stack.print();

  // stack.push(44);
  // stack.push(45);
  // stack.print();

  // stack.push(46);
  // stack.print();

  // stack.pop(41);
  // stack.print();
  // stack.pop(42);
  // stack.print();
  // stack.pop(43);
  // stack.print();
  // stack.pop(44);
  // stack.print();
  // stack.pop(45);
  // stack.print();
  // stack.pop(46);
  // stack.print();

  // stack.clear();

  // stack.push(5);
  // stack.push(4);
  // stack.push(3);
  // stack.push(2);
  // stack.push(1);
  // stack.print();
  // stack.pop(3);
  // stack.print();
}

} // namespace thea
