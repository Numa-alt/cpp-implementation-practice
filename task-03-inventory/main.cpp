#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

// 商品
class Product
{
private:
    std::string mCode;
    std::string mName;
    int mPrice;
    int mStock;

public:
    Product(const std::string &code, const std::string &name, int price, int stock) : mCode(code),
                                                                                      mName(name), mPrice(price), mStock(stock) {}

    const std::string &GetCode() const { return mCode; }
    const std::string &GetName() const { return mName; }
    int GetPrice() const { return mPrice; }
    int GetStock() const { return mStock; }

    bool StockOut(int num)
    {
        if (num <= 0)
        {
            // 不正な個数を出庫しようとした
            return false;
        }
        if (num > mStock)
        {
            // 在庫以上に出庫しようとした
            return false;
        }

        mStock -= num;

        return true;
    }

    bool StockIn(int num)
    {
        if (num <= 0)
        {
            // 不正な個数を入庫しようとした
            return false;
        }
        mStock += num;
        return true;
    }

    long long CalcTotalPrice() const
    {
        return static_cast<long long>(mStock) * mPrice;
    }
};

class Inventory
{
private:
    std::vector<Product> mProducts;

    // 商品検索
    // 商品コードで商品を検索する
    // 存在しない場合はnullptrを返す
    Product *ReadWrite(const std::string &code)
    {
        auto it = std::find_if(
            mProducts.begin(),
            mProducts.end(),
            [&code](const auto &p)
            {
                return (p.GetCode() == code);
            });

        if (it != mProducts.end())
        {
            return &(*it);
        }
        return nullptr;
    }

public:
    // 商品登録
    // 以下の場合は失敗する
    // 同じ商品コードが存在する、単価が０以下、初期在庫が０未満
    bool RegisterProduct(const std::string &code, const std::string &name, int price, int stock)
    {
        const Product *p = FindProduct(code);

        if (p)
        {
            // 重複する商品コード
            return false;
        }

        if (price <= 0)
        {
            // 不正な価格
            return false;
        }

        if (stock < 0)
        {
            // 不正な在庫数
            return false;
        }

        mProducts.emplace_back(code, name, price, stock);

        return true;
    }

    // 商品検索
    // 商品コードで商品を検索する
    // 存在しない場合はnullptrを返す
    const Product *FindProduct(const std::string &code) const
    {
        const auto it = std::find_if(
            mProducts.begin(),
            mProducts.end(),
            [&code](const auto &p)
            {
                return (p.GetCode() == code);
            });

        if (it != mProducts.end())
        {
            return &(*it);
        }
        return nullptr;
    }

    // 入庫
    // 以下の場合は失敗する
    // 商品が存在しない、数量が０以下
    bool StockIn(const std::string &code, int num)
    {
        auto p = ReadWrite(code);

        if (p == nullptr)
        {
            // 商品コードが見つからない
            return false;
        }

        return p->StockIn(num);
    }

    // 出庫
    // 以下の場合は失敗する
    // 商品が存在しない
    // 数量が０以下
    // 現在庫より多く出庫しようとした
    // 失敗した場合在庫数は変更しない
    bool StockOut(const std::string &code, int num)
    {
        auto p = ReadWrite(code);
        if (p == nullptr)
        {
            // 商品が存在しない
            return false;
        }

        return p->StockOut(num);
    }

    // 在庫切れの商品の取得
    // もと商品の順番を変更しないこと
    std::vector<const Product *> GetOutOfStockList() const
    {
        std::vector<const Product *> list;

        for (const auto &p : mProducts)
        {
            if (p.GetStock() <= 0)
            {
                list.push_back(&p);
            }
        }
        return list;
    }

    // 全商品取得
    // 外部から商品を直接変更できないように
    const std::vector<Product> &GetList() const
    {
        return mProducts;
    }

    // 在庫総額
    long long TotalPrice() const
    {
        long long totalPrice = 0;
        for (const auto& p : mProducts)
        {
            totalPrice += p.CalcTotalPrice();
        }

        return totalPrice;
    }
};

void DisplayInventory(const std::vector<Product> list)
{
    std::cout << "inventory" << "\n";
    for (const auto &p : list)
    {
        std::cout << "    " << p.GetCode() << " " << p.GetName() << " " << p.GetPrice() << " x " << p.GetStock() << "\n";
        ;
    }
    std::cout << "\n";
}

void DisplayInventory( const std::vector<const Product*> list)
{
    std::cout << "inventory" << "\n";
    for (const auto &p : list)
    {
        std::cout << "    " << p->GetCode() << " " << p->GetName() << " " << p->GetPrice() << " x " << p->GetStock() << "\n";
        ;
    }
    std::cout << "\n";
}

int main()
{
    Inventory inventory;

    inventory.RegisterProduct("A001", "Pen", 120, 10);
    inventory.RegisterProduct("B001", "Notebook", 20, 5);
    bool r0 = inventory.RegisterProduct("C001", "Eraser", 100, 0);
    std::cout << "register C001 in 0 result " << r0 << "\n";
    inventory.RegisterProduct("D001", "Book", 220, 30);

    bool sameId = inventory.RegisterProduct("B001","Note",20,10);
    std::cout<<"sameId register "<< sameId << "\n";

    bool zeroPrice = inventory.RegisterProduct("E001","Note",0,10);
    std::cout<<"zeroPrice register "<< zeroPrice << "\n";

    bool minusStock = inventory.RegisterProduct("F001","Note",10,-10);
    std::cout<<"minusStock register "<< minusStock << "\n";

    
    DisplayInventory(inventory.GetList());
    std::cout << "TotalPrice " << inventory.TotalPrice() << "\n";

    std::cout << "A001 in 20\n";
    bool b0 = inventory.StockIn("A001", 20);
    std::cout << "reuslt " << b0 << "\n";
    DisplayInventory(inventory.GetList());
    std::cout << "TotalPrice " << inventory.TotalPrice() << "\n";

    std::cout << "B001 out 10";
    bool b1 = inventory.StockOut("B001", 10);
    std::cout << "result " << b1 << "\n";
    DisplayInventory(inventory.GetList());
    std::cout << "TotalPrice " << inventory.TotalPrice() << "\n";

    std::cout << "C001 out 1\n";
    bool c001out = inventory.StockOut("C001", 1);
    std::cout << "result " << c001out << "\n";

    DisplayInventory(inventory.GetList());
    std::cout << "TotalPrice " << inventory.TotalPrice() << "\n";

    //存在しない商品への入庫
    bool notExistProductStockIn = inventory.StockIn("H001",10);
    std::cout<<"notExistProductStockIn " << notExistProductStockIn << "\n";

    //在庫に０個追加
    bool zeroStockIn = inventory.StockIn("A001",0);
    std::cout<<"zeroStockIn "<<zeroStockIn<<"\n";

    //在庫数を超える出庫
    bool overStockOut = inventory.StockOut("A001",500);
    std::cout<<"overStockOut "<<overStockOut<<"\n";

    DisplayInventory( inventory.GetOutOfStockList() );

    return 0;
}