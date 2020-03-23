#ifndef UNDO_QUEUE_H
#define UNDO_QUEUE_H
#include <vector>
#include <string>

enum class Action : char {
    Insert, Delete, Left, Right, Up, Down
};

struct Event {
    Action type;
    std::string text{};
};

class UndoQueue {
private:
    constexpr static std::size_t CacheSize = 10; //in characters
    std::vector<Event> m_events;
    std::string m_insert_cache;
    std::string m_delete_cache;
    void flush_insert_cache();
    void flush_delete_cache();
public:
    UndoQueue();
    ~UndoQueue();
    /**Immediately pushes an event to the queue*/
    void push(Action event, char letter = 0);
    Event pop();
    std::size_t size() const { return m_events.size(); }
};
#endif
