#include <iostream>
#include <optional>
#include <string>
#include <vector>
#include <algorithm>
#include <utility>

class Score{
private:
    int id;
    std::string name;
    int point;
public:
    Score(int _id, std::string _name, int _point):id(_id),name(std::move(_name)),point(_point){}
    int GetId() const{return id;}
    const std::string& GetName()const{return name;}
    int GetPoint()const{ return point;}
};

class ScoreManager{
private:
    std::vector<Score> scoreTable;
    int nextId;

    int GetNextId(){
        nextId++;
        return nextId;
    }

public:
    ScoreManager():nextId(0){}

    void AddScore(std::string name, int point){
        scoreTable.emplace_back( GetNextId(), std::move(name), point );
    }
    std::optional<int> FindPointById(int id) const
    {
        auto it = std::find_if(
            scoreTable.begin(),
            scoreTable.end(),
            [id](const auto& a)
            {
                return ( a.GetId() == id );
            }
        );
        
        if(it != scoreTable.end()){
            return it->GetPoint();
        }
        return std::nullopt;
    }

    std::optional<int> FindIdByName( const std::string & name ) const
    {
        auto it = std::find_if(
            scoreTable.begin(),
            scoreTable.end(),
            [ name ]( const auto & a)
            {
                return ( a.GetName() == name );
            }
        );

        if( it != scoreTable.end() ){
            return it->GetId();
        }
        return std::nullopt;
    }

    bool RemoveScoreById(int id)
    {
        auto it = std::remove_if(
            scoreTable.begin(),
            scoreTable.end(),
            [id]( const auto& a)
            {
                return ( a.GetId() == id );
            }
        );

        if( it != scoreTable.end() ){
            scoreTable.erase(it, scoreTable.end() );
            return true;
        }
        return false;
    }
    const std::vector<Score>& GetScores()const{
        return scoreTable;
    }
};

void DisplayScore( const std::vector<Score>& score )
{
    for( const auto & i : score ){
        std::cout<<"id "<<i.GetId()<<" name "<<i.GetName() << " point " << i.GetPoint()<<"\n";
    }
}
void TestFunc()
{
    ScoreManager scoreManager;
    scoreManager.AddScore("test0",90 );
    scoreManager.AddScore("test1",80 );
    scoreManager.AddScore("test2",85 );

    std::optional<int> id = scoreManager.FindIdByName("test0");
    if( id.has_value() ){
        std::optional<int>point = scoreManager.FindPointById(id.value());
        if( point.has_value() ){
            std::cout<<"id "<<id.value() << " point " << point.value() << "\n";
        }
    }

    DisplayScore( scoreManager.GetScores() );

    if( id.has_value() ){
        std::cout<<"remove id " << id.value() <<"\n";
        scoreManager.RemoveScoreById( id.value() );
    }

    DisplayScore( scoreManager.GetScores() );
}

//タスクマネージャ設計書

#include <algorithm>
#include <iostream>
#include <vector>
#include <string>

//状態
enum class Status{
    Todo,
    Doing,
    Done,
};
class Task
{
private:
        int mId;
        std::string mTitle;
        Status mStatus;
    public:
        Task(const int id, const std::string &title):mId(id),mTitle(title),mStatus(Status::Todo)
        {

        }

        int GetId()const
        {
            return mId;
        }

        void SetStatus( const Status status )
        {
            mStatus = status;
        }

        Status GetStatus() const
        {
            return mStatus;
        }

        void SetTitle( const std::string &title )
        {
            mTitle = title;
        }
        const std::string & GetTitle() const
        {
            return mTitle;
        }

};

class TaskManager
{
    private:
        int mNextId;
        std::vector<Task> mTasks;

        //  ID発行    
        int GetNextId()
        {
            mNextId++;
            return mNextId;
        }

    public:

        TaskManager():mNextId(0){}

