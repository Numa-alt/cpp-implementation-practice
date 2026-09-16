#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

class Character
{
private:
    std::string mName;
    int mMaxHp;
    int mHp;

public:
    // maxHpが０以下の場合は1に補正する
    Character(const std::string &name, int maxHp) : mName(name)
    {
        if (maxHp <= 0)
        {
            maxHp = 1;
        }
        mMaxHp = maxHp;
        mHp = maxHp;
    }

    // ダメージを与える
    void Damage(int damage)
    {
        if (damage <= 0)
        {
            // ０以下のダメージならなにもしない
            return;
        }

        mHp -= damage;
        if (mHp < 0)
        {
            mHp = 0;
        }
        return;
    }

    // 死亡しているか？
    bool IsDead() const { return (mHp <= 0); }

    const std::string &GetName() const { return mName; }
    int GetMaxHp() const { return mMaxHp; }
    int GetHp() const { return mHp; }
};

class Party
{
private:
    std::vector<Character> mCharacters;

public:
    // キャラクターを追加
    // すでに存在する名前の場合はfalse
    bool AddCharacter(const std::string &name, int maxHp)
    {
        auto it = std::find_if(
            mCharacters.begin(),
            mCharacters.end(),
            [&name](const auto &c)
            {
                return (c.GetName() == name);
            });

        if (it == mCharacters.end())
        {
            mCharacters.emplace_back(name, maxHp);
            return true;
        }
        // 同じ名前のキャラがいる
        return false;
    }

    // 名前を指定してキャラクターを検索する
    // 存在しない場合はnullptrを返す
    const Character *FindCharacterByName(const std::string &name) const
    {
        auto it = std::find_if(
            mCharacters.begin(),
            mCharacters.end(),
            [&name](const auto &c)
            {
                return (c.GetName() == name);
            });

        if (it != mCharacters.end())
        {
            return &(*it);
        }
        return nullptr;
    }

    // 指定した名前のキャラクターにダメージを与える
    // 存在しない場合はfalseを返す
    bool Damage(const std::string &name, int damage)
    {
        auto it = std::find_if(
            mCharacters.begin(),
            mCharacters.end(),
            [&name](const auto &c)
            {
                return (c.GetName() == name);
            });

        if (it != mCharacters.end())
        {
            it->Damage(damage);
            return true;
        }
        return false;
    }

    // 戦闘不能になっているCharacterをすべてパーティーから削除する
    // 削除した数を返す
    unsigned int RemoveDeadCharacters()
    {
        auto it = std::remove_if(
            mCharacters.begin(),
            mCharacters.end(),
            [](const auto &c)
            {
                return c.IsDead();
            });
        auto removeNum = mCharacters.end() - it;
        mCharacters.erase(it, mCharacters.end());

        return static_cast<unsigned int>(removeNum);
    }

    const std::vector<Character> &GetCharacterList() const
    {

        return mCharacters;
    }
};

void DisplayParty(const std::vector<Character> &list)
{
    std::cout << "party\n";
    for (const auto &c : list)
    {
        std::cout << "    " << c.GetName() << " Hp " << c.GetHp() << "/" << c.GetMaxHp() << "\n";
    }
}

int main()
{
    std::cout << "task02-party\n";

    Party party;
    party.AddCharacter("Alice", 100);
    party.AddCharacter("Bob", 120);
    party.AddCharacter("Carol", 130);
    party.AddCharacter("Dean", -1);
    bool added = party.AddCharacter("Carol", 30);
    std::cout << "added " << added << "\n";

    bool noExistCharacterDamage = party.Damage("Casy", 40);
    std::cout << "noExistCharacterDamage" << noExistCharacterDamage << "\n";

    const Character *p = party.FindCharacterByName("google");
    if (p == nullptr)
    {
        std::cout << "no exist find character by name if failed\n";
    }

    party.Damage("Alice", -50);

    DisplayParty(party.GetCharacterList());

    party.Damage("Bob", 60);

    DisplayParty(party.GetCharacterList());

    party.Damage("Bob", 60);

    DisplayParty(party.GetCharacterList());

    party.RemoveDeadCharacters();

    DisplayParty(party.GetCharacterList());

    return 0;
}
