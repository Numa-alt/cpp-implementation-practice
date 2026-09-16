#include <algorithm>
#include <iostream>
#include <optional>
#include <string>
#include <utility>
#include <vector>

class Score
{
private:
    int id;
    std::string name;
    int point;

public:
    Score(int _id, std::string _name, int _point)
        : id(_id), name(std::move(_name)), point(_point) {}
    int GetId() const { return id; }
    const std::string &GetName() const { return name; }
    int GetPoint() const { return point; }
};

class ScoreManager
{
private:
    std::vector<Score> scoreTable;
    int nextId;

    int GetNextId()
    {
        nextId++;
        return nextId;
    }

public:
    ScoreManager() : nextId(0) {}

    void AddScore(std::string name, int point)
    {
        scoreTable.emplace_back(GetNextId(), std::move(name), point);
    }
    std::optional<int> FindPointById(int id) const
    {
        auto it =
            std::find_if(scoreTable.begin(), scoreTable.end(),
                         [id](const auto &a)
                         { return (a.GetId() == id); });

        if (it != scoreTable.end())
        {
            return it->GetPoint();
        }
        return std::nullopt;
    }

    std::optional<int> FindIdByName(const std::string &name) const
    {
        auto it = std::find_if(
            scoreTable.begin(), scoreTable.end(),
            [name](const auto &a)
            { return (a.GetName() == name); });

        if (it != scoreTable.end())
        {
            return it->GetId();
        }
        return std::nullopt;
    }

    bool RemoveScoreById(int id)
    {
        auto it =
            std::remove_if(scoreTable.begin(), scoreTable.end(),
                           [id](const auto &a)
                           { return (a.GetId() == id); });

        if (it != scoreTable.end())
        {
            scoreTable.erase(it, scoreTable.end());
            return true;
        }
        return false;
    }
    const std::vector<Score> &GetScores() const { return scoreTable; }
};

void DisplayScore(const std::vector<Score> &score)
{
    for (const auto &i : score)
    {
        std::cout << "id " << i.GetId() << " name " << i.GetName() << " point "
                  << i.GetPoint() << "\n";
    }
}
int main()
{
    ScoreManager scoreManager;
    scoreManager.AddScore("test0", 90);
    scoreManager.AddScore("test1", 80);
    scoreManager.AddScore("test2", 85);

    std::optional<int> id = scoreManager.FindIdByName("test0");
    if (id.has_value())
    {
        std::optional<int> point = scoreManager.FindPointById(id.value());
        if (point.has_value())
        {
            std::cout << "id " << id.value() << " point " << point.value()
                      << "\n";
        }
    }

    DisplayScore(scoreManager.GetScores());

    if (id.has_value())
    {
        std::cout << "remove id " << id.value() << "\n";
        scoreManager.RemoveScoreById(id.value());
    }

    DisplayScore(scoreManager.GetScores());

    return 0;
}
