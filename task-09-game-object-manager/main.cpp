#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <cassert>

class GameObjectManager;

class BaseObject
{
private:
    int mId;
    float mX;
    float mY;

public:
    BaseObject(int id, float x, float y) : mId(id), mX(x), mY(y)
    {
    }
    virtual ~BaseObject() = default;

    virtual void Update(GameObjectManager &manager) = 0;

    int GetId() const { return mId; }
    float GetX() const { return mX; }
    void SetX(float x) { mX = x; }
    float GetY() const { return mY; }
    void SetY(float y) { mY = y; }
};

class Character : public BaseObject
{
private:
    int mHp;

public:
    Character(int id, float x, float y, int hp) : BaseObject(id, x, y), mHp(hp)
    {
    }

    int GetHp() const { return mHp; }
    void SetHp(int hp) { mHp = hp; }
};

class Player : public Character
{
public:
    Player(int id, float x, float y, int hp) : Character(id, x, y, hp)
    {
    }
    void Update(GameObjectManager &manager) override
    {
        float x = GetX();
        ++x;
        SetX(x);
    }
};

class Enemy : public Character
{
public:
    Enemy(int id, float x, float y, int hp) : Character(id, x, y, hp)
    {
    }
    void Update(GameObjectManager &manager) override;
};

class Item : public BaseObject
{
public:
    Item(int id, float x, float y) : BaseObject(id, x, y)
    {
    }
    void Update(GameObjectManager &manager) override
    {
    }
};

class GameObjectManager
{
private:
    std::vector<std::unique_ptr<BaseObject>> mObjects;

    std::vector<int> mPendingDeleteIds;

    std::vector<std::unique_ptr<BaseObject>> mPendingAddObjects;

    bool mIsUpdating = false;

    // 削除予定のオブジェクトを削除する
    void FlushPendingDeletes()
    {
        // 削除予定が存在する？
        if (mPendingDeleteIds.empty())
        {
            return;
        }

        auto newEnd = std::remove_if(
            mObjects.begin(),
            mObjects.end(),
            [&](const std::unique_ptr<BaseObject> &o)
            {
                for (auto id : mPendingDeleteIds)
                {
                    if (id == o->GetId())
                    {
                        return true;
                    }
                }
                return false;
            });
        if (newEnd != mObjects.end())
        {
            // 削除
            mObjects.erase(newEnd, mObjects.end());
        }
        mPendingDeleteIds.clear();
    }

    void FlushPendingAdds()
    {
        for (auto &o : mPendingAddObjects)
        {
            mObjects.push_back(std::move(o));
        }
        mPendingAddObjects.clear();
    }
    bool RequestRemoveObject(int id)
    {
        // 指定のIDのオブジェクトが存在するか
        auto object = FindObject(id);
        if (object == nullptr)
        {
            return false;
        }

        // 既に削除予定されているか？
        auto it = std::find_if(mPendingDeleteIds.begin(), mPendingDeleteIds.end(), [id](int pendingId)
                               { return pendingId == id; });
        if (it != mPendingDeleteIds.end())
        {
            return false;
        }

        mPendingDeleteIds.push_back(id);

        return true;
    }

public:
    ~GameObjectManager() = default;

    bool AddObject(std::unique_ptr<BaseObject> &object)
    {
        if (object == nullptr)
        {
            return false;
        }

        if (FindObject(object->GetId()) != nullptr)
        {
            return false;
        }

        int id = object->GetId();
        // auto it = std::find_if(mPendingAddObjects.begin(), mPendingAddObjects.end(), [id](const std::unique_ptr<BaseObject> &o)
        //                        { return id == o->GetId(); });

        auto it = std::find_if(
            mPendingAddObjects.begin(),
            mPendingAddObjects.end(),
            [id](const std::unique_ptr<BaseObject> &o)
            {
                return (o->GetId() == id);
            });

        if (it != mPendingAddObjects.end())
        {
            return false;
        }

        if (mIsUpdating)
        {
            mPendingAddObjects.push_back(std::move(object));
        }
        else
        {
            mObjects.push_back(std::move(object));
        }

        return true;
    }

    void UpdateAll()
    {
        mIsUpdating = true;
        for (auto &o : mObjects)
        {
            o->Update(*this);
        }
        mIsUpdating = false;

        FlushPendingDeletes();

        FlushPendingAdds();
    }

    const BaseObject *FindObject(int id) const
    {
        auto it = std::find_if(
            mObjects.begin(),
            mObjects.end(),
            [id](const std::unique_ptr<BaseObject> &o)
            {
                return (o->GetId() == id);
            });
        if (it != mObjects.end())
        {
            return it->get();
        }
        return nullptr;
    }

    bool RemoveObject(int id)
    {
        if (mIsUpdating)
        {
            return RequestRemoveObject(id);
        }

        auto newEnd = std::remove_if(
            mObjects.begin(),
            mObjects.end(),
            [id](const std::unique_ptr<BaseObject> &o)
            {
                return (o->GetId() == id);
            });

        if (newEnd != mObjects.end())
        {
            mObjects.erase(newEnd, mObjects.end());
            return true;
        }

        return false;
    }
};
void Enemy::Update(GameObjectManager &manager)
{
    if (GetHp() <= 0)
    {
        manager.RemoveObject(GetId());
        std::unique_ptr<BaseObject> item = std::make_unique<Item>(4, GetX(), GetY());
        manager.AddObject(item);
        return;
    }
    float x = GetX();
    --x;
    SetX(x);
}

int main()
{
    std::cout << "task 10 game object manager start" << std::endl;

    GameObjectManager manager;

    std::unique_ptr<BaseObject> player = std::make_unique<Player>(1, 10.0f, 0.0f, 100);
    manager.AddObject(player);

    std::unique_ptr<BaseObject> enemy = std::make_unique<Enemy>(2, 20.0f, 0.0f, 0);
    manager.AddObject(enemy);

    std::unique_ptr<BaseObject> item = std::make_unique<Item>(3, 30.0f, 0.0f);
    manager.AddObject(item);

    manager.UpdateAll();

    assert(manager.FindObject(2) == nullptr);
    assert(manager.FindObject(4) != nullptr);
    assert(manager.FindObject(4)->GetX() == 20.0f);

    {
        const BaseObject *o2 = manager.FindObject(2);
        if (o2 == nullptr)
        {
            std::cout << "o2 == nullptr" << std::endl;
        }
    }

    const BaseObject *o1 = manager.FindObject(1);
    if (o1)
    {
        std::cout << "o1 x " << o1->GetX() << std::endl;
    }
    const BaseObject *o2 = manager.FindObject(2);
    if (o2)
    {
        std::cout << "o2 x " << o2->GetX() << std::endl;
    }
    const BaseObject *o3 = manager.FindObject(3);
    if (o3)
    {
        std::cout << "o3 x " << o3->GetX() << std::endl;
    }

    const BaseObject *o2d = manager.FindObject(2);
    if (o2d == nullptr)
    {
        std::cout << "removed 2 object" << std::endl;
    }

    std::unique_ptr<BaseObject> duplicate = std::make_unique<Enemy>(1, 50, 0.0f, 80);
    bool result = manager.AddObject(duplicate);
    if (!result)
    {
        if (duplicate != nullptr)
        {
            std::cout << "dup x " << duplicate->GetX() << std::endl;
        }
    }

    const BaseObject *o1chk = manager.FindObject(1);
    if (o1chk)
    {
        std::cout << "o1chk x " << o1chk->GetX() << std::endl;
    }

    return 0;
}
