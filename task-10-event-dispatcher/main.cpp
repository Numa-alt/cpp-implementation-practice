#include <iostream>
#include <vector>
#include <functional>
#include <utility>
#include <map>
#include <algorithm>

enum class Event
{
    EnemyDefeated,
    PlayerDamaged,
};

struct DeleteEvent
{
    DeleteEvent(Event event, unsigned int id) : mEvent(event), mId(id)
    {
    }
    Event mEvent;
    unsigned int mId;
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

    bool mIsPublishExecuting = false;
    std::vector<DeleteEvent> mPendingDeletes;

public:
    unsigned int Subscribe(Event event, std::function<void()> callback)
    {
        mNextId++;
        mCallbacks[event].emplace_back(mNextId, std::move(callback));
        return mNextId;
    }
    bool Unsubscribe(Event event, unsigned int id)
    {
        std::cout << "do unsubscribe id " << id << std::endl;

        auto itEv = mCallbacks.find(event);

        if (itEv == mCallbacks.end())
        {
            std::cout << "not found event" << std::endl;
            return false;
        }

        auto &callbacks = itEv->second;

        auto it = std::find_if(
            callbacks.begin(),
            callbacks.end(),
            [id](const Callback &c)
            {
                return id == c.mId;
            });

        if (it != callbacks.end())
        {
            if (mIsPublishExecuting)
            {
                auto itDe = std::find_if(mPendingDeletes.begin(), mPendingDeletes.end(), [event, id](const DeleteEvent &e)
                                         { return (event == e.mEvent) && (id == e.mId); });
                if (itDe == mPendingDeletes.end())
                {
                    std::cout << "pending delete event id " << id << std::endl;
                    mPendingDeletes.emplace_back(event, id);
                    return true;
                }
                return false;
            }
            callbacks.erase(it);
            return true;
        }

        std::cout << "not found id " << std::endl;

        return false;
    }
    void Publish(Event event)
    {
        std::cout << "execute publish" << std::endl;

        auto it = mCallbacks.find(event);
        if (it == mCallbacks.end())
        {
            return;
        }

        mIsPublishExecuting = true;

        for (const auto &cb : it->second)
        {
            cb.mCallback();
        }
        mIsPublishExecuting = false;

        for (auto e : mPendingDeletes)
        {
            Unsubscribe(e.mEvent, e.mId);
        }
        mPendingDeletes.clear();
    }
};

int main()
{
    std::cout << "event dispatcher start" << std::endl;

    int score = 0;

    EventDispatcher eventDispatcher;

    unsigned int id0 = eventDispatcher.Subscribe(Event::PlayerDamaged, [&score]()
                                                 { score+=100; std::cout << "score + 100" << std::endl; });
    unsigned int id1 = eventDispatcher.Subscribe(Event::EnemyDefeated, [&eventDispatcher, id0]()
                                                 { std::cout << "enemy defeated" << std::endl; eventDispatcher.Unsubscribe(Event::PlayerDamaged,id0); });

    std::cout << "id0 = " << id0 << " id1 = " << id1 << std::endl;

    // bool f = eventDispatcher.Unsubscribe(Event::EnemyDefeated, id1);
    // if (f)
    // {
    //     std::cout << "success unsubscribe Event::EnemyDefeated id" << id1 << std::endl;
    // }
    // bool f2 = eventDispatcher.Unsubscribe(Event::EnemyDefeated, id1);
    // if (f2)
    // {
    //     std::cout << "success unsubscribe Event::EnemyDefeated id " << id1 << std::endl;
    // }
    // else
    // {
    //     std::cout << "failed unsubscribe Event::EnemyDefeated id " << id1 << std::endl;
    // }

    // bool f3 = eventDispatcher.Unsubscribe(Event::PlayerDamaged, id1);
    // if (f3)
    // {
    //     std::cout << "success unsubscribe Event::PlayerDamaged id " << id1 << std::endl;
    // }
    // else
    // {
    //     std::cout << "failed unsubscribe Event::PlayerDamaged id " << id1 << std::endl;
    // }

    std::cout << "event publish" << std::endl;
    eventDispatcher.Publish(Event::EnemyDefeated);

    bool f3 = eventDispatcher.Unsubscribe(Event::PlayerDamaged, id1);
    if (f3)
    {
        std::cout << "success unsubscribe Event::PlayerDamaged id " << id1 << std::endl;
    }
    else
    {
        std::cout << "failed unsubscribe Event::PlayerDamaged id " << id1 << std::endl;
    }

    eventDispatcher.Publish(Event::PlayerDamaged);

    return 0;
}

/*


*/