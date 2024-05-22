
#include <string>
#include <vector>
#include <iostream>
#include "WareHouse.h"
#include "Action.h"
#include "Order.h"
#include "Customer.h"
#include "Volunteer.h"
using std::string;

extern WareHouse* backup;

Action::Action(): errorMsg(""), status(ActionStatus::ERROR){}
ActionStatus Action:: getStatus() const{
        return status;
}
void Action::complete(){
        status = ActionStatus::COMPLETED;
}
void Action::error(string errorMsg){
        Action::errorMsg = errorMsg;
        status = ActionStatus::ERROR;
        std::cout << "Error: " + errorMsg  <<  std::endl;
}
string Action::getErrorMsg() const{
        return errorMsg;
}


//SimulateStep functions

SimulateStep::SimulateStep(int numOfSteps):numOfSteps(numOfSteps){} //constructor
//SimulateStep::SimulateStep(const SimulateStep& other){}//copy constructor
SimulateStep:: ~SimulateStep(){} //distructor
void SimulateStep::act(WareHouse &wareHouse){
        Volunteer* volunteer = nullptr;
        vector<Order*> pendingOrders = wareHouse.getPendingOrders();
        vector<Order*> inProcessOrders = wareHouse.getInProcessOrders();
        vector<Order*> completedOrders = wareHouse.getCompletedOrders();
        vector<Volunteer*> volunteers = wareHouse.getvolunteers();
        for (int step=0; step<numOfSteps; step++){
                pendingOrders = wareHouse.getPendingOrders();
                inProcessOrders = wareHouse.getInProcessOrders();
                completedOrders = wareHouse.getCompletedOrders();
                volunteers = wareHouse.getvolunteers();
        // stage 1 in the scheme
                for (Order* order: pendingOrders) //loop over pending vector orders
                {
                        bool volunteerFound = false;
                        if ((order->getStatus() == OrderStatus::PENDING) | (order->getStatus() == OrderStatus::COLLECTING)){ //status: pending or collecting, waits 
                                for (unsigned int j = 0 ; !volunteerFound && j < volunteers.size() ; j++ ){ // while we didnt find a volunteer and there are still more volunteer
                                        volunteer = volunteers.at(j);
                                        if (volunteer->canTakeOrder(*order)){ // found a volunteer
                                                volunteer->acceptOrder(*order);
                                                volunteerFound = true;
                                                wareHouse.setOrderDetails(order->getId(),volunteer->getId());

                                                // move order to next orders vector
                                                wareHouse.pendingToInProcces(wareHouse.getIndexOrder(order->getId()),order->getId());
                                        }       
                                }

                        }
                        
                }

                for (unsigned int i = 0 ; i < volunteers.size() ; i++ ){
                        inProcessOrders = wareHouse.getInProcessOrders(); // update the vector according to changes in warehouse
                         volunteer = volunteers.at(i);
                        if (volunteer->isBusy()){ //processing an order
                                Order activeOrder = wareHouse.getOrder(volunteer->getActiveOrderId());
                                // stage 2 in the scheme
                                volunteer->step();
                                if (!volunteer->isBusy()){ // finished processing the order from earlier
                                        // stage 3 in the scheme
                                        bool found = false;
                                        for (unsigned int i=0; !found && i<inProcessOrders.size(); i++){
                                                if (inProcessOrders.at(i)->getId() == activeOrder.getId()){
                                                        if (activeOrder.getStatus() == OrderStatus::COLLECTING){
                                                                wareHouse.InProccesToPending(i,activeOrder.getId());           
                                                        }
                                                        else if (activeOrder.getStatus() == OrderStatus::DELIVERING){
                                                                wareHouse.InProccesToComplete(i,activeOrder.getId());
                                                        }
                                                }
                                        }
                                        // stage 4 in the scheme
                                        if (!volunteer->hasOrdersLeft()){ // reached his maxOrder limit and finished his last order
                                                wareHouse.deleteVolunteer(volunteer->getId());
                                        }
                                }
                        }   
                }
        }
        this->complete();
}
std::string SimulateStep::toString() const {
        return "SimulateStep "+ std::to_string(numOfSteps)  + " COMPLETED";
}
SimulateStep * SimulateStep::clone() const {
        return new SimulateStep(*this);
}




//AddOrder functions

AddOrder::AddOrder(int id):customerId(id){}

AddOrder:: ~AddOrder(){}

