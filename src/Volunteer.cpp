
#include <string>
#include <vector>
#include "Order.h"
#include "Volunteer.h"
#include <iostream>
using std::string;
using std::vector;

#define NO_ORDER -1

// Volunteer
Volunteer:: Volunteer(int id, const string &name): completedOrderId(NO_ORDER),activeOrderId(NO_ORDER),id(id),name(name){}
Volunteer:: ~Volunteer(){}
int Volunteer:: getId() const{return id;}
const  string  & Volunteer::getName() const{return name;}
int Volunteer::getActiveOrderId() const{return activeOrderId;}
int Volunteer::getCompletedOrderId() const{return completedOrderId;}

// Signal whether the volunteer is currently processing an order 
bool Volunteer:: isBusy() const{
        if (activeOrderId == -1) return false;
        return true;
}


//CollectorVolunteer

CollectorVolunteer::CollectorVolunteer(int id, const string &name, int coolDown):Volunteer(id,name),coolDown(coolDown), timeLeft(coolDown){}
CollectorVolunteer::~CollectorVolunteer(){}
CollectorVolunteer * CollectorVolunteer::clone() const {
        return new CollectorVolunteer(*this);
}
void CollectorVolunteer:: step(){
        if (isBusy()){
                if (decreaseCoolDown()){ // the volunteer finished his current order
                       completedOrderId = activeOrderId;
                       activeOrderId = NO_ORDER;
                }
        }
}
int CollectorVolunteer:: getCoolDown() const{return coolDown;}
int CollectorVolunteer::getTimeLeft() const{return timeLeft;}
bool CollectorVolunteer::decreaseCoolDown(){
        timeLeft--;
        if (timeLeft == 0) 
                return true; // the volunteer finished his current order
        return false;
}
bool CollectorVolunteer::hasOrdersLeft() const {return true;}
bool CollectorVolunteer::canTakeOrder(const Order &order) const {
        if (order.getStatus() == OrderStatus::PENDING){ // collector can take only pending orders
                if (!isBusy()) 
                        return true;
        }
        return false;
        
}
void CollectorVolunteer:: acceptOrder(const Order &order) {
        activeOrderId = order.getId(); // reset activeOrderId
        timeLeft = coolDown; // reset TimeLeft

}
string CollectorVolunteer::toString() const {
        string s ="";
       if (isBusy())
                s.append("timeLeft: " +std::to_string(getTimeLeft()) + "\n");
        s.append("ordersLeft: No Limit\n");
        return s;
}



// LimitedCollectorVolunteer
LimitedCollectorVolunteer::LimitedCollectorVolunteer(int id, const string &name, int coolDown ,int maxOrders):CollectorVolunteer(id,name,coolDown),maxOrders(maxOrders), ordersLeft(maxOrders){}
LimitedCollectorVolunteer::~LimitedCollectorVolunteer(){}
LimitedCollectorVolunteer * LimitedCollectorVolunteer::clone() const {
        return new LimitedCollectorVolunteer(*this);
}
bool LimitedCollectorVolunteer:: LimitedCollectorVolunteer::hasOrdersLeft() const{
        if (ordersLeft == 0)
                return false;
        return true;
}
bool LimitedCollectorVolunteer::canTakeOrder(const Order &order) const{
        if (CollectorVolunteer::canTakeOrder(order))
                if (hasOrdersLeft())
                        return true;
        return false;
}
void LimitedCollectorVolunteer::acceptOrder(const Order &order) {
        CollectorVolunteer::acceptOrder(order); // reset activeOrderId,TimeLeft
        ordersLeft--; // update OrdersLeft
}

int LimitedCollectorVolunteer::getMaxOrders() const{return maxOrders;}
int LimitedCollectorVolunteer::getNumOrdersLeft() const{return ordersLeft;}
string LimitedCollectorVolunteer::toString() const {
        string s ="";
        if (isBusy())
                s.append("timeLeft: " +std::to_string(getTimeLeft())+ "\n");
        s.append("ordersLeft: " +std::to_string(getNumOrdersLeft())+ "\n");
        return s;
}


//DriverVolunteer

DriverVolunteer::DriverVolunteer(int id, const string &name, int maxDistance, int distancePerStep):Volunteer(id,name),maxDistance(maxDistance), distancePerStep(distancePerStep),distanceLeft(maxDistance){}
DriverVolunteer::~DriverVolunteer(){}
DriverVolunteer * DriverVolunteer::clone() const{
        return new DriverVolunteer(*this);
} 

int DriverVolunteer::getDistanceLeft() const{return distanceLeft;}
int DriverVolunteer::getMaxDistance() const{return maxDistance;}
int DriverVolunteer::getDistancePerStep() const{return distancePerStep;}
bool DriverVolunteer::decreaseDistanceLeft(){
        distanceLeft -= distancePerStep;
        if (distanceLeft <= 0) // the driver reached his destination
        {
                distanceLeft = 0;
                return true;
        }
                
        return false;
}
bool DriverVolunteer::hasOrdersLeft() const {
        return true;
}

bool DriverVolunteer::canTakeOrder(const Order &order) const {
        if (order.getStatus() == OrderStatus::COLLECTING){ // driver can take only collected orders
                if (!isBusy() && (order.getDistance() <= maxDistance)) // not busy and the order is within his maxDistance
                        return true;
        }
        return false;
}
void DriverVolunteer::acceptOrder(const Order &order) {
    activeOrderId = order.getId(); // reset activeOrderId
    distanceLeft = order.getDistance(); // Reset distanceLeft to the order distance
} 
void DriverVolunteer::step() {
    if (isBusy()){
        if (decreaseDistanceLeft()){ // the volunteer finished his current order
                completedOrderId = activeOrderId;
                activeOrderId = NO_ORDER;
                }
        }
} 
string DriverVolunteer::toString() const {
         string s ="";
         if (isBusy())
                s.append("timeLeft: " +std::to_string(getDistanceLeft())+ "\n");
        s.append("ordersLeft: No Limit \n");

        return s;
}




//LimitedDriverVolunteer
LimitedDriverVolunteer::LimitedDriverVolunteer(int id, const string &name, int maxDistance, int distancePerStep,int maxOrders):DriverVolunteer(id,name,maxDistance,distancePerStep),maxOrders(maxOrders),ordersLeft(maxOrders){}
LimitedDriverVolunteer::~LimitedDriverVolunteer(){}
LimitedDriverVolunteer * LimitedDriverVolunteer::clone() const{
        return new LimitedDriverVolunteer(*this);
}
int LimitedDriverVolunteer:: getMaxOrders() const{return maxOrders;}
int LimitedDriverVolunteer::getNumOrdersLeft() const{return ordersLeft;}
bool LimitedDriverVolunteer::hasOrdersLeft() const{
        if (ordersLeft == 0)
            return false;
        return true;
}
bool LimitedDriverVolunteer::canTakeOrder(const Order &order) const{
        if (DriverVolunteer::canTakeOrder(order)){ // not busy and the order is within his maxDistance
               if (hasOrdersLeft())
                return true; 
        }
        return false;
}
void LimitedDriverVolunteer::acceptOrder(const Order &order){
        DriverVolunteer::acceptOrder(order);
        ordersLeft--;
}
string LimitedDriverVolunteer::toString() const{
        string s = "";
        if (isBusy())
                s.append("timeLeft: " + std::to_string(getDistanceLeft())+ "\n"); 
         s.append("ordersLeft: " + std::to_string(getNumOrdersLeft())+ "\n");

        return s;
}
