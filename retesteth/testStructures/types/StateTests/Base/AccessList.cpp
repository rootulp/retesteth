#include "AccessList.h"
#include <libdevcore/Address.h>
#include <retesteth/testStructures/Common.h>

namespace test
{
namespace teststruct
{
AccessListElement::AccessListElement(DataObject const& _data)
{
    REQUIRE_JSONFIELDS(_data, "AccessListElement " + _data.getKey(),
        {{"address", {{DataType::String}, jsonField::Required}},
         {"storageKeys", {{DataType::Array}, jsonField::Required}}});

    m_address = spFH20(new FH20(_data.atKey("address")));
    for (auto const& el : _data.atKey("storageKeys").getSubObjects())
        m_storageKeys.push_back(spFH32(new FH32(dev::toCompactHexPrefixed(dev::u256(el->asString()), 32))));
}

AccessList::AccessList(DataObject const& _data)
{
    for (auto const& el : _data.getSubObjects())
        m_list.push_back(spAccessListElement(new AccessListElement(el)));
}

spDataObject AccessList::asDataObject() const
{
    spDataObject accessList(new DataObject(DataType::Array));
    for (auto const& el : m_list)
    {
        spDataObject accessListElement;
        (*accessListElement)["address"] = el->address().asString();
        spDataObject keys(new DataObject(DataType::Array));
        (*accessListElement).atKeyPointer("storageKeys") = keys;
        for (auto const& el2 : el->keys())
        {
            spDataObject k(new DataObject(el2->asString()));
            (*accessListElement)["storageKeys"].addArrayObject(k);
        }
        (*accessList).addArrayObject(accessListElement);
    }
    return accessList;
}

const dev::RLPStream AccessListElement::asRLPStream() const
{
    dev::RLPStream stream(2);
    stream << dev::Address(m_address->asString());

    dev::RLPStream storages(m_storageKeys.size());
    for (auto const& key : m_storageKeys)
        storages << dev::h256(key->asString());
    // storages << dev::u256(key.asString());

    stream.appendRaw(storages.out());
    return stream;
}

AccessListElement::AccessListElement(dev::RLP const& _rlp)
{
    size_t i = 0;
    m_address = spFH20(new FH20(_rlp[i++]));
    auto const& rlplist = _rlp[i++].toList();
    for (auto const& key : rlplist)
        m_storageKeys.push_back(spFH32(new FH32(key)));
}

AccessList::AccessList(dev::RLP const& _rlp)
{
    for (auto const& accList : _rlp.toList())
        m_list.push_back(spAccessListElement(new AccessListElement(accList)));
}

}  // namespace teststruct
}  // namespace test
