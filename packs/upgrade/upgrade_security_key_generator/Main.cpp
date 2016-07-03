#include "upgrade_security_key_generator/MaterialGenerator.hpp"
#include "upgrade_pack_builder_library/RandomNumberGenerator.hpp"
#include <iostream>

int main(int argc, char* argv[])
{
    try
    {
        application::SecureRandomNumberGenerator randomNumberGenerator;
        application::MaterialGenerator generator(randomNumberGenerator);
        generator.WriteKeys("Keys.cpp");
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
        return 2;
    }

    return 0;
}
