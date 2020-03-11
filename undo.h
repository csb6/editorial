#ifndef UNDO_QUEUE_H
#define UNDO_QUEUE_H
#include <vector>
#include <string>

enum class Action : char {
    Insert, Delete, Left, Right, Up, Down
};

struct Event {
    int col;
    int row;
    Action type;
    std::string text = "";
};

class UndoQueue {
private:
    constexpr static std::size_t CacheSize = 1;
    std::vector<Event> m_events;
    std::string m_letter_cache;
    int m_cache_start_x = 0;
    int m_cache_start_y = 0;
public:
    UndoQueue();
    ~UndoQueue();
    void flush_cache();
    void erase(int col, int row);
    void insert(char letter, int col, int row);
    /**Immediately pushes an event to the queue*/
    void push_back(Event next);
    Event pop_back();
    std::size_t size() const { return m_events.size(); }
};
#endif
