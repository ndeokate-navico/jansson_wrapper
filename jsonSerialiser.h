/*!
 * @file JsonSerializer.h
 * @brief This contains the class that can serialize and deserialize a packet of
 * data to and from json format.
 * Details. Use the parse function and the Getters to convert data in json
 * format back to c variables/data structures.  Use the createrootobject and the
 * putters to convert the data from c variables/data structures to json format.
 * Finally the resultant json can be converted to a null terminated byte stream
 * by the SteamJsontoBuffer function.
 * @author Nitin Deokate
 * @date Sept/2013
 * $Id$
 * */

#ifndef JSONSERIALIZER_H
#define JSONSERIALIZER_H

#include <string>
#include <set>
#include <vector>
#include <sstream>
#include <memory>

#include <jansson.h>

#define DEFAULT_LIMIT_GET_COLLECTION -1

/* Class JsonSerializer is a wrapper which hides the details of underlying cJSON
 * apis from the user. It also provides a C++ way of interaction with the json
 * formatting code.
 * */
class JsonSerializer {
   public:
    JsonSerializer();
    JsonSerializer(json_t* json);
    ~JsonSerializer();
    JsonSerializer(const JsonSerializer& serializer);
    JsonSerializer& operator=(const JsonSerializer& serializer);
    void Clear();
    bool Parse(const std::string& instr);
    bool CreateRootObject();
    bool GetValue(const std::string& key, std::string& value) const;
    bool PutValue(const std::string& key, const std::string& value);
    bool GetObject(const std::string& key, JsonSerializer& serializer) const;
    bool PutObject(const std::string& key, JsonSerializer& object);
    bool GetCollection(const std::string& key,
                       std::vector<JsonSerializer>& vec) const;
    bool PutCollection(const std::string& key,
                       std::vector<JsonSerializer>& collection);
    bool GetStringCollection(const std::string& key,
                             std::set<std::string>& collection,
                             int limit = DEFAULT_LIMIT_GET_COLLECTION) const;
    bool PutStringCollection(const std::string& key,
                             const std::set<std::string>& collection,
                             int limit = DEFAULT_LIMIT_GET_COLLECTION);
    char* StreamJsonToBuffer() const;

    /*!
     * GetValue is a template function that gets the value out from the parsed
     * cjson structure and converts it to the right type
     * @param const reference to string which is the key to look for in the
     * current cjson struct.
     * @param reference to value of the required type. The value found is
     * returned in this variable.
     * @param reference to function pointer which is used by istringstream to
     * convert the string value to correct type.
     * @return bool. True if the value exists in the cjson and can be converted
     * to the type desired. Otherwise false.
     * */
    template <typename T>
    bool GetValue(const std::string& key, T& value,
                  std::ios_base& (*f)(std::ios_base&) = std::dec) const {
        if (m_Json && json_is_object(m_Json)) {
            json_t* item = json_object_get(m_Json, key.c_str());
            if (item) {
                std::ostringstream oss;
                if (json_is_integer(item)) {
                    oss << json_integer_value(item);
                } else if (json_is_real(item)) {
                    oss << json_real_value(item);
                } else if (json_is_string(item)) {
                    oss << json_string_value(item);
                } else if (json_is_boolean(item)) {
                    oss << json_boolean_value(item);
                } else {
                    char* s = json_dumps(item, JSON_ENCODE_ANY | JSON_COMPACT);
                    if (s == 0) return false;
                    oss << s;
                    free(s);
                }

                std::istringstream iss(oss.str());
                return !(iss >> f >> value).fail();
            }
        }

        return false;
    }

    /*!
     * PutValue is a template function that adds the passed key and value pair
     * to the cjson structure.
     * @param const reference to string which is the key to add to the current
     * cjson struct.
     * @param const reference to value of the required type. The value is added
     * to the cjson struct
     * @param reference to function pointer which is used by istringstream to
     * convert the string value to correct type.
     * @return bool. False if the stringstream object cant convert from the
     * value to a string. true otherwise
     * */
    template <typename T>
    bool PutValue(const std::string& key, const T& value,
                  std::ios_base& (*f)(std::ios_base&) = std::dec) const {
        if (m_Json && json_is_object(m_Json)) {
            std::ostringstream oss;
            if ((oss << f << value).fail()) return false;

            std::istringstream iss(oss.str());
            long long val;

            if (!(iss >> val).fail()) {
                int ret = json_object_set_new(m_Json, key.c_str(),
                                              json_integer(value));
                if (ret == -1) return false;
            } else {
                int ret = json_object_set_new(m_Json, key.c_str(),
                                              json_string(oss.str().c_str()));
                if (ret == -1) return false;
            }

            return true;
        }

        return false;
    }

   private:
    /* members */
    json_t* m_Json;
};

#endif  // JSONSERIALIZER_H
