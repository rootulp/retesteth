#pragma once
#include "../object.h"
#include <retesteth/Options.h>
#include <retesteth/TestHelper.h>

using namespace test;
namespace testprivate
{
class scheme_RPCTestBase : public object
{
public:
    scheme_RPCTestBase(DataObject const& _test) : object(_test) {}
    std::string get_method() { return m_data.at("request_method").asString(); }
    std::vector<std::string> get_params()
    {
        // no static here because of the threads
        std::vector<std::string> params;
        for (auto const& d : m_data.at("request_params").getSubObjects())
        {
            if (d.type() == DataType::String)
                params.push_back("\"" + d.asString() + "\"");
            else if (d.type() == DataType::Bool)
            {
                if (d == true)
                    params.push_back("true");
                else
                    params.push_back("false");
            }
        }
        return params;
    }
};
}

using namespace testprivate;
namespace test
{
class scheme_RPCTest : public scheme_RPCTestBase
{
public:
    scheme_RPCTest(DataObject const& _test) : scheme_RPCTestBase(_test)
    {
        requireJsonFields(_test, "rpcTest " + _test.getKey(),
            {{"request_method", {DataType::String}}, {"request_params", {DataType::Array}}});
    }
};

class scheme_RPCTestFiller : public scheme_RPCTestBase
{
public:
    scheme_RPCTestFiller(DataObject const& _test) : scheme_RPCTestBase(_test)
    {
        requireJsonFields(_test, "rpcTest " + _test.getKey(),
            {{"_info", {{DataType::Object}, jsonField::Optional}},
                {"request_method", {{DataType::String}, jsonField::Required}},
                {"request_params", {{DataType::Array}, jsonField::Required}},
                {"expect_return", {{DataType::Object}, jsonField::Required}}});
    }
    DataObject const& get_expectReturn() { return m_data.at("expect_return"); }
};
}
