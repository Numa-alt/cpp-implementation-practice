#include <cassert>
#include <array>

#define BIT(x) (1 << (x))

// 盤面のサイズ
static constexpr int HEIGHT = 12;
static constexpr int WIDTH = 6;

enum class GameStatus
{
    Title,     // タイトル画面
    Playing,   // ゲーム中
    GameOver,  // ゲームオーバー
    GameClear, // ゲームクリア
};

enum class PlayerStatus
{
    Entry,      // 新しいぷよ出現
    Control,    // ぷよを操作中
    EraseCheck, // 消えるかチェック
    Erase,      // 消去中
    Falling,    // 落下中
    Result,     // 結果表示中
};

enum class GameResult
{
    Playing,
    GameOver,
    GameClear,
};

enum class BlockColor
{
    Empty, // 無し
    Red,
    Blue,
    Green,
    Yellow,
};

// 相方のブロックの位置
enum class Direction
{
    Up,
    Right,
    Down,
    Left,
};

enum class Buttons
{
    Rotate = BIT(0),
    Left = BIT(1),
    Right = BIT(2),
    Down = BIT(3),
};

class InputManager
{
private:
    unsigned int mPressed = 0;
    unsigned int mReleased = 0;
    unsigned int mRepeat = 0;

public:
    bool IsPressed(Buttons button) const
    {
        return (mPressed & static_cast<unsigned int>(button)) != 0;
    }

    bool IsRepeat(Buttons button) const
    {
        return (mRepeat & static_cast<unsigned int>(button)) != 0;
    }
};

// 操作中のブロック
class FallingPair
{
private:
    int mX; // 左端を0 右端をWIDTH-1とする
    int mY; // 一番上の段をHEIGHT-1、一番下の段を0とする
    Direction mDirection;
    std::array<BlockColor, 2> mColors;

public:
    void Reset()
    {
        mX = 0;
        mY = HEIGHT - 1;
        mDirection = Direction::Left;
        for (auto& c : mColors)
        {
            c = BlockColor::Empty;
        }
    }
    int GetX() const
    {
        return mX;
    }
    int GetY() const
    {
        return mY;
    }
    void SetDirection(Direction next)
    {
        mDirection = next;
    }
    Direction GetDirection() const
    {
        return mDirection;
    }
    void SetColor(int index, BlockColor color)
    {
        assert((index >= 0) && (index < 2));
        mColors[index] = color;
    }
    BlockColor GetColor(int index) const
    {
        return mColors[index];
    }

    void SetPosition(int x, int y)
    {
        mX = x;
        mY = y;
    }

    void Move(int x, int y)
    {
        mX += x;
        mY += y;
    }

    void Rotate()
    {
        switch (mDirection)
        {
        case Direction::Down:
            mDirection = Direction::Left;
            break;
        case Direction::Left:
            mDirection = Direction::Up;
            break;
        case Direction::Up:
            mDirection = Direction::Right;
            break;
        case Direction::Right:
            mDirection = Direction::Down;
            break;
        }
    }
};

// 盤面
class Board
{
private:
    // 左端を0 右端をWIDTH-1
    // 一番上の段をHEGHT-1 一番下の段を0とする
    BlockColor mCells[HEIGHT][WIDTH];

public:
    Board()
    {
        Reset();
    }

    void Reset()
    {
        for (auto& row : mCells)
        {
            for (auto& c : row)
            {
                c = BlockColor::Empty;
            }
        }
    }

    bool CanPlace(const FallingPair& pair) const
    {
        // 軸のブロック
        int x = pair.GetX();
        int y = pair.GetY();

        // 軸のブロックが範囲外？
        if ((x < 0) || (x >= WIDTH) || (y < 0) || (y >= HEIGHT))
        {
            return false;
        }
        // 軸のブロックの位置が空いてない？
        if (mCells[y][x] != BlockColor::Empty)
        {
            return false;
        }

        // 相方のブロックの位置
        switch (pair.GetDirection())
        {
        case Direction::Down:
            y -= 1;
            break;
        case Direction::Left:
            x -= 1;
            break;
        case Direction::Up:
            y += 1;
            break;
        case Direction::Right:
            x += 1;
            break;
        }
        // 相方のブロックが範囲外？
        if ((x < 0) || (x >= WIDTH) || (y < 0) || (y >= HEIGHT))
        {
            return false;
        }

        // 相方のブロックの位置が空いない？
        if (mCells[y][x] != BlockColor::Empty)
        {
            return false;
        }

        return true;
    }
};

class PlayingManager
{
private:
    FallingPair mFallingPair; // 操作中のブロック
    FallingPair mNextPair;    // 次に出現するブロック
    Board mBoard;
    PlayerStatus mStatus = PlayerStatus::Entry;
    int mChainCount = 0;
    GameResult mGameResult = GameResult::Playing;

public:
    void Start()
    {
        mChainCount = 0;
        mGameResult = GameResult::Playing;
        mBoard.Reset();
        mFallingPair.Reset();
        mNextPair.Reset();

        GenerateNextPair();
        SetStatus(PlayerStatus::Entry);
    }

    void GenerateNextPair();

    // ステータス変更時に行う処理
    void SetStatus(PlayerStatus next);

    void Execute(const InputManager& input);

    void Entry();
    void Control(const InputManager& input);
    void Falling();
    void EraseCheck();
    void Erase();

    // GameManager側から結果を取得するため
    GameResult GetGameResult() const { return mGameResult; }
};

void PlayingManager::GenerateNextPair()
{
    // 出現位置
    mNextPair.SetPosition(2, HEIGHT - 1);
    // ランダムに色を設定する
    mNextPair.SetColor(0, BlockColor::Blue);
    mNextPair.SetColor(1, BlockColor::Red);
}

