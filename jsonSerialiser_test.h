/*!
 * @file   : "JSonSerializerTest.h"
 * @brief  : Unit tests for JSonSerializer class. It will  check each
 *           module with positive and negative tests. It has around 23 tests.
 * @author : nitin deokate
 * @date   : Sept/2013
  $Id$
 */

#include "cxxtest/TestSuite.h"
#include "common/qappframework/JSonSerializer.h"
#include "common/qappframework/Utils.h"
#include "common/qappframework/Logger.h"
#include <vector>
#include <set>
#include <algorithm>
#include <stdio.h>
#include <iostream>
#include <fstream>

using std::string;

class JSonSerializerTest : public CxxTest::TestSuite {

   public:
    void testParsePositive();
    void testParseNegative();
    void testCreateRootObject();
    void testGetValuePositive1();
    void testGetValueNegative1();
    void testGetValuePositive2();
    void testGetValueNegative2();
    void testGetObjectPositive1();
    void testGetObjectNegative1();
    void testGetObjectPositive2();
    void testGetObjectNegative2();
    void testGetObjectNegative3();
    void testStreamJsonToBufferPositive();
    void testStreamJsonToBufferNegative();
    void testCopyConstructor();
    void testAssignmentOp();
    void testPutValue();
    void testPutCollection();
    void testGetCollection();
    void testGetStringCollectionPositive1();
    void testGetStringCollectionPositive2();
    void testPutStringCollectionPositive1();
    void testPutStringCollectionPositive2();

   private:
    static const string m_kStrval;
    static const string m_kWrongval;
    static const string m_kTeststr;
};

/* Testing String
 * These strings are needed testing
 */
const string JSonSerializerTest::m_kStrval =
    "{\"dbtype\":\"mongo\",\"mongo\":{\"hostip\":\"127.0.0.1\",\"port\":"
    "\"30000\",\"WC\":\"1\"}}";
const string JSonSerializerTest::m_kWrongval = "\"30000\",\"WC\": \"1\" }} ";
const string JSonSerializerTest::m_kTeststr =
    "{\"test\":[{\"0\":\"1d43965f-3871-4ba0-a640-e306678989c2\"},{\"1\":"
    "\"914e273a-0a2c-4716-a21d-f1a783d534a1\"},{\"2\":\"8551a577-b9bb-4724-"
    "aa4e-b0abac71d9da\"},{\"3\":\"7b8f16aa-25d2-44c4-b2f2-18828492fc62\"},{"
    "\"4\":\"17b95552-401c-4842-8fc5-f57e5d5a2b00\"},{\"5\":\"bf4c336b-da18-"
    "47a3-9f19-7c690a2cd96b\"},{\"6\":\"6e2b003e-44bc-4cd8-a525-9b1ef3e6df47\"}"
    ",{\"7\":\"5f1537e7-7946-48f1-9e4d-8bb7a6ebe976\"},{\"8\":\"dcff72de-ae70-"
    "4b51-a241-de89b58d1f76\"},{\"9\":\"846fe197-7ad8-4794-b2e6-ee284045d93b\"}"
    "]}";

/*
 * Test1
 * Method: Parse()
 * This test will check whether passed stream is in correct format.
 * Parse returns true on success and false on failure.
 * This test is positive test when stream is correct and returns true.
 */

void JSonSerializerTest::testParsePositive() {
    JsonSerializer objJson;
    TS_ASSERT(objJson.Parse(m_kStrval));
}

/*
 * Test2
 * Method: Parse()
 * This test will check whether passed stream is in correct format.
 * Parse returns true on success and false on failure.
 * This test is negative test when stream is incorrect and returns false.
 */

void JSonSerializerTest::testParseNegative() {
    JsonSerializer objJson;
    TS_ASSERT(!(objJson.Parse(m_kWrongval)));
    COMMON_LOG(3, "hello");
}

