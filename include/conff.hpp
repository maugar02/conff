/******************************************************************************
 *  Proyecto: CONFF
 *  Archivo: conff.hpp
 *  Descripción: Declaración de la clase conf_file
 *  Autor: maugar02
 *  Fecha de creación: 05-06-2025
 *  Licencia: BSD 3-Clause
 *
 * Este código declara la clase <<conf_file>>, la cual representa un archivo
 * de configuración.
 *
 * También en este archivo se declaran espresiones constantes para el manejo
 * de errores en los métodos para obtener datos. De igual manera se declaran
 * estructuras de datos que sirven de base para la gestión de items.
 * 
 * Copyright (c) 2025, maugar02
 * Todos los derechos reservados.
 *
 ******************************************************************************/

#pragma once

#ifdef _MSC_VER
#pragma warning(disable: 4251)
#endif

#ifdef CONFF_EXPORTS
#define CONFF_API __declspec(dllexport)
#else
#define CONFF_API __declspec(dllimport)
#endif

#include <fstream>  // Para manejo de archivos
#include <vector>   // Para utilizar std::vector
#include <string>   // Para utlizar std::string

namespace cf {

    // Estas expresiones constantes se utlilizan para detectar errores
    // en los métodos en los que se obtiene datos. Estos métodos retornan
    // dichos valores constantes para indicar un problema al obtener los datos.
    constexpr int CONFF_VERSION(1000);
    constexpr int INT_ERROR(2147483647);
    constexpr char STR_ERROR[] = "_NaN_";

    // Define identifiadores para los tipos de datos
    enum class CONFF_ITEM_TYPE : unsigned short {
        TEXT = 0,
        INTEGER,
        NO_TYPE
    };

    // La estructura guarda datos que represrtan en contenido de una variable
    // de configuración.
    struct conff_item {
        conff_item() : type(CONFF_ITEM_TYPE::NO_TYPE), hash(""), value("") {}
        CONFF_ITEM_TYPE type;
        std::string hash;
        std::string value;
    };

    // Clase conf_file, esta clase engloba lo relacionado con el manejo de
    // Archivos de configuración
    class CONFF_API conf_file final {
        public:

        // Constructor por defecto
        conf_file();
        // Destructor
        ~conf_file();

        // Abre un archivo de configuración.
        // @param file_name: Nombre del archivo de configuración.
        // @return bool: verdadero si se pudo abrir con éxito. 
        bool open(const std::string &file_name);

        // Crear y abre un archivo de configuración.
        // Si el archivo existía se borra el contenido.
        // @param file_name: Nombre del archivo de configuración.
        // @return bool: verdadero si se pudo crear y abrir con éxito. 
        bool create_and_open(const std::string &file_name);

        // Guarda las modificaciones realizadas durante la ejecución
        // a un archivo de configuración.
        // @return bool: verdadero si se pudo guardar con éxito. 
        bool save();

        // Modifica o crear un items de configuración de tipo <<STRING>
        // @param config_name: identificador de configuración.
        // @param text: texto que se asigna al item.
        // @return bool: Verdadero si se ha agregado o modificado el item con éxito.
        bool set_string(const std::string &config_name, const std::string &text);

        // Modifica o crear un items de configuración de tipo <<INTEGER>
        // @param config_name: identificador de configuración.
        // @param value: valor entero que se asigna al item.
        // @return bool: Verdadero si se ha agregado o modificado el item con éxito.
        bool set_int(const std::string &config_name, int value);

        // Elimina un items de configuración
        // @param config_name: Nombre del item de configuración.
        // @return bool: Verdadero si se ha eliminado con éxito.
        bool delete_config(const std::string &config_name);
        
        // Obtiene datos desde el items en formato de tipo <<STRING>>
        // @param config_name: nombre del item.
        // @return std::string: el contenido como tipo <<STRING>>
        // Si no se puede obtener los datos de devuelve <<STR_ERROR>>
        std::string get_string(const std::string &config_name);

        // Obtiene datos desde el items en formato de tipo <<INTEGER>>
        // @param config_name: nombre del item.
        // @return int: el contenido como tipo <<INTEGER>>.
        // Si no se puede obtener los datos o si el formato de los
        // datos no corresponde valores enteros de devuelve <<INT_ERROR>>
        int get_int(const std::string &config_name);

        // Devuelve el número de items que posee el archivo.
        // @return std::size:t: número de items del archivo
        std::size_t get_n_items();

        // Obtene la version de conff que utiliza el archivo.
        int get_version();

        private:

        // Variables
        int _version;   // Guarada la versión conff del archivo.
        std::string _file_name; // Guarda el nombre del archivo.
        std::vector<conff_item> _items; // Contiene los items del archivo.
        
        // Métodos

        // Convierte una cadena de texto en un item.
        conff_item _item_from_text(const std::string &text) const;
        // Convuerte un item en una cadena de texto.
        std::string _text_from_item(const conff_item &item) const;
        // Busca un item mediante el nombre.
        int _find_item_by_name(const std::string &name);
        // Busca un items meiante el hash
        int _find_item_by_hash(const std::string &hash);

    };

}