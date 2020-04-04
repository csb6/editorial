#include <algorithm>
#include "undo.h"
//#include <iostream>

UndoQueue::UndoQueue()
{
    m_events.reserve(50);
    m_it = m_events.begin();
}

UndoQueue::~UndoQueue()
{
    /*for(const auto &[type, text] : m_events) {
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
        }*/
}

/**Immediately pushes an event to the queue*/
void UndoQueue::push(Action event, char letter)
{
    switch(event) {
    case Action::Delete:
    case Action::Insert:
        if(m_it != m_events.end())
            m_events.erase(m_it, m_events.end());
        m_events.push_back({event, letter});
        m_it = m_events.end();
        break;
    case Action::Left:
    case Action::Right:
    case Action::Up:
    case Action::Down:
        if(m_it != m_events.end())
            m_events.erase(m_it, m_events.end());
        m_events.push_back({event});
        m_it = m_events.end();
        break;
    }
}

Event UndoQueue::undo()
{
    if(m_it == m_events.end())
        --m_it;
    return *(m_it--);
}

Event UndoQueue::redo()
{
    return *(++m_it);
}
