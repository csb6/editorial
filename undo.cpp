/**Work-in-progress code for keeping track of user actions so they can be
   undone/redone. Will also be useful for future macro system. Currently
   not compiled as part of main code.
*/

enum class Action : char {
    Insert, Delete, Left, Right, Up, Down
};

struct Event {
    std::size_t pos;
    Action type;
    std::size_t length;
    const char *text;
};

class UndoQueue {
private:
    constexpr static std::size_t QueueSize = 50;
    constexpr static std::size_t CacheSize = 2;
    std::vector<Event> m_events;
    std::string m_letter_cache;
    std::size_t m_cache_start = 0;
    void flush_cache()
    {
	if(m_letter_cache.empty())
	    return;

	push_back({m_cache_start, Action::Insert,
		   m_letter_cache.size(), m_letter_cache.c_str()});
	m_letter_cache.clear();
    }
public:
    UndoQueue() { m_events.reserve(QueueSize); }

    ~UndoQueue()
    {
	/*std::ofstream log("log.txt");
	for(const auto &event : m_events) {
	    log << "Pos: " << event.pos
		      << "\nAction Type: " << (short)event.type
		      << "\nLength: " << (short)event.length << '\n';
	    if(event.text != nullptr) {
	        log << "Text: \"" << std::string(event.text) << "\"";
	    }
	    log << '\n' << std::endl;
	    }*/
    }

    void erase(std::size_t pos)
    {
	push_back({pos, Action::Delete, 0, nullptr});
    }

    void insert(char letter, std::size_t pos)
    {
	m_letter_cache += letter;
        if(m_letter_cache.size() >= CacheSize) {
	    flush_cache();
	} else if(m_letter_cache.size() == 1) {
	    m_cache_start = pos;
	}
    }

    /**Immediately pushes an event to the queue*/
    void push_back(Event next)
    {
	if(m_events.size() >= QueueSize) {
	    m_events.erase(m_events.begin());
	}
	switch(next.type) {
	case Action::Delete:
	    flush_cache();
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
	    break;
	}   
    }
};
