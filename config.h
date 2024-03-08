
# ifndef config_h
# define config_h

#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <string>

using std::getline;
using std::ifstream;
using std::istringstream;
using std::runtime_error;
using std::string;
using std::unordered_map;
namespace flightSimulator
{
    class Config
    {
        string filename;
        unordered_map<string, unordered_map<string, string>> config;

        void extractFromFile(std::string const &filename){
            std::ifstream file(filename);

            if (!file.is_open())
                throw runtime_error("Failed to open file: " + filename);

            string line;
            string currentSection;
            while (getline(file, line))
            {
                if (line.empty() || line[0] == ';' || line[0] == '#')
                    continue;

                if (line[0] == '[' && line.back() == ']')
                {
                    currentSection = line.substr(1, line.size() - 2);
                    continue;
                }

                istringstream iss(line);
                string key, value;
                if (getline(iss, key, '=') && getline(iss, value))
                {
                    key.erase(0, key.find_first_not_of(" \t"));
                    key.erase(key.find_last_not_of(" \t") + 1);
                    value.erase(0, value.find_first_not_of(" \t"));
                    value.erase(value.find_last_not_of(" \t") + 1);

                    config[currentSection][key] = value;
                }
            }
        }

    public:
        Config(string const filename)
        {
            extractFromFile(filename);
        }

        string get(string const &section, string const &key)
        {
            if (config.find(section) == config.end())
                throw runtime_error("Section not found: " + section);

            if (config[section].find(key) == config[section].end())
                throw runtime_error("Section not found: " + section);

            return config[section][key];
        }

        int getInt(string const &section, string const &key)
        {
            if (config.find(section) == config.end())
                throw runtime_error("Section not found: " + section);

            if (config[section].find(key) == config[section].end())
                throw runtime_error("Key not found: " + key);

            return std::stoi(get(section, key));
        }

        float getFloat(string const &section, string const &key)
        {
            if (config.find(section) == config.end())
                throw runtime_error("Section not found: " + section);

            if (config[section].find(key) == config[section].end())
                throw runtime_error("Key not found: " + key);

            return std::stof(get(section, key));
        }

        template <typename T>
        void set(string const &section, string const &key, T const &value)
        {
            config[section][key] = std::to_string(value);
        }

        void extend(string const &filename){
            extractFromFile(filename);
        }
    };

    Config config("config/default.ini");
}
# endif