#include "Order.h"
#include <string>
#include <vector>
using std::string;
using std::vector;


    Order::Order(int id, int customerId, int distance):id(id),customerId(customerId),distance(distance),
        status(changeStatus("pending")), collectorId(NO_VOLUNTEER), driverId(NO_VOLUNTEER){}

    OrderStatus Order:: changeStatus (string s){
        if(s=="pending")
            return OrderStatus::PENDING;
        else if(s=="collecting")
            return OrderStatus::COLLECTING;
        else if(s== "delivering")
            return OrderStatus::DELIVERING;
        else
            return OrderStatus::COMPLETED;
    }
    int Order::getId() const{return id;}
    int Order::getCustomerId() const{return customerId;}
    int Order::getDistance() const{return distance;}
    void Order::setStatus(OrderStatus status){Order::status = status;}
    void Order::setCollectorId(int collectorId){Order::collectorId = collectorId;}
    void Order::setDriverId(int driverId){Order::driverId = driverId;}
    int Order::getCollectorId() const{return collectorId;}
    int Order::getDriverId() const{return driverId;}
    OrderStatus Order::getStatus() const{return status;}
    string Order::getStatusString() {
        OrderStatus check = getStatus(); 
        if(check == OrderStatus::COLLECTING)
            return "Collecting";
        else if(check == OrderStatus::COMPLETED)
             return "Completed";
        else if(check == OrderStatus::DELIVERING)
             return "Delivering";
        else return "Pending";
    }
    const string Order::toString() const{
        string s = "OrderId: " +std::to_string(getId()) + "\n";
        OrderStatus check = getStatus(); 
        if(check == OrderStatus::COLLECTING)
            s.append("OrderStatus: Collecting \n");
        else if(check == OrderStatus::COMPLETED)
            s.append("OrderStatus: Completed \n");
        else if(check == OrderStatus::DELIVERING)
            s.append("OrderStatus: Delivering \n");
        else 
            s.append("OrderStatus: Pending \n");
                
        s.append("CustomerID: " + std::to_string(getCustomerId())+"\n");

        if (getCollectorId() != -1)
            s.append("Collector: " + std::to_string(getCollectorId())+ "\n");
        else 
            s.append("Collector: None \n");
        if (getDriverId() != -1)
            s.append("Driver: " + std::to_string(getDriverId())+ "\n");
        else 
            s.append("Driver: None \n");

        return s;
    }
