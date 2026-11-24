#include "core/properties/propertyid.h"

#include <algorithm>
#include <cassert>

namespace core
{

PropertyId::Registry& PropertyId::getRegistry()
{
    // Property IDs in other translation units register during static initialization.
    // Construct their registry on first use, independently of linker object order.
    static Registry registry;
    return registry;
}

PropertyId::PropertyId(size_t internalId) : m_internalId(internalId)
{
}

size_t PropertyId::getInternalId() const
{
    return m_internalId;
}

const PropertyId::PropertyData& PropertyId::getPropertyData() const
{
    auto& registry = getRegistry();
    assert(m_internalId < registry.allPropertyIds.size());
    assert(m_internalId < registry.allPropertyData.size());
    assert(m_internalId == registry.allPropertyIds[m_internalId].m_internalId);

    return registry.allPropertyData[m_internalId];
}

const std::string& PropertyId::getIdString() const
{
    return getPropertyData().idString;
}

const std::string& PropertyId::getInfo() const
{
    return getPropertyData().info;
}

const Version& PropertyId::getVersion() const
{
    return getPropertyData().version;
}

PropertyId PropertyId::createPropertyId(const std::string& idString,
                                        const std::string& info,
                                        const Version& version)
{
    auto& registry = getRegistry();
    assert(!idString.empty());
    assert(registry.allPropertyData.size() == registry.allPropertyIds.size());

    const auto internalId = registry.allPropertyIds.size();
    const PropertyId propertyId(internalId);

    assert(!std::binary_search(registry.allPropertyIds.begin(), registry.allPropertyIds.end(), propertyId));
    assert(registry.idStringToInternalId.find(idString) == registry.idStringToInternalId.end() && "id string duplicity!");

    registry.allPropertyIds.push_back(propertyId);
    registry.allPropertyData.emplace_back(idString, info, version);

    registry.idStringToInternalId.emplace(idString, internalId);

    return propertyId;
}

std::optional<PropertyId> PropertyId::getPropertyIdByInternalId(size_t internalId)
{
    auto& registry = getRegistry();
    if (internalId < registry.allPropertyIds.size())
    {
        auto propertyId = registry.allPropertyIds[internalId];
        assert(propertyId.m_internalId == internalId);
        return propertyId;
    }
    return std::nullopt;
}

std::optional<PropertyId> PropertyId::getPropertyIdByIdString(const std::string& idString)
{
    auto& registry = getRegistry();
    const auto it = registry.idStringToInternalId.find(idString);
    if (it != registry.idStringToInternalId.end())
    {
        return getPropertyIdByInternalId(it->second);
    }
    return std::nullopt;
}

const std::vector<PropertyId>& PropertyId::getAllPropertyIds()
{
    auto& registry = getRegistry();
    return registry.allPropertyIds;
}

} // namespace core
