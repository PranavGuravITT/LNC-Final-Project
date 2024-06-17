#include "server.h"
#include "database.h"
int main() {
    Database database("127.0.0.1", "pranav", "pranav@123", "Cafetaria");
    Server server(8080,&database);
    server.run();
    return 0;
}
