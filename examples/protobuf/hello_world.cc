#include <iostream>

#include "examples/protobuf/proto/hello_world.pb.h"

int main()
{
    std::cout << "Hello World" << std::endl;

    examples::hello_world::HelloWorld msg;
    msg.set_name("this is name");
    msg.set_id(1234);
    msg.set_message("new message");

    std::cout << msg.DebugString() << std::endl;

    return 0;
}
