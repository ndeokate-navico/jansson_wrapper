/*!
 * @file JsonSerializer.h
 * @brief This contains the class that can serialize and deserialize a packet of
 * data to and from json format.
 * Details. Use the parse function and the Getters to convert data in json
 * format back to c variables/data structures.  Use the createrootobject and the
 * putters to convert the data from c variables/data structures to json format.
 * Finally the resultant json can be converted to a null terminated byte stream
 * by the SteamJsontoBuffer function.
 * @author : nitin deokate
 * @date   : Sept/2013
 * $Id$
 * */

#include "public/JSonSerializer.h"

/*!
 * default no param constructor
 * */
JsonSerializer::JsonSerializer() : m_Json(0) {}

/*!
 * overloaded one param constructor. This gets passed a json pointer.
 * @param pointer to an allocated json struct
 * */
JsonSerializer::JsonSerializer(json_t* json) : m_Json(json) {
    json_incref(m_Json);
}

/*!
 * Copy constructor.
 * @param const reference to a jsonserializer object.
 * */
JsonSerializer::JsonSerializer(const JsonSerializer& serializer) {
    m_Json = serializer.m_Json;
    json_incref(m_Json);
}

/*! Assignment operator.
 * @param const reference to a jsonserializer object.
 * @return reference to JsonSerializer object.
 * */
JsonSerializer& JsonSerializer::operator=(const JsonSerializer& serializer) {
    Clear();
    m_Json = serializer.m_Json;
    json_incref(m_Json);
    return *this;
}

/*!
 * Destructor
 * */
JsonSerializer::~JsonSerializer() { Clear(); }

void JsonSerializer::Clear() {
    if (m_Json) json_decref(m_Json);
    m_Json = 0;
}

/*!
 * function Parse. A json formatted data stream is parsed into a json
 * structure.
 * @param const reference to a string which contains the json formatted data
 * stream.
 * @return bool. True if the data is in correct format and we could create a
 * json object from it.False otherwise
 * */
bool JsonSerializer::Parse(const std::string& instr) {
    Clear();
    m_Json = json_loads(instr.c_str(), 0, NULL);
    return m_Json ? true : false;
}

/*!
 * function CreateRootObject. This function allocates memory for an empty json
 * structure so that we can add data to it before streaming it to a json
 * formatted data stream.
 * @return bool. True an empty json object could be created.
 * */
bool JsonSerializer::CreateRootObject() {
    Clear();
    m_Json = json_object();
    return m_Json ? true : false;
}

/*!
 * GetValue function. Given a string key, this looks through the json object
 * and returns a string value if found.
 * @param const reference to string which is the key to look for in the current
 * json struct.
 * @param reference to value of the required type. The value found is returned
 * in this variable.
 * @return bool. True if the value exists in the json. Otherwise false.
 * */
bool JsonSerializer::GetValue(const std::string& key,
                              std::string& value) const {
    if (m_Json && json_is_object(m_Json)) {
        json_t* item = json_object_get(m_Json, key.c_str());
        if (item && json_is_string(item)) {
            value.assign(json_string_value(item));
            return true;
        }
    }

    return false;
}

/*!
 * PutValue function. Given a string key, and a string value, this function adds
 * them to the json object.
 * @param const reference to string which is the key to look for in the current
 * json struct.
 * @param const reference to value of the required type. The value found is
 * returned in this variable.
 * @return bool. True if the value exists in the json. Otherwise false.
 * */
bool JsonSerializer::PutValue(const std::string& key,
                              const std::string& value) {
    if (m_Json && json_is_object(m_Json)) {
        int ret = json_object_set_new(m_Json, key.c_str(),
                                      json_string(value.c_str()));
        if (ret == 0) return true;
    }

    return false;
}

/*!
 * Getobject function. Given a string key, this function retrieves the json
 * object that is nested under this key.
 * @param const reference to string which is the key to look for in the current
 * json struct.
 * @param reference to JsonSerializer. The underlying nested json object is
 * passed on to this variable.
 * @return bool. True if the value exists in the json. Otherwise false.
 * */
bool JsonSerializer::GetObject(const std::string& key,
                               JsonSerializer& serializer) const {
    if (m_Json && json_is_object(m_Json)) {
        json_t* json = json_object_get(m_Json, key.c_str());
        if (json) {
            serializer.Clear();
            serializer.m_Json = json;
            json_incref(serializer.m_Json);
            return true;
        }
    }
    return false;
}

/*!
 * Putobject function. Given a string key, and a serializer, the passed json
 * object is nested under this key in the json owned by this serializer.
 * @param const reference to string which is the key to look for in the current
 * json struct.
 * @param reference to JsonSerializer. The underlying nested json object is
 * passed on to this variable.
 * @return bool. True
 * */
bool JsonSerializer::PutObject(const std::string& key, JsonSerializer& object) {
    int ret = json_object_set(m_Json, key.c_str(), object.m_Json);
    if (ret == 0) return true;
    return false;
}

