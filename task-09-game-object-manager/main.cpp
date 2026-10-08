#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>

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
    virtual void Update() = 0;

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
    virtual void Update() = 0;
    int GetHp() const { return mHp; }
    void SetHp(int hp) { mHp = hp; }
};

class Player : public Character
{
public:
    Player(int id, float x, float y, int hp) : Character(id, x, y, hp)
    {
    }
    void Update() override
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
    void Update() override
    {
        float x = GetX();
        --x;
        SetX(x);
    }
};

class Item : public BaseObject
{
public:
    Item(int id, float x, float y) : BaseObject(id, x, y)
    {
    }
    void Update() override
    {
    }
};

class GameObjectManager
{
private:
    std::vector<std::unique_ptr<BaseObject>> mObjects;

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

        mObjects.push_back(std::move(object));

        return true;
    }

    void UpdateAll()
    {
        for (auto &o : mObjects)
        {
            o->Update();
        }
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

int main()
{
    std::cout << "task 09 game object manager start" << std::endl;

    GameObjectManager manager;

    std::unique_ptr<BaseObject> player = std::make_unique<Player>(1, 10.0f, 0.0f, 100);
    manager.AddObject(player);

    return 0;
}
