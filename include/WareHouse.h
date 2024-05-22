#pragma once
#include <string>
#include <vector>

#include "Order.h"
#include "Customer.h"

class Action;
class Volunteer;

// Warehouse responsible for Volunteers, Customers Actions, and Orders.


class WareHouse {

    public:
        WareHouse(const string &configFilePath);
        //rule of five :
        WareHouse(const WareHouse& other); //copy constructor
        ~WareHouse(); //distructor
        WareHouse &operator= (const WareHouse& other); //assignment operator
        WareHouse(WareHouse && other); //Move constructor
        WareHouse& operator=(WareHouse&& other); //Move assignment operator
        void start();
        void addOrder(Order* order);
        void addCustomer(Customer* customer); //we added this function in order to add a custumer to the vector
        void addAction(Action* action);
        void addVolunteer(Volunteer* volunteer);//we added this function in order to add a volunteer to the vector
        Customer &getCustomer(int customerId) const;
        Volunteer &getVolunteer(int volunteerId) const;
        Order &getOrder(int orderId) const;
        const vector<Order*> &getPendingOrders()const;
        const vector<Order*> &getInProcessOrders()const;
        const vector<Order*> &getCompletedOrders()const;
        const vector<Volunteer*> &getvolunteers()const;
        const vector<Action*> &getActions()const;
        void close();
        void open();
        //functions that are responsible for the deletion of a volunteer
        void deleteVolunteer(int volunteerId);
        int getIndexVolunteer(int volunteerId);
        int getIndexOrder(int OrderId);

        int getOrderID();
        int getCustomerCounter();
        int getVolunteerCounter();
        //this functions help us change the order place in the vectors
        void pendingToInProcces(int indexInCurrentVector, int orderId);
        void InProccesToPending(int indexInCurrentVector, int orderId);
        void InProccesToComplete(int indexInCurrentVector, int orderId);
        void setOrderDetails(int orderId, int volunteerId);

    private:
        bool isOpen;
        vector<Action*> actionsLog;
        vector<Volunteer*> volunteers;
        vector<Order*> pendingOrders;
        vector<Order*> inProcessOrders;
        vector<Order*> completedOrders;
        vector<Customer*> customers;
        int customerCounter; //For assigning unique customer IDs
        int volunteerCounter; //For assigning unique volunteer IDs
        int orderCounter; //For assigning unique order IDs


        string separate(string& input);
        void checkAction(string input);
};