void AddOrder:: act(WareHouse &wareHouse){
        if(customerId < wareHouse.getCustomerCounter() ) // check if this customer exists
        {
                Customer &c = wareHouse.getCustomer(customerId); // reference to the customer
                int check = c.addOrder(wareHouse.getOrderID()); // add the new order to the customer's orders vector (if possible)
                if (check == -1)// check if passed his limit
                        error("Cannot place this order");
                else {
                        Order* newOrder = new Order(wareHouse.getOrderID(),customerId, c.getCustomerDistance()); // create new order
                        wareHouse.addOrder(newOrder); // add the new order to pendingOrders vector
                        this->complete();
                }
        }
        else
        {
             error("Cannot place this order");   
        }
}
string AddOrder::toString() const{
        if(ActionStatus::COMPLETED == getStatus())
                return "order " + std::to_string(customerId) + " COMPLETED";
        else
                return "order " + std::to_string(customerId) + " ERROR";
}

AddOrder * AddOrder::clone() const{
        return new AddOrder(*this);

}


//AddCustomer functions

AddCustomer::AddCustomer(const string &customerName, const string &customerType, int distance, int maxOrders)
:customerName(customerName), customerType(createType(customerType)),distance(distance),maxOrders(maxOrders),customerType_string(customerType){}

AddCustomer:: ~AddCustomer(){}

//private function that helps me change the string value into CustomerType using enum.
CustomerType AddCustomer::createType(string customerType) {
        if(customerType == "soldier"){
                return CustomerType::Soldier;
        }
        else
                return CustomerType::Civilian;
}

void AddCustomer::act(WareHouse &wareHouse){
        Customer* newCustomer = nullptr;
        if(CustomerType::Civilian == customerType)
                newCustomer = new CivilianCustomer(wareHouse.getCustomerCounter(), customerName,distance,maxOrders); // create new customer
        else
                newCustomer = new SoldierCustomer(wareHouse.getCustomerCounter(), customerName,distance,maxOrders);
        wareHouse.addCustomer(newCustomer); // add the new customer to customers vector
        this->complete();

}
AddCustomer * AddCustomer::clone() const{
        return new AddCustomer(*this);

}
string AddCustomer::toString() const{

        return "customer " + customerName +" " +customerType_string+" " + std::to_string(distance) +" " +std::to_string(maxOrders) +"  COMPLETED";
}




//PrintOrderStatus functions

PrintOrderStatus::PrintOrderStatus(int id):orderId(id){}
PrintOrderStatus:: ~PrintOrderStatus(){}
void PrintOrderStatus::act(WareHouse &wareHouse){
        int checkId = wareHouse.getOrderID();
        if(orderId < checkId)
        {
                Order order = wareHouse.getOrder(orderId);
                 std::cout <<order.toString()<< std::endl;
                this->complete();
        }
        else
                error("Order doesn't exist");
}
PrintOrderStatus * PrintOrderStatus::clone() const{
        return new PrintOrderStatus(*this);
}
string PrintOrderStatus::toString() const{
        if(ActionStatus::COMPLETED == getStatus())
                return "orderStatus " + std::to_string(orderId) + " COMPLETED";
        else
                return "orderStatus " + std::to_string(orderId) + " ERROR";
}
    

//PrintCustomerStatus functions

PrintCustomerStatus::PrintCustomerStatus(int customerId):customerId(customerId){}
PrintCustomerStatus:: ~PrintCustomerStatus(){}

void PrintCustomerStatus::act(WareHouse &wareHouse){
        int checkId = wareHouse.getCustomerCounter(); 
        if(customerId < checkId)
        {
                Customer &customer = wareHouse.getCustomer(customerId);
                // print CustomerId
                std::cout <<"CustomerId: " +std::to_string(customerId)<< std::endl;
        
                // for each of his orders, print OrderId & OrderStatus
                vector<int> orders = customer.getOrdersIds();
                for(unsigned int i = 0 ; i < orders.size(); i++)
                {
                        Order order = wareHouse.getOrder(orders.at(i));

                        // print OrderId
                        std::cout <<"OrderID: " + std::to_string(order.getId()) << std::endl;

                        // print OrderStatus
                        std::cout <<"OrderStatus: " + order.getStatusString() << std::endl;

                }

                // print numOrdersLeft (max order - orders that the customer has done)
                std::cout <<"numOrdersLeft: " +std::to_string((customer.getMaxOrders() - customer.getNumOrders()))<< std::endl;
                std::cout <<" "<< std::endl;
                this->complete();
        }
        else
                error("Customer doesn't exist");
}
PrintCustomerStatus * PrintCustomerStatus::clone() const{
        return new PrintCustomerStatus(*this);

}
string PrintCustomerStatus::toString() const{
        if(ActionStatus::COMPLETED == getStatus())
                return "customrStatus " + std::to_string(customerId) + " COMPLETED";
        else
                return "customrStatus " + std::to_string(customerId) + " ERROR";
}




//PrintVolunteerStatus functions
    
