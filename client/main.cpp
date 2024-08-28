#include "clientApp.h"

int main() {
    ClientApp client("127.0.0.1", 8080);
    client.run();
    return 0;
}
