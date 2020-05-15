#ifndef UNDO_QUEUE_H
#define UNDO_QUEUE_H
#include <vector>
#include <string>

enum class Action : char {
    Insert, Delete, Left, Right, Up, Down
};

struct Event {
    Action type;
    int text{};
};

class UndoQueue {
private:
    std::vector<Event> m_events;
    std::vector<Event>::iterator m_it;
public:
    UndoQueue();
    ~UndoQueue();
    /**Immediately pushes an event to the queue*/
    void push(Action event, char letter);
    Event undo();
    Event redo();
    bool empty() const { return m_events.empty(); }
    bool at_begin() const { return m_it == m_events.begin(); }
    bool at_end() const { return m_it >= std::prev(m_events.end()); }
    std::size_t size() const { return m_events.size(); }
};
#endif
