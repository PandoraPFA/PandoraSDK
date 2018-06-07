/**
 *  @file   PandoraSDK/src/Pandora/ExternallyConfiguredAlgorithm.h
 *
 *  @brief  Implementation of the externally configured algorithm class.
 *
 *  $Log: $
 */

#include "Pandora/ExternallyConfiguredAlgorithm.h"

namespace pandora
{

ExternallyConfiguredAlgorithm::ExternalParametersMap ExternallyConfiguredAlgorithm::m_externalParametersMap;

//------------------------------------------------------------------------------------------------------------------------------------------

ExternalParameters::ExternalParameters() :
    m_nParameterAccessAttempts(0)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

ExternalParameters::~ExternalParameters()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

void ExternalParameters::RegisterParameterAccessAttempt()
{
    if (++m_nParameterAccessAttempts > 1)
        throw StatusCodeException(STATUS_CODE_NOT_ALLOWED);
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

ExternallyConfiguredAlgorithm::~ExternallyConfiguredAlgorithm()
{
    try
    {
        m_externalParametersMap.RemoveExternalParameters(this->GetPandora(), this->GetType());
    }
    catch (const StatusCodeException &) {}
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ExternallyConfiguredAlgorithm::SetExternalParameters(const Pandora &pandora, const std::string &algorithmType, ExternalParameters *const pExternalParameters)
{
    return m_externalParametersMap.SetExternalParameters(pandora, algorithmType, pExternalParameters);
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ExternallyConfiguredAlgorithm::ExternalParametersPresent() const
{
    return m_externalParametersMap.ExternalParametersPresent(this->GetPandora(), this->GetType());
}

//------------------------------------------------------------------------------------------------------------------------------------------

ExternalParameters *ExternallyConfiguredAlgorithm::GetExternalParameters() const
{
    ExternalParameters *const pExternalParameters(m_externalParametersMap.GetExternalParameters(this->GetPandora(), this->GetType()));
    pExternalParameters->RegisterParameterAccessAttempt();
    return pExternalParameters;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

ExternallyConfiguredAlgorithm::ExternalParametersMap::~ExternalParametersMap()
{
    for (const TheMap::value_type &pandoraMapEntry : m_theMap)
    {
        for (const AlgTypeToParametersMap::value_type &algorithmMapEntry : pandoraMapEntry.second)
            delete algorithmMapEntry.second;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ExternallyConfiguredAlgorithm::ExternalParametersMap::ExternalParametersPresent(const Pandora &pandora, const std::string &algorithmType) const
{
    return ((m_theMap.count(&pandora) > 0) && (m_theMap.at(&pandora).count(algorithmType) > 0));
}

//------------------------------------------------------------------------------------------------------------------------------------------

ExternalParameters *ExternallyConfiguredAlgorithm::ExternalParametersMap::GetExternalParameters(const Pandora &pandora, const std::string &algorithmType) const
{
    if (!this->ExternalParametersPresent(pandora, algorithmType))
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_theMap.at(&pandora).at(algorithmType);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ExternallyConfiguredAlgorithm::ExternalParametersMap::SetExternalParameters(const pandora::Pandora &pandora, const std::string &algorithmType,
    ExternalParameters *const pExternalParameters)
{
    if (!pExternalParameters)
        return STATUS_CODE_INVALID_PARAMETER;

    if (!m_theMap[&pandora].insert(AlgTypeToParametersMap::value_type(algorithmType, pExternalParameters)).second)
        return STATUS_CODE_ALREADY_PRESENT;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void ExternallyConfiguredAlgorithm::ExternalParametersMap::RemoveExternalParameters(const pandora::Pandora &pandora, const std::string &algorithmType)
{
    if (this->ExternalParametersPresent(pandora, algorithmType))
    {
        AlgTypeToParametersMap &algTypeToParametersMap(m_theMap[&pandora]);
        AlgTypeToParametersMap::iterator iter(algTypeToParametersMap.find(algorithmType));

        if (algTypeToParametersMap.end() != iter)
        {
            delete iter->second;
            algTypeToParametersMap.erase(iter);
        }
    }
}

} // namespace pandora
