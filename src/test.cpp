

#include "../include/primitive.h"
#include "../include/model.h"
#include "../include/agent.h"

int main() {

    auto vec = Vec3({0,0,0}) + ZAXIS;
    auto shape = Primitive::rectangle(5.0, 5.0);
    auto agent = Agent();

    return 0;
}