        //タイトル指定 成功時true 失敗時false
        bool SetTitle(const int id, const std::string & title)
        {
            auto it = std::find_if(
                mTasks.begin(),
                mTasks.end(),
                [ id ]( const auto & a )
                {
                    return( a.GetId() == id );
                }
            );

            if( it != mTasks.end() ){
                it->SetTitle( title );
                return true;
            }

            return false;
        } 
        
        //追加 idを発行
        int AddTask( const std::string &title)
        {
            const int id = GetNextId();
            mTasks.emplace_back( id, title );
            return id;
        } 
        
        //状態変更 成功時true 失敗時false
        bool SetTaskStatus( const int id, const Status status )
        {
            auto it = std::find_if(
                mTasks.begin(),
                mTasks.end(),
                [id]( const auto & a )
                {
                    return ( a.GetId() == id );
                }
            );

            if( it != mTasks.end() ){
                it->SetStatus( status );
                return true;
            }
            return false;
        } 

        //削除 成功時true 失敗時false
        bool RemoveTaskById( const int id)
        {
            auto newEnd = std::remove_if(
                mTasks.begin(),
                mTasks.end(),
                [id]( const auto &a)
                {
                    return (a.GetId() == id);
                }
            );

            if( newEnd != mTasks.end() ){
                mTasks.erase(newEnd,mTasks.end());
                return true;
            }
            return false;
        } 

        //検索（Id指定） 成功時 Task* 失敗時 nullptr
        const Task * FindTaskById( const int id ) const 
        {
            auto it = std::find_if(
                mTasks.begin(),
                mTasks.end(),
                [id]( const auto &a)
                {
                    return ( a.GetId() == id );
                }
            );
            if( it != mTasks.end() ){
                return &(*it);
            }
            return nullptr;
        }

        //Doneの状態のタスクを削除 削除した個数を返す
        int RemoveDoneTasks()
        {
            const auto newEnd = std::remove_if(
                mTasks.begin(),
                mTasks.end(),
                []( const auto &a )
                {
                    return (a.GetStatus() == Status::Done );
                }
            );
        
            int count = mTasks.end() - newEnd;
            
            mTasks.erase( newEnd, mTasks.end() );

            return count;
        } 
        
        //リストを取得
        const std::vector<Task> & GetTaskList()const
        {
            return mTasks;
        }

        //指定したステータスのタスクを取得
        std::vector<const Task*> GetTaskListByStatus( Status status ) const
        {
            std::vector<const Task*> list;
            for( const auto &a : mTasks ){
                if( a.GetStatus() == status ){
                    list.push_back(&a);
                }
            }
            return list;
        }

};

void DispTasks(const std::vector<Task>& tasks)
{
    std::cout<<"TaskList\n";
    for( const auto &a : tasks ){
        std::cout<<"  "<<a.GetTitle()<<" ";
        switch(a.GetStatus()){
            case Status::Doing: std::cout<<"Doing \n"; break;
            case Status::Done: std::cout<<"Done \n"; break;
            case Status::Todo: std::cout<<"Todo \n"; break;
        }
    }
    std::cout<<"\n";
}

void TestFunc2()
{
    TaskManager taskManager;

    int id0 = taskManager.AddTask("task1 ");
    int id1 = taskManager.AddTask("task2 ");
    int id2 = taskManager.AddTask("task3 ");

    DispTasks( taskManager.GetTaskList() );
    
    const Task *p = taskManager.FindTaskById( id1 );
    if( p ){
        std::cout <<"Find "<< p->GetTitle() << "\n";
    }
    taskManager.SetTitle( id0, "overwrite");
    DispTasks( taskManager.GetTaskList() );

    taskManager.SetTaskStatus( id0, Status::Done );
    DispTasks( taskManager.GetTaskList() );

    std::cout<<"DoneTaskList\n";
    std::vector<const Task*>taskList = taskManager.GetTaskListByStatus( Status::Done );
    for( auto t : taskList ){
        std::cout<< t->GetTitle() <<" \n";
    }

    taskManager.RemoveDoneTasks();
    DispTasks( taskManager.GetTaskList() );

    taskManager.RemoveTaskById( id2 );
    DispTasks( taskManager.GetTaskList() );
    
    return;
}
