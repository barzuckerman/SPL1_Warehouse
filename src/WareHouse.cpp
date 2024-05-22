#include "Action.h"
#include "WareHouse.h"
#include "Volunteer.h"
#include <iostream>
#include <fstream> //for reading the file
class Action;
class Volunteer;

// Warehouse responsible for Volunteers, Customers Actions, and Orders.

    string WareHouse:: separate(string& input){
        string cut="";
        unsigned int  i; 
        for (i=0; i<input.length() && input.at(i)!=' '; i++)
            cut+=input.at(i);
        input.erase(0,i+1);

        return cut;

    }
    

    WareHouse:: WareHouse(const string &configFilePath):isOpen(false),actionsLog(),volunteers(),pendingOrders(),inProcessOrders(),completedOrders(),customers(),customerCounter(0),volunteerCounter(0),orderCounter(0){
        std::ifstream file(configFilePath);
        if (file.is_open()) {
            string line;
            while (std::getline(file,line))
            {
                if (line.empty() || line[0] == '#') {
                    continue;
                }
                std::string keyword;
                keyword = separate(line);


                if (keyword == "customer") {
                    Customer* newCustomer;
                    string name = separate(line);
                    string type = separate(line);
                    string dist = separate(line);
                    string maxOrders = separate(line);
                    if(type == "soldier")
                        newCustomer = new SoldierCustomer(customerCounter, name, stoi(dist), stoi(maxOrders));
                    else 
                        newCustomer = new CivilianCustomer(customerCounter, name, stoi(dist), stoi(maxOrders));
                    addCustomer(newCustomer);
                }
                
                else if (keyword == "volunteer") {
                    Volunteer* newVolunteer;
                    string name = separate(line);
                    string role = separate(line);
                    if(role == "collector"){
                        string coolDown = separate(line);
                        newVolunteer = new CollectorVolunteer(volunteerCounter,name,stoi(coolDown));
                    }
                    else if(role == "limited_collector"){
                        string coolDown = separate(line);
                        string maxOrders = separate(line);
                        newVolunteer = new LimitedCollectorVolunteer(volunteerCounter,name,stoi(coolDown), stoi(maxOrders));
                    }
                    else if (role == "driver") {
                        string maxDistance = separate(line);
                        string distancePerStep = separate(line);
                        newVolunteer = new DriverVolunteer(volunteerCounter, name, stoi(maxDistance), stoi(distancePerStep));
                   }
                    else
                    {
                        string maxDistance = separate(line);
                        string distancePerStep = separate(line);
                        string maxOrders = separate(line);
                        newVolunteer = new LimitedDriverVolunteer(volunteerCounter, name, stoi(maxDistance), stoi(distancePerStep), stoi(maxOrders));
                    }
                    addVolunteer(newVolunteer);
                     
                } 
            }


        }
            

            file.close();
    } 
    
    //copy constructor
    WareHouse:: WareHouse(const WareHouse& other): isOpen(other.isOpen), actionsLog(), volunteers(), pendingOrders(), inProcessOrders(), completedOrders(), customers(), customerCounter(other.customerCounter),volunteerCounter(other.volunteerCounter) , orderCounter(other.orderCounter){
        
        for(Customer* customer : other.customers){
            if (customer != nullptr)
                this->customers.push_back(customer->clone());
        }
        for(Volunteer* volunteer : other.volunteers){
            if (volunteer != nullptr)
                this->volunteers.push_back(volunteer->clone());
        }
        for(Action* action :other.actionsLog){
            this->actionsLog.push_back(action->clone());
        }
        for (Order* order: other.pendingOrders)
        {
            this->pendingOrders.push_back(new Order(*order));
        }
        for (Order* order: other.inProcessOrders)
        {
            this->inProcessOrders.push_back(new Order(*order));
        }
        for (Order* order: other.completedOrders)
        {
            this->completedOrders.push_back(new Order(*order));
        }

    }

    WareHouse::~WareHouse(){
        for(Customer* customer : customers){
            if(customer != nullptr){
                delete customer;
            }
        }
        customers.clear();

        for(Volunteer* volunteer : volunteers){
            if(volunteer != nullptr){
                delete volunteer;
            }
        }
        volunteers.clear();

        for(Action* action : actionsLog){
            if(action != nullptr){
                delete action;
            }
        }
        actionsLog.clear();

        for(Order* order : pendingOrders){
            if(order != nullptr){
                delete order;
            }
        }
        pendingOrders.clear();

        for(Order* order : inProcessOrders){
            if(order != nullptr){
                delete order;
            }
        }
        inProcessOrders.clear();

        for(Order* order : completedOrders){
            if(order != nullptr){
                delete order;
            }
        }
        completedOrders.clear();




    }

    


    void WareHouse:: start(){
        open();
        string input;
        std::getline(std::cin,input);
        while (input!="close")
        {
            checkAction(input);
            std::getline(std::cin,input);
        }
        Action* action = new Close();
        
        action->act(*this);
        addAction(action->clone());
        delete action;
    }

    

    void WareHouse:: checkAction(string input){
        string actionString = separate(input);
        Action* action;

        if(actionString == "step"){
            int numOfSteps = stoi(separate(input));
            action = new SimulateStep(numOfSteps);

        }
        else if(actionString == "order"){
            int customerID = stoi(separate(input));
            action = new AddOrder(customerID);

            
        }
         else if(actionString == "customer"){
            string customerName = separate(input);
            string customerType = separate(input);
            int CustomerDis = stoi(separate(input));
            int maxOrders = stoi(separate(input));
            action = new AddCustomer(customerName,customerType,CustomerDis,maxOrders);
            
        }
         else if(actionString == "orderStatus"){
            int actionID = stoi(separate(input));
            action = new PrintOrderStatus(actionID);
            
        }
         else if(actionString == "customerStatus"){
            int customerID = stoi(separate(input));
            action = new PrintCustomerStatus(customerID);
            
        }
         else if(actionString == "volunteerStatus"){
            int volunteerID = stoi(separate(input));
            action = new PrintVolunteerStatus(volunteerID);
            
        }
         else if(actionString == "log"){
            action = new PrintActionsLog();
            
        }
        else if(actionString == "backup"){
            action = new BackupWareHouse();
            
        }
        else if(actionString == "restore"){
            action = new RestoreWareHouse();
            
        }
        
        else { // invalid action
            std::cout << "Error: Cannot place this order" << std::endl;
        }
        
        
        action->act(*this); //oop
        addAction(action->clone());
        delete action;
        

    }
    

    void WareHouse::addOrder(Order* order){
        pendingOrders.push_back(order);
        orderCounter++;
    }

    void WareHouse::addCustomer(Customer* customer){
        customers.push_back(customer);
        customerCounter++;
    }

    void WareHouse::addAction(Action* action){
        actionsLog.push_back(action);
    }


    void WareHouse:: close(){
        isOpen = false;
    }

    void WareHouse:: open(){
        isOpen = true;
        std::cout << "Warehouse is open!" << std::endl;
    }
    

    // vectors getters
    const vector<Order*> & WareHouse::getPendingOrders() const{return pendingOrders;}
    const vector<Order*> & WareHouse::getInProcessOrders()const{return inProcessOrders;}
    const vector<Order*> & WareHouse::getCompletedOrders()const {return completedOrders;}
    const vector<Action*> & WareHouse::getActions()const{return actionsLog;}
    const vector<Volunteer*> & WareHouse::getvolunteers()const{return volunteers;}

     Customer & WareHouse::getCustomer(int customerId) const{
        int size = customers.size();
        for(int i = 0 ; i < size ; i++ )
        {
            if(customers.at(i)->getId() == customerId)
                return *customers.at(i);
        }
        return *customers.at(0);//default customer - it won't ever get to this line because we check before getting to this function.
        


    }

    Volunteer & WareHouse::getVolunteer(int volunteerId) const{
        int size = volunteers.size();
        for(int i = 0 ; i < size ; i++ )
        {
            if(volunteers.at(i)->getId() == volunteerId)
                return *volunteers.at(i);

        }
        return *volunteers.at(0); //default volunteer - it won't ever get to this line because we check before getting to this function.

    }

     Order & WareHouse::getOrder(int orderId) const{
        
        for(unsigned i = 0 ; i < pendingOrders.size(); i++ )
        {
            if(pendingOrders.at(i)->getId() == orderId)
                return *pendingOrders.at(i);
        }
        for(unsigned int i = 0 ; i < inProcessOrders.size(); i++ )
        {
            if(inProcessOrders.at(i)->getId() == orderId)
                return *inProcessOrders.at(i);
        }
        for(unsigned int i = 0 ; i < completedOrders.size(); i++ )
        {
            if(completedOrders.at(i)->getId() == orderId)
                return *completedOrders.at(i);
        }
        return *pendingOrders.at(0); //default order - it won't ever get to this line because we check before getting to this function.

    }
    void WareHouse::addVolunteer(Volunteer* volunteer){
        volunteers.push_back(volunteer);
        volunteerCounter++;
    }

    int WareHouse:: getOrderID(){
        return orderCounter;
    }

    int WareHouse:: getCustomerCounter(){
        return customerCounter;
    }
    int WareHouse:: getVolunteerCounter(){
        return volunteerCounter;
    }

    void WareHouse:: setOrderDetails(int orderId, int volunteerId){
        // the order was accepted by a collector
        if(orderId < orderCounter){
            Order& orderRef = getOrder(orderId);
            if (orderRef.getStatus() == OrderStatus::PENDING){          
                orderRef.setStatus(OrderStatus::COLLECTING);
                orderRef.setCollectorId(volunteerId);
            }
            // the order was accepted by a driver
            else if(orderRef.getStatus() == OrderStatus::COLLECTING){
                orderRef.setStatus(OrderStatus::DELIVERING);
                orderRef.setDriverId(volunteerId);
            }
        }

        

    }

    //this functions help us change the order place in the vectors
    void WareHouse:: pendingToInProcces(int indexInCurrentVector, int orderId){
        Order* order = pendingOrders[indexInCurrentVector];
        pendingOrders.erase(pendingOrders.begin()+indexInCurrentVector);
        inProcessOrders.push_back(order);

    }
    void WareHouse:: InProccesToPending(int indexInCurrentVector, int orderId){
        Order* order = inProcessOrders[indexInCurrentVector];
        inProcessOrders.erase(inProcessOrders.begin()+indexInCurrentVector);
        pendingOrders.push_back(order);

    }
    void WareHouse:: InProccesToComplete(int indexInCurrentVector, int orderId){

        Order* order = inProcessOrders[indexInCurrentVector];
        order->setStatus(OrderStatus::COMPLETED);
        inProcessOrders.erase(inProcessOrders.begin()+indexInCurrentVector);
        completedOrders.push_back(order);

    }

    int WareHouse:: getIndexVolunteer(int volunteerId){
        int index = 0;
        for(Volunteer* v: volunteers){
            if(v->getId() == volunteerId)
                return index;
            index++;

        }
        return -1; //if he went to this line it means there is no volunteer
    }

    int WareHouse:: getIndexOrder(int OrderId){
        int index = 0;
        for(Order* o: pendingOrders){
            if(o->getId() == OrderId)
                return index;
            index ++;
        }
        return index;
    }

    void WareHouse:: deleteVolunteer(int volunteerId){
        int volunteerIndex = getIndexVolunteer(volunteerId);
        Volunteer* v = volunteers[volunteerIndex];
        delete v;
        v = nullptr;
        volunteers.erase(volunteers.begin()+volunteerIndex);

    }

    WareHouse &WareHouse::operator=(const WareHouse &other) {
        if (this != &other) {
            isOpen = other.isOpen;
        
            // Delete old vector's objects
            for (unsigned int i = 0 ; i < actionsLog.size() ; i++ ){
                delete actionsLog.at(i);
            }
            for (unsigned int i = 0 ; i < volunteers.size() ; i++ ){
                delete volunteers.at(i);
            }
            for (unsigned int i = 0 ; i < pendingOrders.size() ; i++ ){
                delete pendingOrders.at(i);
            }
            for (unsigned int i = 0 ; i < inProcessOrders.size() ; i++ ){
                delete inProcessOrders.at(i);
            }
            for (unsigned int i = 0 ; i < completedOrders.size() ; i++ ){
                delete completedOrders.at(i);
            }
            for (unsigned int i = 0 ; i < customers.size() ; i++ ){
                delete customers.at(i);
            }

            // Clear vectors before copying elements
            actionsLog.clear();
            volunteers.clear();
            pendingOrders.clear();
            inProcessOrders.clear();
            completedOrders.clear();
            customers.clear();
        
            // Copy elements from other to this
            // this->actionsLog = other.actionsLog;
            // this->volunteers = other.volunteers;
            // this->pendingOrders = other.pendingOrders;
            // this->inProcessOrders = other.inProcessOrders;
            // this->completedOrders = other.completedOrders;
            // this->customers = other.customers;
            for (Action* action: other.actionsLog){
                this->actionsLog.push_back(action->clone());
            }
            for (Volunteer* volunteer: other.volunteers){
                this->volunteers.push_back(volunteer->clone());
            }
            for (Order* order: other.pendingOrders){
                this->pendingOrders.push_back(new Order(*order));
            }
            for (Order* order: other.inProcessOrders){
                this->inProcessOrders.push_back(new Order(*order));
            }
            for (Order* order: other.completedOrders){
                this->completedOrders.push_back(new Order(*order));
            }
            for (Customer* customer: other.customers){
                this->customers.push_back(customer->clone());
            }

            // Copy counters
            customerCounter = other.customerCounter;
            volunteerCounter = other.volunteerCounter;
            orderCounter = other.orderCounter;
    }
    return *this;
}


     //Move constructor
     WareHouse::WareHouse(WareHouse && other):isOpen(other.isOpen),
      actionsLog(std::move(other.actionsLog)),
      volunteers(std::move(other.volunteers)),
      pendingOrders(std::move(other.pendingOrders)),
      inProcessOrders(std::move(other.inProcessOrders)),
      completedOrders(std::move(other.completedOrders)),
      customers(std::move(other.customers)),
      customerCounter(other.customerCounter),
      volunteerCounter(other.volunteerCounter),
      orderCounter(other.orderCounter){}


     //Move assignment operator
     WareHouse & WareHouse:: operator=(WareHouse&& other){
        if (this != &other)
        {
            isOpen = other.isOpen;
        
            actionsLog = std::move(other.actionsLog);
            volunteers = std::move(other.volunteers);
            pendingOrders = std::move(other.pendingOrders);
            inProcessOrders = std::move(other.inProcessOrders);
            completedOrders = std::move(other.completedOrders);
            customers = std::move(other.customers);

            other.actionsLog.clear();
            other.volunteers.clear();
            other.pendingOrders.clear();
            other.inProcessOrders.clear();
            other.completedOrders.clear();
            other.customers.clear();

            customerCounter = other.customerCounter;
            volunteerCounter = other.volunteerCounter;
            orderCounter = other.orderCounter;


        }
        return *this;
    } 
