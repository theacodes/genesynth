#ifndef THEA_NOTE_STACK_H
#define THEA_NOTE_STACK_H

#ifndef NOTE_STACK_MAX_SIZE
#define NOTE_STACK_MAX_SIZE 10
#endif

namespace thea {

struct NoteStackItem {
    int note;
    float pitch;
};

class NoteStack {
public:
    NoteStack() {}
    void push(int note, float pitch);
    void pop(int note);
    const NoteStackItem top();
    bool is_empty();
    void clear();
    void print();

private:
    int len = 0;
    NoteStackItem stack[NOTE_STACK_MAX_SIZE];
};

void test_note_stack();

} // namespace thea

#endif
