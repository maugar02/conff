/******************************************************************************
 *  Proyecto: CONFF
 *  Archivo: conff.cpp
 *  Descripción: Definición de la clase conf_file
 *  Autor: maugar02
 *  Fecha de creación: 05-06-2025
 *  Licencia: BSD 3-Clause
 *
 * Este código define la clase <<conf_file>>, la cual representa un archivo
 * de configuración.
 *
 * Copyright (c) 2025, maugar02
 * Todos los derechos reservados.
 *
 ******************************************************************************/

#include <conff.hpp>
#include <sstream>  // Para utilizar std::ostringstream
#include <cctype>   // Para usar std::is_digit
#include "md5.h"    // Para crear los hash de los items

namespace cf {

    // Verifica si una cadena de texto corresponde
    // a un valor númerico de tipo entero.
    // @param text: texto a verificar.
    // @return bool: Verdadero si el texto correspode a
    // un valor numero entero.
    bool is_integer(const std::string &text) {

        if(text.empty()) {
            return false;
        }

        for(const auto &c : text) {
            if(!std::isdigit(c)) {
                return false;
            }
        }

        return true;
    }

    // Separa una cadena de texto
    std::vector<std::string> Split(const std::string& text, char separator)
    {
        std::vector<std::string> result;
        std::size_t start = 0, end;

        if(text.empty())
            return result;

        while ((end = text.find(separator, start)) != std::string::npos) {
            result.emplace_back(text.substr(start, end - start));
            start = end + 1;
        }

        result.emplace_back(text.substr(start)); // Añade el último fragmento.
        
        return result;
    }
    
    // Obtene la version de conff que utiliza el archivo.
    int conf_file::get_version() {
        return _version;
    }

    // Constructor
    conf_file::conf_file() : _version{-1}, _file_name{""}
    {
        // Nada por ahora
    }

    // Destructor
    conf_file::~conf_file()
    {
        // Nada por ahora
    }

    // Devuelve el número de items que posee el archivo.
    // @return std::size:t: número de items del archivo
    std::size_t conf_file::get_n_items()
    {
        return _items.size();
    }

    // Convierte una cadena de texto en un item.
    conff_item conf_file::_item_from_text(const std::string &text) const {

        conff_item item;

        auto content = Split(text, '~');

        if(content.size() != 2ULL) {
            return item;
        }

        auto info = Split(content[0], ' ');

        if(info.size() != 3ULL || info[0] != "$config" || !is_integer(info[2])) {
            return item;
        }

        item.hash = info[1];
        item.value = content[1];

        item.type = static_cast<CONFF_ITEM_TYPE> (std::stoul(info[2]));

        return item;
    }

    // Convuerte un item en una cadena de texto.
    std::string conf_file::_text_from_item(const conff_item &item) const {

        std::ostringstream line;

        line << "$config " << item.hash << " " << static_cast<unsigned short>(item.type) << "~" << item.value << std::endl;

        return line.str();
    }

    // Busca un item mediante el nombre.
    int conf_file::_find_item_by_name(const std::string &name)
    {
        MD5 hash_generator;
        std::string hash = hash_generator(name);
        return _find_item_by_hash(hash);
    }
    
    // Busca un items meiante el hash
    int conf_file::_find_item_by_hash(const std::string &hash)
    {
        int index(0);

        for(conff_item const&item : _items) {
            if(item.hash == hash)
                return index;
            index++;
        }

        return -1;
    }

    // Abre un archivo de configuración.
    // @param file_name: Nombre del archivo de configuración.
    // @return bool: verdadero si se pudo abrir con éxito.
    bool conf_file::open(const std::string &file_name)
    {
        // Verificamos si no hay datos
        if(!_file_name.empty()) {
            _items.clear();
            _file_name.clear();

            _version = -1;
        }

        // Creamos y abrimos el archivo
        std::ifstream file(file_name);

        if(!file.good()) {
            file.close();
            return false;
        }


        bool first(true);
        std::string line;

        // Carga el header del archivo
        auto load_header = [this](const std::string &_text) -> bool {

            auto header = Split(_text, ':');

            if(header.size() != 2ULL || header[0] != "@conff") {
                return false;
            }
            
            if(!is_integer(header[1]))
            {
                return false;
            }

            this->_version = std::stoi(header[1], nullptr);

            return true;
        };

        // Recorremos todo el archivo
        while(!file.eof()) {
            std::getline(file, line); // Obtenemos una línea

            if(line.empty()) continue; // Ignoramos si está vacía

            // La primera línea la consideramos como el header
            if(first) {

                if(!load_header(line)) {
                    file.close();
                    return false;
                }

                first = false;
                continue; // Pasamos a la siguiente línea
            }

            // Cremos el item a partir de la linea de texto
            auto item = _item_from_text(line);

            // Si hay un error de formato, ignoramos en contenido de la línea
            if(item.hash.empty()) {
                continue;    
            }

            // Agreamos a la lista de items
            _items.push_back(item);
        }

        _file_name = file_name;

        return true;
    }