// ステータス変更時に行う処理
void PlayingManager::SetStatus(PlayerStatus next)
{
    mStatus = next;

    switch (next)
    {
    case PlayerStatus::Entry:
        {
            mFallingPair = mNextPair;

            if (!mBoard.CanPlace(mFallingPair)) // 追加できる場所がない?
            {
                mGameResult = GameResult::GameOver;
                SetStatus(PlayerStatus::Result);
                return;
            }

            // 次のブロックの色をランダムで設定する
            GenerateNextPair();

            mChainCount = 0;
            break;
        }

    case PlayerStatus::Erase:
        mChainCount++;
        break;
    }
}

void PlayingManager::Execute(const InputManager& input)
{
    switch (mStatus)
    {
        // 新規ブロック追加
    case PlayerStatus::Entry:
        Entry();
        break;

        // 操作中
    case PlayerStatus::Control:
        Control(input);
        break;

        // 落下中
    case PlayerStatus::Falling:
        Falling();
        break;

        // 消えるかチェック
    case PlayerStatus::EraseCheck:
        EraseCheck();
        break;

        // 消去
    case PlayerStatus::Erase:
        Erase();
        break;

        // 結果表示中
    case PlayerStatus::Result:
        break;
    }
}
// 新しいぷよ出現
void PlayingManager::Entry()
{

    if () // ぷよ追加演出完了？
    {
        SetStatus(PlayerStatus::Control);
    }
}
//落下中のブロックの操作
void PlayingManager::Control(const InputManager& input)
{
    if (input.IsRepeat(Buttons::Left))
    {
        FallingPair pair = mFallingPair;
        pair.Move(-1, 0);
        if (mBoard.CanPlace(pair)) // 左へ移動可能？
        {
            // 左へ移動
            mFallingPair = pair;
        }
    }
    else if (input.IsRepeat(Buttons::Right))
    {
        FallingPair pair = mFallingPair;
        pair.Move(1, 0);
        if (mBoard.CanPlace(pair)) // 右へ移動可能？
        {
            // 右へ移動
            mFallingPair = pair;
        }
    }
    else if (input.IsRepeat(Buttons::Down))
    {
        FallingPair pair = mFallingPair;
        pair.Move(0, -1);
        if (mBoard.CanPlace(pair))
        {
            // 1段下へ移動
            mFallingPair = pair;
        }
    }

    if (input.IsPressed(Buttons::Rotate))
    {
        // 回転処理
        FallingPair pair = mFallingPair;

        //90度
        pair.Rotate();
        if (mBoard.CanPlace(pair))
        {
            mFallingPair = pair;
        }
        else {
            //180度
            pair.Rotate();
            if (mBoard.CanPlace(pair))
            {
                mFallingPair = pair;
            }
        }
    }

    //
    if () // 前回の落下から一定時間経過？
    {
        // 1段落下
    }

    if () // 着地して、一定時間経過している？
    {
        // 着地処理
        SetStatus(PlayerStatus::Falling);
    }
}

// 消去チェック
void PlayingManager::EraseCheck()
{
    if () // 消える？
    {
        SetStatus(PlayerStatus::Erase);
    }
    else
    {
        SetStatus(PlayerStatus::Entry);
    }
}

// ぷよを消去
void PlayingManager::Erase()
{

    if () // 消去処理終了？
    {
        if () // 落下するぷよがある？
        {
            SetStatus(PlayerStatus::Falling); //
        }
        else
        {
            SetStatus(PlayerStatus::Entry);
        }
    }
}

// ぷよ落下処理
void PlayingManager::Falling()
{
    if () // 落下処理終了？
    {
        SetStatus(PlayerStatus::EraseCheck);
    }
}

//--------------------------------------

class GameManager
{
private:
    GameStatus mGameStatus = GameStatus::Title;

    InputManager mInputManager;

    PlayingManager mPlayingManager;

public:
    void Execute();

    //
    void SetStatus(GameStatus next);

    void Title();
    void Playing();
    void GameOver();
    void GameClear();
};

void GameManager::SetStatus(GameStatus next)
{
    if (next == GameStatus::Playing)
    {
        mPlayingManager.Start();
    }
    mGameStatus = next;
}

void GameManager::Execute()
{
    switch (mGameStatus)
    {
    case GameStatus::Title:
        Title();
        break;

    case GameStatus::Playing:
        Playing();
        break;

    case GameStatus::GameOver:
        GameOver();
        break;

    case GameStatus::GameClear:
        GameClear();
        break;
    }
}

// タイトル
void GameManager::Title()
{
    if (mInputManager.IsPressed(Buttons::Rotate))
    {
        SetStatus(GameStatus::Playing);
    }
}
// ゲーム中
void GameManager::Playing()
{
    mPlayingManager.Execute(mInputManager);

    GameResult result = mPlayingManager.GetGameResult();
    switch (result)
    {
    case GameResult::Playing:
        break;

    case GameResult::GameOver:
        SetStatus(GameStatus::GameOver);
        break;

    case GameResult::GameClear:
        SetStatus(GameStatus::GameClear);
        break;
    }
}
// ゲームオーバー
void GameManager::GameOver()
{
    if (mInputManager.IsPressed(Buttons::Rotate))
    {
        // タイトルへ
        SetStatus(GameStatus::Title);
    }
}
// ゲームクリア
void GameManager::GameClear()
{
    if (mInputManager.IsPressed(Buttons::Rotate))
    {
        // タイトルへ
        SetStatus(GameStatus::Title);
    }
}
