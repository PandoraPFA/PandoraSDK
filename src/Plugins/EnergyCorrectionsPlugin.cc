/**
 *  @file   PandoraSDK/src/Plugins/EnergyCorrectionsPlugin.cc
 * 
 *  @brief  Implementation of the energy corrections helper class.
 * 
 *  $Log: $
 */

#include "Helpers/XmlHelper.h"

#include "Objects/Cluster.h"

#include "Plugins/EnergyCorrectionsPlugin.h"

namespace pandora
{

StatusCode EnergyCorrections::MakeEnergyCorrections(const Cluster *const pCluster, float &correctedElectromagneticEnergy,
    float &correctedHadronicEnergy) const
{
    correctedHadronicEnergy = pCluster->GetHadronicEnergy();

    for (const EnergyCorrectionPlugin *const pPlugin : m_hadEnergyCorrectionPlugins)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pPlugin->MakeEnergyCorrections(pCluster, correctedHadronicEnergy));
    }

    correctedElectromagneticEnergy = pCluster->GetElectromagneticEnergy();

    for (const EnergyCorrectionPlugin *const pPlugin : m_emEnergyCorrectionPlugins)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pPlugin->MakeEnergyCorrections(pCluster, correctedElectromagneticEnergy));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

EnergyCorrections::EnergyCorrections(const Pandora *const pPandora) :
    m_pPandora(pPandora)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

EnergyCorrections::~EnergyCorrections()
{
    for (const EnergyCorrectionPluginMap::value_type &mapEntry : m_hadEnergyCorrectionPluginMap)
        delete mapEntry.second;

    for (const EnergyCorrectionPluginMap::value_type &mapEntry : m_emEnergyCorrectionPluginMap)
        delete mapEntry.second;

    m_hadEnergyCorrectionPluginMap.clear();
    m_emEnergyCorrectionPluginMap.clear();
    m_hadEnergyCorrectionPlugins.clear();
    m_emEnergyCorrectionPlugins.clear();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode EnergyCorrections::RegisterPlugin(const std::string &name, const EnergyCorrectionType energyCorrectionType,
    EnergyCorrectionPlugin *const pEnergyCorrectionPlugin)
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, pEnergyCorrectionPlugin->RegisterDetails(m_pPandora, name, name));

    EnergyCorrectionPluginMap &energyCorrectionPluginMap(this->GetEnergyCorrectionPluginMap(energyCorrectionType));

    if (!energyCorrectionPluginMap.insert(EnergyCorrectionPluginMap::value_type(name, pEnergyCorrectionPlugin)).second)
        return STATUS_CODE_ALREADY_PRESENT;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode EnergyCorrections::InitializePlugins(const TiXmlHandle *const pXmlHandle)
{
    for (EnergyCorrectionPluginMap::value_type &mapEntry : m_hadEnergyCorrectionPluginMap)
    {
        TiXmlElement *const pXmlElement(pXmlHandle->FirstChild(mapEntry.first).Element());
        if (nullptr != pXmlElement)
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, mapEntry.second->ReadSettings(TiXmlHandle(pXmlElement)));

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, mapEntry.second->Initialize());
    }

    for (EnergyCorrectionPluginMap::value_type &mapEntry : m_emEnergyCorrectionPluginMap)
    {
        TiXmlElement *const pXmlElement(pXmlHandle->FirstChild(mapEntry.first).Element());

        if (nullptr != pXmlElement)
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, mapEntry.second->ReadSettings(TiXmlHandle(pXmlElement)));

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, mapEntry.second->Initialize());
    }

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->InitializePlugin(pXmlHandle,
        "HadronicEnergyCorrectionPlugins", HADRONIC, m_hadEnergyCorrectionPlugins));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->InitializePlugin(pXmlHandle,
        "ElectromagneticEnergyCorrectionPlugins", ELECTROMAGNETIC, m_emEnergyCorrectionPlugins));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode EnergyCorrections::InitializePlugin(const TiXmlHandle *const pXmlHandle, const std::string &xmlTagName,
    const EnergyCorrectionType energyCorrectionType, EnergyCorrectionPluginVector &energyCorrectionPluginVector)
{
    if (!energyCorrectionPluginVector.empty())
        return STATUS_CODE_FAILURE;

    StringVector requestedPluginNames;
    PANDORA_RETURN_RESULT_IF_AND_IF(STATUS_CODE_SUCCESS, STATUS_CODE_NOT_FOUND, !=, XmlHelper::ReadVectorOfValues(*pXmlHandle,
        xmlTagName, requestedPluginNames));

    EnergyCorrectionPluginMap &energyCorrectionPluginMap(this->GetEnergyCorrectionPluginMap(energyCorrectionType));

    for (const std::string &requestedPluginName : requestedPluginNames)
    {
        EnergyCorrectionPluginMap::const_iterator mapIter = energyCorrectionPluginMap.find(requestedPluginName);

        if (energyCorrectionPluginMap.end() == mapIter)
            return STATUS_CODE_NOT_FOUND;

        energyCorrectionPluginVector.push_back(mapIter->second);
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

EnergyCorrections::EnergyCorrectionPluginMap &EnergyCorrections::GetEnergyCorrectionPluginMap(const EnergyCorrectionType energyCorrectionType)
{
    switch (energyCorrectionType)
    {
    case HADRONIC:
        return m_hadEnergyCorrectionPluginMap;

    case ELECTROMAGNETIC:
        return m_emEnergyCorrectionPluginMap;

    default:
        throw StatusCodeException(STATUS_CODE_INVALID_PARAMETER);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode EnergyCorrections::ResetForNextEvent()
{
    for (const EnergyCorrectionPluginMap::value_type &mapEntry : m_hadEnergyCorrectionPluginMap)
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, mapEntry.second->Reset());

    for (const EnergyCorrectionPluginMap::value_type &mapEntry : m_emEnergyCorrectionPluginMap)
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, mapEntry.second->Reset());

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