/*
 * Test3
 * Method : CreateRootObject();
 * This test will check whether it is creation root object.
 * CreateRootObject returns true on success and false on failure.
 * This is just specific to CreateRootObject, functionality tests
 * anyway will happen in other functions in other tests.
 */

void JSonSerializerTest::testCreateRootObject() {
    JsonSerializer objJson;
    TS_ASSERT(objJson.CreateRootObject());
}

/*
 * Test4
 * Method : GetValue()
 * This test will check GetValue's return status.
 * GetValue returns true on success and false on failure.
 * This is positive test when passed key is present in stream
 */

void JSonSerializerTest::testGetValuePositive1() {
    JsonSerializer objJson;
    string got_val;
    TS_ASSERT(objJson.Parse(m_kStrval));
    TS_ASSERT(objJson.GetValue("dbtype", got_val));
}

/*
 * Test5
 * Method : GetValue()
 * This test will check GetValue's return status.
 * GetValue returns true on success and false on failure.
 * This is negative test when passed key is not present in stream
 */

void JSonSerializerTest::testGetValueNegative1() {
    JsonSerializer objJson;

    string got_val;
    TS_ASSERT(objJson.Parse(m_kStrval));
    TS_ASSERT(!(objJson.GetValue("dbtype1", got_val)));
}

/*
 * Test6
 * Method : GetValue()
 * This test will check GetValue's value of the key passed.
 * GetValue returns true on success with value.
 * This is positive test when passed key is present in stream
 * and matches with expected value.
 */

void JSonSerializerTest::testGetValuePositive2() {
    JsonSerializer objJson;

    string got_val;
    string exp_val = "mongo";
    TS_ASSERT(objJson.Parse(m_kStrval));

    TS_ASSERT(objJson.GetValue("dbtype", got_val));
    TS_ASSERT_EQUALS(exp_val, got_val);
}

/*
 * Test7
 * Method : GetValue()
 * This test will check GetValue's value of the key passed.
 * This is negative test when passed key is not present in stream
 * and not matches with expected value.
 */

void JSonSerializerTest::testGetValueNegative2() {
    JsonSerializer objJson;
    bool result = false;
    string got_val;
    string exp_val = "mongo";
    TS_ASSERT(objJson.Parse(m_kStrval));
    TS_ASSERT(!(objJson.GetValue("dbtype1", got_val)));
    if (exp_val == got_val) result = true;
    TS_ASSERT(!result);
}

/*
 * Test8
 * Method : GetObject()
 * This test will check GetObject's return value.
 * GetObject() retrieves the cjson object that is nested under this key
 * This is positive test when passed key is present in stream
 */

void JSonSerializerTest::testGetObjectPositive1() {
    JsonSerializer objJson, tempJson;

    TS_ASSERT(objJson.Parse(m_kStrval));

    TS_ASSERT(objJson.GetObject("mongo", tempJson));
}

/*
 * Test9
 * Method : GetObject()
 * This test will check GetObject's return value.
 * GetObject() retrieves the cjson object that is nested under this key
 * This is negative test when passed key is not present in stream
 */

void JSonSerializerTest::testGetObjectNegative1() {
    JsonSerializer objJson, tempJson;

    TS_ASSERT(objJson.Parse(m_kStrval));

    TS_ASSERT(!(objJson.GetObject("mongo1", tempJson)));
}

/*
 * Test10
 * Method : GetObject()
 * This test will check GetObject's return value.
 * GetObject() retrieves the cjson object that is nested under this key
 * This is positive test when passed key is present in stream, it should
 * build correct object.
 */

void JSonSerializerTest::testGetObjectPositive2() {
    JsonSerializer objJson, tempJson;

    string exp_val = "127.0.0.1";
    string got_val;

    TS_ASSERT(objJson.Parse(m_kStrval));

    TS_ASSERT(objJson.GetObject("mongo", tempJson));

    TS_ASSERT(tempJson.GetValue("hostip", got_val));
    TS_ASSERT_EQUALS(exp_val, got_val);
}

