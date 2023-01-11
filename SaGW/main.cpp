#include <iostream>
#include <string>
#include <fstream>
#include <list>
#include <vector>
#include <sstream>
#include <map>


using namespace std;

class Entity { // интерфейс сущность
public:
    virtual void printAsCSV(ostream& stream) = 0;  // один виртуальный абстрактный метод, который будет в нужном формате печатать сущность в аутпут стрим

};

class Customer : public Entity {  // Заказчик
private:
    string _name;          // с именем
    string _surname;       // фамилией
    string _phoneNumber;   // и номером телефона
public:
    Customer()  = default;  // обычный пустой конструктор

    // Гетеры и сеттеры так как это все же сущность
    const string &getName() const {
        return _name;
    }

    void setName(const string &name) {
        _name = name;
    }

    const string &getSurname() const {
        return _surname;
    }

    void setSurname(const string &surname) {
        _surname = surname;
    }

    const string &getPhoneNumber() const {
        return _phoneNumber;
    }

    void setPhoneNumber(const string &phoneNumber) {
        _phoneNumber = phoneNumber;
    }

    void printAsCSV(ostream& stream) override {  // оверрайднем наш метод собственно от интерфейса
        stream << _name << "," << _surname << "," << _phoneNumber;
    }

};

class Product : public Entity { // Собственно здесь все то же самое, только поля другие
private:
    string _name;
    string _features;
    int _price;
public:
    Product() = default;

    const string &getName() const {
        return _name;
    }

    void setName(const string &name) {
        _name = name;
    }

    const string &getFeatures() const {
        return _features;
    }

    void setFeatures(const string &features) {
        _features = features;
    }

    int getPrice() const {
        return _price;
    }

    void setPrice(int price) {
        _price = price;
    }

    void printAsCSV(ostream& stream) override {
        stream << _name << "," << _features << "," << _price;
    }

};


class Order : public Entity { // Собственно здесь все то же самое, только поля другие и цсв аутпут лучше
private:
    Customer* _customer;
    Product* _product;
    int _count;
public:
    Order() = default;

    const Customer* getCustomer() const {
        return _customer;
    }

    void setCustomer(Customer* customer) {
        _customer = customer;
    }

    const Product* getProduct() const {
        return _product;
    }

    void setProduct(Product* product) {
        _product = product;
    }

    int getCount() const {
        return _count;
    }

    void setCount(int count) {
        _count = count;
    }

    void printAsCSV(ostream& stream) override {
        _customer->printAsCSV(stream);
        stream << ",";
        _product->printAsCSV(stream);
        stream << "," << _count;
    }

    ~Order() {
        delete _customer;
        delete _product;
    }
};

class Creator {
public:
    virtual Entity* create(vector<string> params) = 0;

};

class CustomerCreator : public Creator {
public:
    Customer* create(vector<string> params) override {
        auto* customer = new Customer();
        customer->setName(params[0]);
        customer->setSurname(params[1]);
        customer->setPhoneNumber(params[2]);
        return customer;
    }

};

class ProductCreator : public Creator {
public:
    Product* create(vector<string> params) override {
        auto* product = new Product();
        product->setName(params[0]);
        product->setFeatures(params[1]);
        product->setPrice(stoi(params[2]));
        return product;
    }

};

class OrderCreator : public Creator {
    Order* create(vector<string> params) override {
        auto* order = new Order;
        order->setCount(stoi(params[0]));
        return order;
    }

};

class OrderList {
private:
    list<Order*> _list;
public:
    void addEntity(Order* order) {
        _list.push_back(order);
    }

    void deleteOrderByPosition(int pos) {
        if (_list.size() < pos || pos < 1) {
            cout << "Неверная позиция" << endl;
            return;
        }
        auto it = _list.begin();
        for (int i = 0; i < pos; i++, it++);
        _list.erase(it);
    }

    void printList(ostream& stream, bool isIndexed = true) {
        int i = 1;
        for (Order* order: _list) {
            if (isIndexed) stream << i++ << ") ";
            order->printAsCSV(stream);
            stream << endl;
        }
    }