    // Crear y abre un archivo de configuración.
    // Si el archivo existía se borra el contenido.
    // @param file_name: Nombre del archivo de configuración.
    // @return bool: verdadero si se pudo crear y abrir con éxito.    
    bool conf_file::create_and_open(const std::string &file_name)
    {
        std::ofstream out_file(file_name, std::ios::trunc);

        if(!out_file.good()) {
            out_file.close();
            return false;
        }

        std::ostringstream oss;

        oss << "@conff:" << CONFF_VERSION << std::endl;
        out_file.write(oss.str().c_str(), oss.str().length());
        out_file.close();

        return open(file_name);
    }

    // Guarda las modificaciones realizadas durante la ejecución
    // a un archivo de configuración.
    // @return bool: verdadero si se pudo guardar con éxito.
    bool conf_file::save()
    {
        // Verificamos que exista un nombre de archivo
        if(_file_name.empty()) {
            return false;
        }

        std::ofstream file(_file_name);

        if(!file.good()) {
            file.close();
            return false;
        }

        std::ostringstream ss;
        ss << "@conff:" << CONFF_VERSION << std::endl;
        file.write(ss.str().c_str(), ss.str().length());

        for(const auto &item : _items) {

            std::string line;
            line = _text_from_item(item);
            //line += "\n";
            file.write(line.c_str(), line.length());
        }

        file.close();

        return true;
    }

    // Modifica o crear un items de configuración de tipo <<STRING>
    // @param config_name: identificador de configuración.
    // @param text: texto que se asigna al item.
    // @return bool: Verdadero si se ha agregado o modificado el item con éxito.
    bool  conf_file::set_string(const std::string &config_name, const std::string &text)
    {
        conff_item item;
        MD5 hash_generator;

        auto index = _find_item_by_name(config_name);
        if(index < 0) {

            // Se crea el item
            item.hash = hash_generator(config_name);
            item.type = CONFF_ITEM_TYPE::TEXT;
            item.value = text;

            _items.push_back(item);
            return true;
        }

        // Existe, modificamos
        _items[index].type = CONFF_ITEM_TYPE::TEXT;
        _items[index].value = text;

        return true;
    }

    // Modifica o crear un items de configuración de tipo <<INTEGER>
    // @param config_name: identificador de configuración.
    // @param value: valor entero que se asigna al item.
    // @return bool: Verdadero si se ha agregado o modificado el item con éxito.
    bool  conf_file::set_int(const std::string &config_name, int value)
    {
        conff_item item;
        MD5 hash_generator;

        auto index = _find_item_by_name(config_name);
        if(index < 0) {

            // Se crea el item
            item.hash = hash_generator(config_name);
            item.type = CONFF_ITEM_TYPE::INTEGER;
            item.value = std::to_string(value);

            _items.push_back(item);
            return true;
        }

        // Existe, modificamos
        _items[index].type = CONFF_ITEM_TYPE::INTEGER;
        _items[index].value = std::to_string(value);

        return true;
    }

    // Elimina un items de configuración
    // @param config_name: Nombre del item de configuración.
    // @return bool: Verdadero si se ha eliminado con éxito.
    bool conf_file::delete_config(const std::string &config_name)
    {
        auto index = _find_item_by_name(config_name);
        if(index < 0) {
            // No existe :(
            return false;
        }

        // Existe, eliminamos
        _items.erase(_items.begin() + index);
        return true;
    }
    
    // Obtiene datos desde el items en formato de tipo <<STRING>>
    // @param config_name: nombre del item.
    // @return std::string: el contenido como tipo <<STRING>>
    // Si no se puede obtener los datos de devuelve <<STR_ERROR>>
    std::string conf_file::get_string(const std::string &config_name)
    {
        std::string result;
        
        auto index = _find_item_by_name(config_name);
        if(index < 0) {
            result = STR_ERROR;
            return result;
        }

        result = _items[index].value;
        return result;
    }

    // Obtiene datos desde el items en formato de tipo <<INTEGER>>
    // @param config_name: nombre del item.
    // @return int: el contenido como tipo <<INTEGER>>.
    // Si no se puede obtener los datos o si el formato de los
    // datos no corresponde valores enteros de devuelve <<INT_ERROR>>    
    int conf_file::get_int(const std::string &config_name)
    {
        int result;
        
        auto index = _find_item_by_name(config_name);
        if(index < 0) {
            return INT_ERROR;
        }

        if(!is_integer(_items[index].value))
            return INT_ERROR;

        result = std::stoi(_items[index].value);
        return result;
    }
}