/*
 * Test11
 * Method : GetObject()
 * This test will check GetObject's return value.
 * GetObject() retrieves the cjson object that is nested under this key
 * This is negative test when passed key is not present in stream, it should
 * build non-correct object.
 */

void JSonSerializerTest::testGetObjectNegative2() {
    JsonSerializer objJson, tempJson;

    string exp_val = "127.0.0.1";
    string got_val;

    TS_ASSERT(objJson.Parse(m_kStrval));

    TS_ASSERT(!(objJson.GetObject("mongo1", tempJson)));

    TS_ASSERT(!(tempJson.GetValue("hostip", got_val)));
}

/*
 * Test12
 * Method : GetObject()
 * This test will check GetObject's return value.
 * GetObject() retrieves the cjson object that is nested under this key
 * This is additional negative test for GetValue
 */

void JSonSerializerTest::testGetObjectNegative3() {
    JsonSerializer objJson, tempJson;

    string exp_val = "127.0.0.1";
    string got_val;

    TS_ASSERT(objJson.Parse(m_kStrval));

    TS_ASSERT(objJson.GetObject("mongo", tempJson));

    TS_ASSERT(!(tempJson.GetValue("hostip1", got_val)));
}

/*
 * Test13
 * Method : StreamJsonToBuffer()
 * This test will check StreamJsonToBuffer's return value.
 * streams the member cjson object into a null terminated
 * string in which the data is json formatted
 * This is positive test
 */

void JSonSerializerTest::testStreamJsonToBufferPositive() {
    JsonSerializer objJson;
    char *got_str;
    TS_ASSERT(objJson.Parse(m_kStrval));
    got_str = objJson.StreamJsonToBuffer();
    TS_ASSERT(got_str);

    // std::cout<<"string = "<<got_str;
    free(got_str);
}

/*
 * Test14
 * Method : StreamJsonToBuffer()
 * This test will check StreamJsonToBuffer's return value.
 * streams the member cjson object into a null terminated
 * string in which the data is json formatted
 * This is negative test
 */

void JSonSerializerTest::testStreamJsonToBufferNegative() {
    JsonSerializer objJson;
    TS_ASSERT(NULL == objJson.StreamJsonToBuffer());
}

/*
 * Test15
 * Method : JsonSerializer()
 * This test is to check copy constructor.
 * This is positive test
 * NOTE: this test is not needed, because we might
 *       might have done same operation while testing
 *       other functionality. This is just for sake of it
 */

void JSonSerializerTest::testCopyConstructor() {
    string got_val;
    JsonSerializer *objJson = new JsonSerializer();
    TS_ASSERT(objJson->Parse(m_kStrval));
    // it will call copy constructor
    JsonSerializer tempJson = *objJson;
    delete (objJson);
    TS_ASSERT(tempJson.GetValue("dbtype", got_val));
    // std::cout<<got_val;
}

/*
 * Test16
 * Method : JsonSerializer()
 * This test is to check assignment operator.
 * This is positive test
 * NOTE: this test is not needed, because we might
 *       might have done same operation while testing
 *       other functionality. This is just for sake of it
 */

void JSonSerializerTest::testAssignmentOp() {
    JsonSerializer objJson;
    JsonSerializer tempJson;
    string got_val;

    TS_ASSERT(objJson.Parse(m_kStrval));

    // it will call copy assignment operator
    tempJson = objJson;
    TS_ASSERT(tempJson.GetValue("dbtype", got_val));
}

/*
 * Test17
 * Method : PutValue()
 * This test is to check PutValue's return.
 * This is positive test, on success it returns true.
 */

void JSonSerializerTest::testPutValue() {
    JsonSerializer json;
    json.CreateRootObject();
    std::vector<JsonSerializer> vec;
    for (int i = 0; i < 10; ++i) {
        std::ostringstream strm;
        strm << i;
        std::string strUUID;
        Utils::GenerateUUID(strUUID);
        json.CreateRootObject();
        TS_ASSERT(json.PutValue(strm.str(), strUUID));
        vec.push_back(json);
    }
}

