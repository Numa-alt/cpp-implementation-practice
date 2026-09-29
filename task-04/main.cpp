#include <string>
#include <iostream>
#include <utility>
#include <memory>
#include <vector>
#include <functional>

class Data
{
private:
    std::string mName;
    int mId;

public:
    const std::string &GetName() const { return mName; }
    Data(int id, const std::string &name)
    {
        mId = id;
        std::cout << "string lvalue constructor\n";
        mName = name;
    }
    Data(int id, const Data &other)
    {
        mId = id;
        std::cout << "data copy constructor\n";
        mName = other.mName;
    }

    Data(int id, std::string &&name)
    {
        mId = id;
        std::cout << "string rvalue constructor\n";
        mName = std::move(name);
    }

    Data(const Data &other)
    {
        std::cout << "Data copy constructor\n";
        mId = other.mId;
        mName = other.mName;
    }
    Data(Data &&other) noexcept
    {
        std::cout << "Data move constructor\n";
        mId = other.mId;
        mName = other.mName;
    }
};

void Receive(const Data &data)
{
    std::cout << "Receive lvalue " << data.GetName() << "\n";
}

void Receive(Data &&data)
{
    std::cout << "Receive rvalue " << data.GetName() << "\n";
}

template <class T>
void ForwardTest(T &&value)
{
    Receive(std::forward<T>(value));
};

//...は繰り返したい式につける
template <class... Args>
Data MakeData(Args &&...args)
{
    return Data(std::forward<Args>(args)...);
}

template <class T, class... Args>
std::unique_ptr<T> MyMakeUnique(Args &&...args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

int main2()
{
    std::cout << "task04\n";

    std::cout << "emplace_back\n";
    std::vector<Data> list;
    list.reserve(10);

    std::string name = "X";
    list.emplace_back(4, name);
    list.emplace_back(5, std::move(name));

    std::cout << "push_back\n";
    std::string name2 = "Y";
    list.push_back(Data(6, name2));
    list.push_back(Data(7, std::move(name2)));

    // std::string s = "A";
    // Data a = MakeData(0, s);

    // Data b = MakeData(1, std::move(s));

    // std::string u = "U";
    // auto p1 = MyMakeUnique<Data>(2, u);
    // auto p2 = MyMakeUnique<Data>(3, std::move(u));

    // ForwardTest(a);
    // ForwardTest(std::move(a));
    return 0;
}

// std::function<void(int)> func;
// func = Print;
// func(100);

// [multiplier](int value)
// {
//     return value * multiplier;
// }

// Execute(10,
//         [multiplier](int value)
//         {
//             return value * multiplier;
//         });

class Button
{
public:
    void SetOnClick(std::function<void()> func)
    {
        mFunction = func;
    }
    void Click()
    {
        if (mFunction)
        {
            mFunction();
        }
    }

private:
    std::function<void()> mFunction;
};

int main()
{
    int score = 0;
    Button button;
    button.SetOnClick([&score]()
                      { score += 10; std::cout << "Clicked\n"; });
    button.Click();
    button.Click();
    std::cout << "score = " << score << "\n";
    return 0;
}
