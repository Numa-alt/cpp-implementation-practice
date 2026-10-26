
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
        return static_cast<bool>(mPressed & button);
    }
    bool IsRepeat(Buttons button) const
    {
        return static_cast<bool>(mRepeat & button);
    }
};

class PlayingManager;

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

// 盤面
class Board
{
private:
    BlockColor mCells[HEIGHT][WIDTH];

public:
    Board()
    {
        for (auto &row : mCells)
        {
            for (auto &c : row)
            {
                c = BlockColor::Empty;
            }
        }
    }
    bool CanPlace(const FallingPair &pair) const
    {
        return false;
    }
};

// 操作中のブロック
class FallingPair
{
private:
    int mX;
    int mY;
    Direction mDirection;
    BlockColor mColors[2];

public:
    int GetX() const
    {
        return mX;
    }
    int GetY() const
    {
        return mY;
    }
    Direction GetDirection() const
    {
        return mDirection;
    }
    BlockColor GetColor(int index)
    {
        return mColors[index];
    }
    void SetDirection(Direction next)
    {
        mDirection = next;
    }
};

class PlayingManager
{
private:
    FallingPair mFallingPair;
    Board mBoard;
    PlayerStatus mStatus = PlayerStatus::Entry;
    int mChainCount = 0;
    GameResult mGameResult = GameResult::Playing;

public:
    void Start()
    {
        mChainCount = 0;
        mGameResult = GameResult::Playing;
    }

    // ステータス変更時に行う処理
    void SetStatus(PlayerStatus next);

    void Execute(const InputManager &input);

    void Entry();
    void Control(const InputManager &input);
    void Falling();
    void EraseCheck();
    void Erase();

    // GameManager側から結果を取得するため
    GameResult GetGameResult() const { return mGameResult; }
};

// ステータス変更時に行う処理
void PlayingManager::SetStatus(PlayerStatus next)
{
    mStatus = next;

    switch (next)
    {
    case PlayerStatus::Entry:
        mChainCount = 0;
        break;

    case PlayerStatus::Erase:
        mChainCount++;
        break;
    }
}

void PlayingManager::Execute(const InputManager &input)
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
    if () // 追加できる場所がない
    {
        mGameResult = GameResult::GameOver;
        SetStatus(PlayerStatus::Result);
        return;
    }

    if () // ぷよ追加演出完了？
    {
        SetStatus(PlayerStatus::Control);
    }
}

void PlayingManager::Control(const InputManager &input)
{
    if (input.IsRepeat(Buttons::Left))
    {
        if () // 左へ移動可能？
        {
            // 左移動
        }
    }
    else if (input.IsRepeat(Buttons::Right))
    {
        if () // 右へ移動可能？
        {
            // 右へ移動
        }
    }
    if (input.IsRepeat(Buttons::Down))
    {
        // 1段下処理
    }

    if (input.IsPressed(Buttons::Rotate))
    {
        // 回転処理
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
