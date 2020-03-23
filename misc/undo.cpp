#include <algorithm>
#include "undo.h"
#include <iostream>

UndoQueue::UndoQueue()
{
    m_events.reserve(50);
}

UndoQueue::~UndoQueue()
{
    for(const auto &[type, text] : m_events) {
        std::cout << "Action Type: ";
        switch(type) {
        case Action::Insert:
            std::cout << "Insert\n";
            break;
        case Action::Delete:
            std::cout << "Delete\n";
            break;
        case Action::Left:
            std::cout << "Left\n";
            break;
        case Action::Right:
            std::cout << "Right\n";
            break;
        case Action::Up:
            std::cout << "Up\n";
            break;
        case Action::Down:
            std::cout << "Down\n";
            break;
        }
        std::cout << "Text: " << text << '\n' << std::endl;
    }
}

void UndoQueue::flush_insert_cache()
{
    if(!m_insert_cache.empty()) {
        // If needed, flush cached insert events
        m_events.push_back({Action::Insert, m_insert_cache});
        m_insert_cache.clear();
    }
}

void UndoQueue::flush_delete_cache()
{
    if(!m_delete_cache.empty()) {
        // If needed, flush cached backspace events
        m_events.push_back({Action::Delete, m_delete_cache});
        m_delete_cache.clear();
    }
}

/**Immediately pushes an event to the queue*/
void UndoQueue::push(Action event, char letter)
{
    switch(event) {
    case Action::Delete:
        flush_insert_cache();
        m_delete_cache += letter;
        if(m_delete_cache.size() >= CacheSize) {
            flush_delete_cache();
        }
        break;
    case Action::Insert:
        // Add/delete a chunk of text
        flush_delete_cache();
        m_insert_cache += letter;
        if(m_insert_cache.size() >= CacheSize) {
            flush_insert_cache();
        }
        break;
    case Action::Left:
    case Action::Right:
    case Action::Up:
    case Action::Down:
        // At least one of these 2 calls should do nothing
        flush_insert_cache();
        flush_delete_cache();
        m_events.push_back({event});
        break;
    }
}

Event UndoQueue::pop()
{
    const auto back{m_events.back()};
    m_events.pop_back();
    return back;
}

int main()
{
    UndoQueue q;
    q.push(Action::Insert, 'a');
    q.push(Action::Insert, 'a');
    q.push(Action::Insert, 'a');
    q.push(Action::Insert, 'a');
    q.push(Action::Insert, 'a');
    q.push(Action::Insert, 'a');
    q.push(Action::Insert, 'a');
    q.push(Action::Insert, 'a');
    q.push(Action::Insert, 'a');
    q.push(Action::Insert, 'a');

    q.push(Action::Insert, 'a');
    q.push(Action::Insert, 'a');

    q.push(Action::Delete, 'a');

    q.push(Action::Left);
    q.push(Action::Up);

    q.push(Action::Delete, 'a');
    q.push(Action::Delete, 'a');
    q.push(Action::Delete, 'a');
    q.push(Action::Delete, 'a');

    q.push(Action::Insert, 'b');
    q.push(Action::Right);

    return 0;
}
