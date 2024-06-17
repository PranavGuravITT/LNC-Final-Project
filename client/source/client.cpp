
#include "client.h"

Client::Client(const std::string &server_address, int port)
    : server_address_(server_address), port_(port), client_fd_(0)
{
    memset(&server_addr_, 0, sizeof(server_addr_));
    create_socket();
    connect_to_server();
}

Client::~Client()
{
    close(client_fd_);
}

void Client::create_socket()
{
    if ((client_fd_ = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
}

void Client::connect_to_server()
{
    server_addr_.sin_family = AF_INET;
    server_addr_.sin_port = htons(port_);

    if (inet_pton(AF_INET, server_address_.c_str(), &server_addr_.sin_addr) <= 0)
    {
        perror("Invalid address/Address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(client_fd_, (struct sockaddr *)&server_addr_, sizeof(server_addr_)) < 0)
    {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }
}

void Client::authenticate(const std::string &user_id, const std::string &password)
{
    std::string message = "VALIDATE:" + user_id + ":" + password;
    std::cout << message << std::endl;
    send(client_fd_, message.c_str(), message.length(), 0);
    std::cout << "Authentication request sent\n";

    char buffer[buffer_size_] = {0};
    read(client_fd_, buffer, buffer_size_);
    std::cout << "Response from server: " << buffer << std::endl;
}

void Client::storeFoodItem()
{
    std::string message = "ADD_FOOD_ITEM:2:Pizza:9.99";
    send(client_fd_, message.c_str(), message.length(), 0);
    std::cout << "Add food item request sent\n";

    char buffer[buffer_size_] = {0};
    read(client_fd_, buffer, buffer_size_);
    std::cout << "Response from server: " << buffer << std::endl;
}

void Client::adminScreen(){
        std::string user_id, password;
        std::cout << "Enter User ID: ";
        std::cin >> user_id;
        std::cout << "Enter Password: ";
        std::cin >> password;
        authenticate(user_id, password);
        while(1){
        char choice;

        std::cout<<"----------WELCOME ADMIN-------------"<<std::endl;
        std::cout<<"\n-------OPERATIONS-----"<<std::endl;
        std::cout<<"1. REGISTER EMPLOYEE"<<std::endl;
        std::cout<<"2. ADD FOOD ITEM"<<std::endl;
        std::cout<<"3. EMPLOYEE"<<std::endl;
        std::cin >> choice;
        switch (choice)
        {
        case '1':
            adminScreen();
            break;
        
        default:
            break;
        }
    }
}

void Client::takeEmployeeInformation(){
     std::string message = "ADD_EMPLOYEE:1:Pranav:pranav123";
    send(client_fd_, message.c_str(), message.length(), 0);
    std::cout << "register employee request sent\n";

    char buffer[buffer_size_] = {0};
    read(client_fd_, buffer, buffer_size_);
    std::cout << "Response from server: " << buffer << std::endl;
}

void Client::run()
{
   

    while(1){
        char choice;
        std::cout<<"----------CAFETARIA-------------"<<std::endl;
        std::cout<<"LOGIN AS"<<std::endl;
        std::cout<<"1. ADMIN"<<std::endl;
        std::cout<<"2. CHEF"<<std::endl;
        std::cout<<"3. EMPLOYEE"<<std::endl;
        std::cin >> choice;
        switch (choice)
        {
        case '1':
            adminScreen();
            break;
        
        default:
            break;
        }
    }
    
    while (true)
    {
        char choice;
        std::cout << "Do you want to add a food item? (y/n): ";
        std::cin >> choice;
        if (choice == 'y' || choice == 'Y')
        {
            storeFoodItem();
        }
        else
        {
            std::cout << "Exiting...\n";
            break;
        }
    }
}
