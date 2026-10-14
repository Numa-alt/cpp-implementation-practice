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
    ItemCreated,
};

struct DeleteEvent
{
    DeleteEvent(Event event, unsigned int id) : mEvent(event), mId(id)
    {
    }
    Event mEvent;
    unsigned int mId;
};

struct SubscribeEvent
{
    SubscribeEvent(Event event, std::function<void()> callback, unsigned int id) : mEvent(event), mCallback(std::move(callback)), mId(id)
    {
    }
    Event mEvent;
    std::function<void()> mCallback;
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

class PublishGuard
{
public:
    PublishGuard(unsigned int &depth) : mDepth(depth)
    {
        ++mDepth;
    }
    ~PublishGuard()
    {
        --mDepth;
    }

private:
    unsigned int &mDepth;
};

class EventDispatcher
{
private:
    std::map<Event, std::vector<Callback>> mCallbacks;

    unsigned int mNextId = 0;

    unsigned int mPublishDepth = 0;

    std::vector<DeleteEvent> mPendingDeletes;
    std::vector<SubscribeEvent> mPendingSubscribe;

    void SubscribeInternal(Event event, unsigned int id, std::function<void()> callback)
    {
        mCallbacks[event].emplace_back(id, std::move(callback));
    }

    void FlushPendingOperations()
    {
        for (auto &e : mPendingDeletes)
        {
            Unsubscribe(e.mEvent, e.mId);
        }
        mPendingDeletes.clear();

        for (auto &e : mPendingSubscribe)
        {
            SubscribeInternal(e.mEvent, e.mId, std::move(e.mCallback));
        }

        mPendingSubscribe.clear();
    }

public:
    unsigned int Subscribe(Event event, std::function<void()> callback)
    {
        mNextId++;

        if (mPublishDepth > 0)
        {
            std::cout << "pending subscribe id=" << mNextId << std::endl;
            mPendingSubscribe.emplace_back(event, std::move(callback), mNextId);
        }
        else
        {
            mCallbacks[event].emplace_back(mNextId, std::move(callback));
        }
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
            if (mPublishDepth > 0)
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

        //        mPublishDepth++;

        try
        {
            PublishGuard publishGuard(mPublishDepth);

            std::cout << "mPublishDepth=" << mPublishDepth << std::endl;

            for (const auto &cb : it->second)
            {
                cb.mCallback();
            }
        }
        catch (...)
        {
            // PublishGuardのデストラクタはすでに呼ばれている
            //->mPublishDepthは1段戻っている
            if (mPublishDepth == 0)
            {
                FlushPendingOperations();
            }
            throw; // 捕まえた例外を再送出
        }
        //        mPublishDepth--;
        std::cout << "mPublishDepth=" << mPublishDepth << std::endl;
        if (mPublishDepth == 0)
        {
            FlushPendingOperations();
        }
    }
};

int main()
{
    std::cout << "event dispatcher start" << std::endl;

    int score = 0;
    unsigned int itemCreateId = 0;

    EventDispatcher eventDispatcher;

    unsigned int id0 =
        eventDispatcher.Subscribe(
            Event::PlayerDamaged,
            [&score, &eventDispatcher, &itemCreateId]()
            {
                score += 100;
                std::cout << "score + 100" << std::endl;

                itemCreateId =
                    eventDispatcher.Subscribe(
                        Event::ItemCreated,
                        []()
                        {
                            std::cout << "item created callback\n";
                        });
                throw std::runtime_error("test exeption");
            });

    unsigned int id1 = eventDispatcher.Subscribe(Event::EnemyDefeated, [&eventDispatcher, id0]()
                                                 {
                                                     std::cout << "enemy defeated" << std::endl;
                                                     eventDispatcher.Publish(Event::ItemCreated);
                                                     eventDispatcher.Unsubscribe(Event::PlayerDamaged, id0); });

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

    try
    {
        eventDispatcher.Publish(Event::PlayerDamaged);
    }
    catch (const std::exception &e)
    {
        std::cout << "caught: " << e.what() << std::endl;
    }

    eventDispatcher.Publish(Event::ItemCreated);

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

    std::cout << "item create id = " << itemCreateId << std::endl;

    return 0;
}

/*


*/