/*
 * Test18
 * Method : PutCollection()
 * This test is to check PutCollection's return values
 * This is positive test, on success returns true.
 */

void JSonSerializerTest::testPutCollection() {
    JsonSerializer json;
    json.CreateRootObject();
    std::vector<JsonSerializer> vec;

    for (int i = 0; i < 10; ++i) {
        std::ostringstream strm;
        strm << i;
        std::string strUUID;
        Utils::GenerateUUID(strUUID);
        JsonSerializer js;
        js.CreateRootObject();
        js.PutValue(strm.str(), strUUID);
        vec.push_back(js);
    }
    TS_ASSERT(json.PutCollection("test", vec));
}

/*
 * Test19
 * Method : GetCollection()
 * This test is to check GetCollection's return values
 * This is positive test, on success returns fetched values
 */

void JSonSerializerTest::testGetCollection() {
    JsonSerializer json;
    json.CreateRootObject();
    std::vector<JsonSerializer> vec;

    json.Parse(m_kTeststr);
    TS_ASSERT(json.GetCollection("test", vec));
    TS_ASSERT_EQUALS(10, vec.size());

    for (size_t i = 0; i < vec.size(); ++i) {
        std::ostringstream strm;
        strm << i;
        std::string strval;
        TS_ASSERT(vec[i].GetValue(strm.str(), strval));
    }
}

/*
 * Test20
 * Method : GetStringCollection()
 * This test is to check GetStringCollection's return values
 * This is positive test, on success returns true
 */

void JSonSerializerTest::testGetStringCollectionPositive1() {
    JsonSerializer json;
    json.CreateRootObject();
    std::set<std::string> vec;
    for (int i = 0; i < 10; ++i) {
        std::string strUUID;
        Utils::GenerateUUID(strUUID);
        vec.insert(strUUID);
    }
    TS_ASSERT(json.PutStringCollection("test", vec));
    TS_ASSERT(json.GetStringCollection("test", vec));
    TS_ASSERT_EQUALS(10, vec.size());
}

/*
 * Test21
 * Method : GetStringCollection()
 * This test is to check GetStringCollection's return values
 * This is positive test, on success returns true with limit
 */

void JSonSerializerTest::testGetStringCollectionPositive2() {
    JsonSerializer json;
    json.CreateRootObject();
    std::set<std::string> vec;
    for (int i = 0; i < 10; ++i) {
        std::string strUUID;
        Utils::GenerateUUID(strUUID);
        vec.insert(strUUID);
    }
    TS_ASSERT(json.PutStringCollection("test", vec, 10));
    TS_ASSERT(json.GetStringCollection("test", vec, 4));
    TS_ASSERT_EQUALS(10, vec.size());
}

/* Test22
 * Method : PutStringCollection()
 * This test is to check PutStringCollection's return values
 * This is positive test, on success returns true
 */

void JSonSerializerTest::testPutStringCollectionPositive1() {
    JsonSerializer json;
    json.CreateRootObject();
    std::set<std::string> vec;
    for (int i = 0; i < 10; ++i) {
        std::string strUUID;
        Utils::GenerateUUID(strUUID);
        vec.insert(strUUID);
    }
    TS_ASSERT(json.PutStringCollection("test", vec));
}

/* Test23
 * Method : PutStringCollection()
 * This test is to check PutStringCollection's return values
 * This is positive test, on success returns true with limit
 */
void JSonSerializerTest::testPutStringCollectionPositive2() {
    JsonSerializer json;
    json.CreateRootObject();
    std::set<std::string> vec;
    for (int i = 0; i < 10; ++i) {
        std::string strUUID;
        Utils::GenerateUUID(strUUID);
        vec.insert(strUUID);
    }

    //   TS_ASSERT(json.PutStringCollection("test", vec,5));

    TS_ASSERT(json.PutStringCollection("test", vec, -15));
}
