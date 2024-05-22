
#include <string>
#include "Customer.h"
#include <vector>
using std::string;
using std::vector;




Customer::Customer(int id, const string &name, int locationDistance, int maxOrders):id(id),name(name),locationDistance(locationDistance),maxOrders(maxOrders),ordersId(){}
const string  & Customer::getName() const{
        return name;
}

Customer:: ~Customer(){}

int Customer:: getId() const{
        return id;
}
    
int Customer:: getCustomerDistance() const{
        return locationDistance;
}

int Customer:: getMaxOrders() const{
        return maxOrders;
} //Returns maxOrders
    
int Customer:: getNumOrders() const{
        return ordersId.size();
} //Returns num of orders the customer has made so far

bool Customer:: canMakeOrder() const{ //Returns true if the customer didn't reach max orders
        int size = ordersId.size();
        if (size >= maxOrders)
                return false;
        return true;
} 



const vector<int> & Customer::getOrdersIds() const{
        return ordersId;
}

int Customer:: addOrder(int orderId){ //return OrderId if order was added successfully, -1 otherwise
        if (canMakeOrder()){
                ordersId.push_back(orderId);
                return orderId;
        }
         return -1;       
}




SoldierCustomer:: SoldierCustomer(int id, const string &name, int locationDistance, int maxOrders):Customer(id,name,locationDistance,maxOrders){}
SoldierCustomer * SoldierCustomer::clone() const {
        return new SoldierCustomer(*this);
}
SoldierCustomer:: ~SoldierCustomer(){}



    
CivilianCustomer:: CivilianCustomer(int id, const string &name, int locationDistance, int maxOrders):Customer(id,name,locationDistance,maxOrders){}
CivilianCustomer * CivilianCustomer::clone() const {
        return new CivilianCustomer(*this);
}
CivilianCustomer:: ~CivilianCustomer(){}
