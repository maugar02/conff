#include <conff.hpp>
#include <iostream>

int main(int argc, char **argv)
{
    cf::conf_file cf1;

    // Version test
    std::cout << "Version: " << cf1.get_version() << std::endl;

    // Open Version
    if(!cf1.open("conf.txt")) {

        std::cout << "No se pudo crear el archivo." << std::endl;
    }

    // Version test
    std::cout << "Version: " << cf1.get_version() << std::endl;

    /*
    cf1.set_int("neural_arch1",90);
    cf1.set_int("neural_arc2",232);
    cf1.set_int("neural_arc23h",90231);
    cf1.set_int("neural_ar23ch",9230);
    cf1.set_int("neural_ar2ch",930);

    cf1.set_string("test-aqwsdkjh", "texoasdalsjdasdasd");
    cf1.set_string("test-awesdkjh", "texoasdwwalsjdasdasd");
    cf1.set_string("test-asdqwkjh", "texoasdalsjdasdasd");
    cf1.set_string("test-asdkjh", "texoasdalwesjdasdasd");
    cf1.set_string("test-asddkjh", "texoasdaleesjdasdasd"); */

    std::cout << "N items: " << cf1.get_n_items() << std::endl;

    std::cout << "Get string: " << cf1.get_string("test-asdqwkjh") << std::endl;
    std::cout << "Get int: " << cf1.get_int("neural_ar2ch") << std::endl;

    if (!cf1.save())
        std::cout << "Error al guardar!" << std::endl;

    return 0;
}