/*!
 * GetCollection function. Given a string key, this function retrieves the array
 * of json objects that are nested under this key.
 * @param const reference to string which is the key to look for in the current
 * json struct.
 * @param reference to a vector of serializer objects. The underlying nested
 * array of json objects is added to this variable.
 * @return bool. True if the key exists in the json. Otherwise false.
 * */
bool JsonSerializer::GetCollection(const std::string& key,
                                   std::vector<JsonSerializer>& vec) const {
    if (m_Json) {
        json_t* array = 0;

        if (key.empty()) {
            array = m_Json;
        } else {
            if (!json_is_object(m_Json)) return false;
            array = json_object_get(m_Json, key.c_str());
            if (array == 0) return false;
        }

        if (!json_is_array(array)) return false;

        size_t sz = json_array_size(array);
        if (sz == 0) return true;

        vec.reserve(sz);
        for (size_t i = 0; i < sz; ++i) {
            /* each row in the arary will be one serializer object. */
            json_t* json = json_array_get(array, i);
            if (json == 0) return false;
            vec.push_back(json);
        }

        return true;
    }

    return false;
}

/*!
 * PutCollection function. Given a string key, this function nests the key and
 * the array of json objects into the current json struct.
 * @param const reference to string which is the key to add in the current json
 * struct.
 * @param reference to a vector of serializer objects. The passed array of json
 * objects is nested under the key passed.
 * @return bool. True
 * */
bool JsonSerializer::PutCollection(const std::string& key,
                                   std::vector<JsonSerializer>& collection) {
    if (!m_Json || !json_is_object(m_Json)) return false;

    json_t* array = json_array();
    int ret = 0;
    if (array == 0) return false;

    for (size_t i = 0; i < collection.size(); ++i) {
        ret = json_array_append(array, collection[i].m_Json);
        if (ret == -1) {
            json_decref(array);
            return false;
        }
    }

    ret = json_object_set_new(m_Json, key.c_str(), array);
    if (ret == -1) return false;

    return true;
}

/*!
 * GetStringCollection function. Given a string key, this function retrieves the
 * array of strings that are nested under this key.
 * @param const reference to string which is the key to look for in the current
 * json struct.
 * @param reference to a set of strings. The underlying nested array of strings
 * is added to this variable.
 * @param int limit. After getting upto the limit, no more will be pulled out.
 * @return bool. True if the key exists in the json. Otherwise false.
 * */
bool JsonSerializer::GetStringCollection(const std::string& key,
                                         std::set<std::string>& collection,
                                         int limit) const {
    if (m_Json) {
        json_t* array = 0;

        if (key.empty()) {
            array = m_Json;
        } else {
            if (!json_is_object(m_Json)) return false;
            array = json_object_get(m_Json, key.c_str());
            if (array == 0) return false;
        }

        if (!json_is_array(array)) return false;
        int sz = json_array_size(array);
        if (sz == 0) return true;

        for (int i = 0; i < sz; ++i) {
            if (limit != DEFAULT_LIMIT_GET_COLLECTION && i == (limit - 1)) {
                break;
            }

            /* each row in the arary will be one serializer object.*/
            json_t* json = json_array_get(array, i);
            if (json == 0) return false;
            if (json && json_is_string(json)) {
                collection.insert(json_string_value(json));
            } else
                return false;
        }
        return true;
    }

    return false;
}

/*!
 * PutStringCollection function. Given a string key, this function nests the key
 * and the array of strings passed into the current json struct.
 * @param const reference to string which is the key to add in the current json
 * struct.
 * @param reference to a set of strings. The passed array of strings is nested
 * under the key passed.
 * @return bool. True
 * */
bool JsonSerializer::PutStringCollection(
    const std::string& key, const std::set<std::string>& collection,
    int limit) {
    if (m_Json && key.empty()) return false;
    if (!m_Json && !key.empty()) return false;
    if (m_Json && !json_is_object(m_Json)) return false;

    size_t sz = ((limit <= DEFAULT_LIMIT_GET_COLLECTION) ||
                 (limit > (int)collection.size()))
                    ? collection.size()
                    : (size_t)limit;

    json_t* array = json_array();
    if (array == 0) return false;

    std::set<std::string>::const_iterator iter;
    size_t i = 0;
    int ret = 0;
    for (iter = collection.begin(); i < sz; ++i, ++iter) {
        ret = json_array_append_new(array, json_string(iter->c_str()));
        if (ret == -1) {
            json_decref(array);
            return false;
        }
    }

    if (!m_Json && key.empty()) {
        m_Json = array;
        return m_Json ? true : false;
    }

    ret = json_object_set_new(m_Json, key.c_str(), array);
    if (ret == -1) return false;

    return true;
}

/*!
 * StreamJsonToBuffer function.This function streams the member json object
 * into a null terminated string in which the data is json formatted.
 * @return char*. Pointer to a buffer containing the streamed json formatted
 * null terminated data.
 * NOTE: users need to free the pointer returned. ....use free not delete....
 * */
char* JsonSerializer::StreamJsonToBuffer() const {
    if (m_Json) {
        return json_dumps(m_Json, JSON_ENCODE_ANY | JSON_COMPACT);
    }
    return NULL;
}
