#include <iostream>
#include <vector>
#include <functional>
#include <utility>
#include <map>

enum class Event
{
    EnemyDefeated,
    PlayerDamaged,
};

struct Callback
{
    Callback(unsigned int id, std::function<void()> callback) : mId(id), mCallback(std::move(callback))
    {
    }
    std::function<void()> mCallback;
    unsigned int mId;
};
class EventDispatcher
{
private:
    // std::vector<std::function<void()>> mCallbacks;
    std::map<Event, std::vector<Callback>> mCallbacks;

    unsigned int mNextId = 0;

public:
    unsigned int Subscribe(Event event, std::function<void()> callback)
    {
        mNextId++;
        mCallbacks[event].emplace_back(mNextId, callback);
        return mNextId;
    }
    void Publish(Event event)
    {
        auto it = mCallbacks.find(event);
        if (it == mCallbacks.end())
        {
            return;
        }
        for (const auto &cb : it->second)
        {
            cb.mCallback();
        }
    }
};

int main()
{
    std::cout << "event dispatcher" << std::endl;

    int score = 0;

    EventDispatcher eventDispatcher;

    unsigned int id0 = eventDispatcher.Subscribe(Event::PlayerDamaged, [&score]()
                                                 { score+=100; std::cout << "score + 100" << std::endl; });
    unsigned int id1 = eventDispatcher.Subscribe(Event::EnemyDefeated, []()
                                                 { std::cout << "enemy defeated" << std::endl; });

    std::cout << "id0 = " << id0 << " id1 = " << id1 << std::endl;
    std::cout << "event publish" << std::endl;
    eventDispatcher.Publish(Event::EnemyDefeated);

    return 0;
}

/*


*/