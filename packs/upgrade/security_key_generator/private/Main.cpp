#include "packs/upgrade/security_key_generator/public/MaterialGenerator.hpp"
#include "packs/upgrade/pack_builder/public/RandomNumberGenerator.hpp"
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