    void readOrdersFromFile(ifstream& stream) {

        map<int, Creator*> creatorMap;
        creatorMap.insert({2, new CustomerCreator()});
        creatorMap.insert({5, new ProductCreator()});
        creatorMap.insert({6, new OrderCreator()});
        string line;
        while (getline(stream, line)) {

            istringstream iss(line);
            string word;
            int i = 0;
            vector<string> stringVector;
            Customer* customer;
            Product* product;

            while (getline(iss, word, ',')) {
                stringVector.push_back(word);
                switch (i) {
                    case 2:
                        customer = dynamic_cast<Customer *>(creatorMap[i]->create(stringVector));
                        stringVector.clear();
                        break;
                    case 5:
                        product = dynamic_cast<Product *>(creatorMap[i]->create(stringVector));
                        stringVector.clear();
                        break;
                    case 6:
                        Order* order = dynamic_cast<Order *>(creatorMap[i]->create(stringVector));
                        order->setCustomer(customer);
                        order->setProduct(product);
                        _list.push_back(order);
                        break;
                }
                i++;


            }

        }
        for (pair<int, Creator*> p : creatorMap) {
            delete p.second;
        }
    }

    void clearList() {
        for (Order* order : _list) {
            delete order;
        }
        _list.clear();
    }

    bool empty() {
        return _list.empty();
    }

};

void createOrder(OrderList* orderList) {
    string nameC, surname, phone;
    cout << "Имя заказчика: "; cin >> nameC;
    cout << "Фамилия заказчика: "; cin >> surname;
    cout << "Номер заказчика: "; cin >> phone;
    Customer* customer = new Customer();
    customer->setName(nameC);
    customer->setSurname(surname);
    customer->setPhoneNumber(phone);
    string nameP, feature, price;
    cout << "Имя продукта: "; cin >> nameP;
    cout << "Особенности: "; cin >> feature;
    cout << "Цена: "; cin >> price;
    Product* product = new Product();
    product->setName(nameP);
    product->setFeatures(feature);
    product->setPrice(stoi(price));
    string count;
    cout << "Количество: "; cin >> count;
    Order* order = new Order();
    order->setCustomer(customer);
    order->setProduct(product);
    order->setCount(stoi(count));
    orderList->addEntity(order);
}

int main() {
    setlocale(LC_ALL, "Russian");
    OrderList orderList;
    ifstream ifile("data.csv");
    if (ifile.is_open()){
        orderList.readOrdersFromFile(ifile);
    }
    else
        cout << "Файл с данными не найден! Начальных данных нет" << endl;
    ifile.close();

    char choice = '0';
    string a;							// переменная для того, чтобы "съедать" символ новой строки

    while (choice != '4')				// реализация меню
    {
        cout << endl << "Выберите, что вы хотите сделать : " << endl;
        cout << "_________________________" << endl
             << "1. Создать заказ."	<< endl
             << "2. Просмотреть список заказов." << endl
             << "3. Удалить заказ." << endl
             << "4. Выйти из программы."	<< endl
             << "_________________________";
        cout << endl << "Ваш выбор:";	cin >> choice;
        getline(cin, a, '\n');

        switch (choice)
        {
            case '1':
                createOrder(&orderList);
                break;
            case '2':
                if (orderList.empty()) {
                    cout << "Пусто" << endl;
                    break;
                }
                orderList.printList(cout);
                break;
            case '3':
                if (orderList.empty()) {
                    cout << "Нечего удалять" << endl;
                    break;
                }
                orderList.printList(cout);
                int pos;
                cout << "Введите номер строки, которую нужно удалить: "; cin >> pos;
                orderList.deleteOrderByPosition(pos);
                break;
        }
    }
    ofstream ofile("data.csv");
    if (ofile.is_open())
        orderList.printList(ofile, false);
    else
        cout << "Файл для сохранения не открывается, сохранение не будет произведено" << endl;
    ofile.close();
}
