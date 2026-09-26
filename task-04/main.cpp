#include <string>
#include <iostream>
#include <utility>
#include <memory>

class Data{
    private:
        std::string mName;
        int mId;
    public:
        const std::string & GetName() const{ return mName; }
        Data( int id, const std::string& name )
        {
            mId = id;
            std::cout<<"string lvalue constructor\n";
            mName = name;
        }
        Data( int id, const Data &other )
        {
            mId = id;
            std::cout<<"data copy constructor\n";
            mName = other.mName;
        }

        Data( int id, std::string&& name )
        {
            mId = id;
            std::cout<<"string rvalue constructor\n";
            mName = std::move(name);
        }

};

void Receive( const Data& data )
{
    std::cout<<"Receive lvalue " << data.GetName() << "\n";
}

void Receive( Data&& data )
{
    std::cout<<"Receive rvalue " << data.GetName() << "\n";
}

template<class T>
void ForwardTest( T&& value )
{
    Receive( std::forward<T>(value) );
};

//...は繰り返したい式につける
template< class... Args >
Data MakeData( Args&&... args )
{
    return Data( std::forward<Args>(args)... );
}

template<class T, class... Args>
std::unique_ptr<T>MyMakeUnique(Args&&... args)
{
    return std::unique_ptr<T>( new T( std::forward<Args>( args )... ) );
}

int main()
{
    std::cout <<"task04\n";
    std::string s ="A";
    Data a = MakeData(0,s);

    Data b = MakeData(1,std::move(s));

    std::string u = "U";
    auto p1 = MyMakeUnique<Data>( 2, u );
    auto p2 = MyMakeUnique<Data>( 3, std::move(u) );
    
    ForwardTest(a);
    ForwardTest(std::move(a));
    return 0;
}
