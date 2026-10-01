#include <string>
#include <iostream>
#include <utility>
#include <memory>
#include <vector>
#include <functional>
#include <stdexcept>
#include <cstdio>
#include <string_view>
#include <span>
#include <ranges>

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

class Trace
{
public:
    Trace()
    {
        std::cout << "Trace costructor\n";
    }
    ~Trace()
    {
        std::cout << "Trace destructor\n";
    }
};

void Test(int value)
{
    Trace trace;
    std::cout << "Before throw\n";

    if (value < 0)
    {
        throw std::runtime_error("value is negative");
    }
    std::cout << "after throw\n";
};

struct FileCloser
{
    void operator()(std::FILE *file) const noexcept
    {
        std::cout << "File closed\n";
        std::fclose(file);
    }
};

using FilePtr = std::unique_ptr<std::FILE, FileCloser>;

class SafeFile
{
private:
    FilePtr mFile;

public:
    SafeFile(const char *filename) : mFile(std::fopen(filename, "r"))
    {
        if (!mFile)
        {
            throw std::runtime_error("file open failed");
        }
        std::cout << "File opened\n";

        //
        throw std::runtime_error("error after open");
    }
};

struct Enemy
{
    std::string name;
    int hp;
};

void PrintName(std::string_view name)
{
    std::cout << name << "\n";
}

void PrintEnemies(std::span<const Enemy> enemies)
{
    for (const Enemy &e : enemies)
    {
        std::cout << e.name << " hp=" << e.hp << "\n";
    }
}

struct Enemy2
{
    std::string name;
    int hp;
};

std::vector<Enemy2> enemies2 =
    {
        {"slime", 10},
        {"Orc", 30},
        {"Dragon", 100},
};

int main()
{
    auto it = std::ranges::find_if(
        enemies2,
        [](const Enemy2 &e)
        {
            return e.hp >= 50;
        });
    if (it != enemies2.end())
    {
        std::cout << it->name << "hp=" << it->hp << "\n";
    }
    // std::vector<int> values = {1, 2, 3, 4, 5, 6};
    // auto result =
    //     values | std::views::filter([](int v)
    //                                 { return (v % 2) == 0; }) |
    //     std::views::transform(
    //         [](int v)
    //         {
    //             return v * 3;
    //         });

    // for (auto v : result)
    // {
    //     std::cout << "v:" << v << "\n";
    // }

    // std::vector<Enemy> enemies =
    //     {
    //         {"Slime", 10},
    //         {"Orc", 30},

    // };

    // PrintEnemies(enemies);

    // try
    // {
    //     SafeFile file("CMakeLists.txt");
    // }
    // catch (const std::exception &e)
    // {
    //     std::cout << "error " << e.what() << '\n';
    // }

    // int score = 0;
    // Button button;
    // button.SetOnClick([&score]()
    //                   { score += 10; std::cout << "Clicked\n"; });
    // button.Click();
    // button.Click();
    // std::cout << "score = " << score << "\n";
    return 0;
}