PrintVolunteerStatus::PrintVolunteerStatus(int id):volunteerId(id){}

PrintVolunteerStatus:: ~PrintVolunteerStatus(){}
void PrintVolunteerStatus::act(WareHouse &wareHouse){
        int id = wareHouse.getIndexVolunteer(volunteerId);

        if( id != -1)
        {

                Volunteer &volunteer = wareHouse.getVolunteer(volunteerId); //we checked in the function if exist if not return -1


                // print volunteerId
                std::cout <<"VolunteerId: " + std::to_string(volunteerId)<< std::endl;

                //print isBusy
                std::cout <<std::boolalpha<<"isBusy: " << (volunteer.isBusy())<< std::endl;


                //print orderID, timeLeft, ordersLeft
                if(!volunteer.isBusy()){ // doesnt process any order
                        std::cout <<"OrderID: None "<< std::endl;
                        std::cout <<"timeLeft: None "<< std::endl;
                }
                else 
                       std::cout <<"OrderID: " + std::to_string(volunteer.getActiveOrderId())<< std::endl;
                                    
                // print specific information dut to type
                std::cout <<volunteer.toString()<< std::endl;                
                this->complete();
        }
        else
                error("Volunteer doesn't exist");
}


PrintVolunteerStatus * PrintVolunteerStatus::clone() const{
        return new PrintVolunteerStatus(*this);

}
string PrintVolunteerStatus::toString() const{
        if(ActionStatus::COMPLETED == getStatus())
                return "volunteerStatus " + std::to_string(volunteerId) + " COMPLETED";
        else
                return "volunteerStatus " + std::to_string(volunteerId) + " ERROR";
}


//PrintActionsLog functions

PrintActionsLog::PrintActionsLog(){}
PrintActionsLog:: ~PrintActionsLog(){}
void PrintActionsLog:: act(WareHouse &wareHouse){
        vector<Action*> actions = wareHouse.getActions();
        int size = actions.size();
        for(int i = 0; i<size ; i++)
        {
                std::cout << actions.at(i)->toString() << std::endl;
        }
        std::cout <<""<< std::endl;
        this->complete();


}
PrintActionsLog * PrintActionsLog::clone() const {
        return new PrintActionsLog(*this);

}
string PrintActionsLog::toString() const{
        return "log COMPLETED";
}


//Close functions

Close::Close(){}
Close::~Close(){}
void Close::act(WareHouse &wareHouse){
        vector<Order*> pendingOrders = wareHouse.getPendingOrders();
        vector<Order*> inProcessOrders = wareHouse.getInProcessOrders();
        vector<Order*> completedOrders = wareHouse.getCompletedOrders();
        for(Order* o : pendingOrders)
                std::cout << "OrderID: " + std::to_string(o->getId()) + " CustomerID: " + std::to_string(o->getCustomerId()) + + " OrderStatus: " + o->getStatusString() << std::endl;
        for(Order* o : inProcessOrders)
                std::cout << "OrderID: " + std::to_string(o->getId()) + " CustomerID: " + std::to_string(o->getCustomerId()) + + " OrderStatus: " + o->getStatusString() << std::endl;

        for(Order* o :completedOrders)
                std::cout << "OrderID: " + std::to_string(o->getId()) + " CustomerID: " + std::to_string(o->getCustomerId()) + + " OrderStatus: " + o->getStatusString() << std::endl;

        wareHouse.close();
        this->complete();
}
string Close::toString() const{
        return "";
}
Close * Close::clone() const{
        return new Close(*this);

}


//BackupWareHouse functions

BackupWareHouse:: BackupWareHouse(){}
BackupWareHouse:: ~BackupWareHouse(){}
void BackupWareHouse:: act(WareHouse &wareHouse){
        if (backup != nullptr)
                *backup = wareHouse;
        else
                backup = new WareHouse(wareHouse);
        this->complete();
}
 BackupWareHouse * BackupWareHouse::clone() const{
        return new BackupWareHouse(*this);
            
}
string BackupWareHouse:: toString() const{
        return "backup COMPLETED";
}


//RestoreWareHouse functions

RestoreWareHouse:: RestoreWareHouse(){}
RestoreWareHouse:: ~RestoreWareHouse(){}
void RestoreWareHouse:: act(WareHouse &wareHouse){
         if (backup == nullptr){
                 error("No backup available");
                 return;
         }
        wareHouse = *backup;
        this->complete();
        
}
 RestoreWareHouse * RestoreWareHouse::clone() const{
        return new RestoreWareHouse(*this);

}
string RestoreWareHouse:: toString() const{
        
        if(ActionStatus::COMPLETED == getStatus())
                return "restore COMPLETED";
        else
                return "restore ERROR";
}
