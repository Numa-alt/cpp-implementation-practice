
共有データはタスクのキュー、停止フラグ、処理中のスレッドのカウンタ
mutexでこれらを守る
Workerが仕事なしで待つにはcondition_variableで停止フラグが立つかキューにタスクが追加されるまでまつ
Task追加時は、mutexでロックし、taskキューにデータを追加、notify_oneでスレッドに通知
前タスク完了の判定は、処理中スレッドのカウンタが0、かつキューがから。
destructorで安全に処理するには、ミューテックスのロック、停止フラグを立てる、全スレッドを起こす、全スレッドのjoinableチェック、joinで待つ
重い処理をmutexの外で行うのは、他のスレッドも処理できるようにするため。
notify_oneはタスクが追加された時、notify_allはTaskManagerのdestructorで停止フラグを立てた後


ぷよぷよ風の設計

画面遷移
    タイトル画面
        ボタンを押したら開始

    ゲーム画面
        クリア、ゲームオーバーはオーバーラップで表示

ゲーム中の画面表示
    背景枠
    消した数
    操作中のぷよ
    積まれたぷよ
    次に出てくるぷよ

左右のキーで操作中のぷよが左右に移動する
移動先につまれたぷよがあれば動かない
ぷよは行って時間経過で1マス下に移動。

下キーオンの瞬間に1マス下に移動。リピート入力可能とする。
左右キーオンの瞬間に1マス移動、そのままキーを押し続けたら、短時間待ってまた移動、まだ押し続けていたらリピートで移動
イメージは、キー入力、即座に1マス移動、0.5秒待つ、1マス移動、0.2秒待つ、1マス移動・・・
ボタンを押すと操作中のぷよの右側が起点となり左側のぷよが90度回転。回転先につまれたぷよがあるなら、さらに90度回転。

着地後、一定時間経過で操作中のぷよが固定。
着地後、下キーで即座に固定。

固定後、消滅処理。連鎖処理、ゲームオーバー判定。

その後、新規ぷよ追加。


GameManagerクラス
    全部のクラスを管理する

GameStateクラス
    タイトル
    プレイ中
    ゲームオーバー
    クリア


Boardクラス
    積まれたぷよ
    衝突判定
    消去判定
    落下処理
    連鎖処理
    ゲームオーバー判定

FallingPairクラス
    操作中の2個のぷよ
    座標
    向き
    左右移動
    落下
    回転

NextQueueクラス
    次に出すぷよ

Rendererクラス
    背景枠
    操作中のぷよ
    積まれたぷよ
    次のぷよ
    消去数
    連鎖数
    ゲームオーバー
    ゲームクリア

Inputクラス
   on (押された瞬間1フレームだけボタンのビットが立つ)
   off (離された瞬間1フレームだけボタンのビットが立つ)
   repeat (押されている間、押された瞬間の1フレームと、一定間隔をあけてビットが立つ)

   on & BIT(BUTTON_DOWN)
   repeat & BIT(BUTTON_LEFT)とか使う

ゲームシステム(ハードから分離)
    GameManager{
        GameStates
        Board
        FallingPair
        NextQueue
        GameState
    }

RendererとInputはハード依存部分

ゲーム中の細かいステータス
    操作ぷよ出現
    落下中
    固定中
    消去
    落下
    ゲームオーバー


Update()
{
    input.execute();

    gameManager.execute();

    renderer.execute();
}

GameManager()
{
    void execute()
    {
        function[mStatus].execute();
            //タイトル
            //ゲーム
            //ゲームオーバー
            //ゲームクリア
    }
}

//ゲーム
{
    gameFunction[mGameStatus].exeute();
        操作ぷよ出現
        落下中
        固定中
        消去
        落下

}

enum class GameStatus
{
    Title,
    Playing,
    GameOver,
    GameClear,
};

enum class BlockColor
{
    Empty,//無し
    Red,
    Blue,
    Green,
    Yellow,
};

enum class PlayerStatus
{
    Entry, //新しいぷよ出現
    Control,//ぷよを操作中
    EraseCheck,//消えるかチェック
    Erase,//消去中
    Falling,//落下中
};

//相方のブロックの位置
enum class Direction
{
    Up,
    Right,
    Down,
    Left,
};

void SetStatus( PlayerStatus next )
{
    mPlayerStatus = next;
    switch(next)
    {
        case PlayerStatus::Entry:
            {
                mChainCount = 0;
            }
            break;
        case PlayerStatus::Erase:
            {
                mChainCount++;
            }
            break;
    }
}
Control()
{
    unsigned int pressed = input.getPressed();
    unsigned int repeat = input.getRepeat();

    if( repeat & BIT(BUTTON_LEFT) )
    {
        if()//左へ移動可能？
        {
            //左移動
        }
    }
    else if( repeat & BIT(BUTTON_RIGHT))
    {
        if()//右へ移動可能？
        {
            //右へ移動
        }
    }
    if(reepat&BIT(BUTTON_DOWN))
    {
        //1段下処理
    }
    
    if(pressed&BIT(BUTTON_ROATE))
    {
        //回転処理
    }

    //
    if( )//前回の落下から一定時間経過？
    {
        //1段落下
    }

    if()//着地して、一定時間経過している？
    {
        //着地処理
        SetStatus( PlayerStatus::Falling );
    }
}


//消去チェック
EraseCheck()
{
    if()//消える？
        SetStatus(  PlayerStatus::Erase );
    else
        SetStatus(  PlayerStatus::Entry );
}

//ぷよを消去
Erase()
{

    if()//消去処理終了？
    {
        if()//落下するぷよがある？
        {
            SetStatus( PlayerStatus::Falling );//
        }
        else
        {
            SetStatus(  PlayerStatus::Entry );
        }
    }
}

//ぷよ落下処理
Falling()
{
    if()//落下処理終了？
    {
        SetStatus ( PlayerStatus::EraseCheck );
    }
}

//新しいぷよ出現
Entry()
{
    if()//追加できる場所がない
    {
        mGameStatus = GameStatus::GameOver;
        return;
    }

    if()//ぷよ追加演出完了？
    {
        SetStatus( PlayerStatus::Control );
    }
}

static constexpr int HEIGHT = 12;
static constexpr int WIDTH = 6;

//操作中のブロック
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
    BlockColor GetColor( int index )
    {
        return mColors[index];
    }
};

//盤面
class Board
{
private:
    BlockColor mCells[HEIGHT][WIDTH];
public:
    Board()
    {
        for(auto & row : mCells)
        {
            for( auto & c : row )
            {
                c=BlockColor::Empty;
            }
        }
    }
    bool CanPlace(const FallingPair&pair) const
    {

    }
};

