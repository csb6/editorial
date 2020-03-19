#include <algorithm>
#include "undo.h"

UndoQueue::UndoQueue()
{
    m_events.reserve(50);
}

UndoQueue::~UndoQueue()
{
    //std::ofstream log("log.txt");
    /*for(int i = 0; i < m_insert_pos; ++i) {
        auto &event = m_events[i];
        std::cout << "Pos: " << event.pos
                  << "\nAction Type: " << (short)event.type
                  << "\nText: " << event.text << '\n' << std::endl;
                  }*/
}

void UndoQueue::flush_cache()
{
    if(m_letter_cache.empty())
        return;

    push_back({m_cache_start_x, m_cache_start_y, Action::Insert, m_letter_cache});
    m_letter_cache.clear();
}

void UndoQueue::erase(char letter, int col, int row)
{
    // TODO: Add cache so that can combine multiple deleted letters into 1 event
    std::string text;
    text += letter;
    push_back({col, row, Action::Delete, text});
}

void UndoQueue::insert(char letter, int col, int row)
{
    m_letter_cache += letter;
    if(m_letter_cache.size() >= CacheSize) {
        flush_cache();
    } else if(m_letter_cache.size() == 1) {
        m_cache_start_x = col;
        m_cache_start_y = row;
    }
}

/**Immediately pushes an event to the queue*/
void UndoQueue::push_back(Event next)
{
    switch(next.type) {
    case Action::Delete:
        flush_cache();
        m_events.push_back(next);
        break;
    case Action::Insert:
        // Add/delete a chunk of text
        m_events.push_back(next);
        break;
    case Action::Left:
    case Action::Right:
    case Action::Up:
    case Action::Down:
        // Done with adding chars to current text block
        flush_cache();
        m_events.push_back(next);
        break;
    }
}

Event UndoQueue::pop_back()
{
    const auto back{m_events.back()};
    m_events.pop_back();
    return back;
}
