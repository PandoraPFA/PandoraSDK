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

ExternallyConfiguredAlgorithm::~ExternallyConfiguredAlgorithm()
{
    if (this->ExternalParametersPresent())
    {
        AlgTypeToParametersMap &algTypeToParametersMap(m_externalParametersMap[&(this->GetPandora())]);
        AlgTypeToParametersMap::iterator iter(algTypeToParametersMap.find(this->GetType()));

        if (algTypeToParametersMap.end() != iter)
        {
            delete iter->second;
            algTypeToParametersMap.erase(iter);
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ExternallyConfiguredAlgorithm::SetExternalParameters(const Pandora &pandora, const std::string &algorithmType, ExternalParameters *const pExternalParameters)
{
    if (!pExternalParameters)
        return STATUS_CODE_INVALID_PARAMETER;

    if (!m_externalParametersMap[&pandora].insert(AlgTypeToParametersMap::value_type(algorithmType, pExternalParameters)).second)
        return STATUS_CODE_ALREADY_PRESENT;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ExternallyConfiguredAlgorithm::ExternalParametersPresent() const
{
    const Pandora *const pPandora(&(this->GetPandora()));
    return ((m_externalParametersMap.count(pPandora) > 0) && (m_externalParametersMap.at(pPandora).count(this->GetType()) > 0));
}

//------------------------------------------------------------------------------------------------------------------------------------------

ExternallyConfiguredAlgorithm::ExternalParameters *ExternallyConfiguredAlgorithm::GetExternalParameters() const
{
    if (!this->ExternalParametersPresent())
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_externalParametersMap.at(&(this->GetPandora())).at(this->GetType());
}

//------------------------------------------------------------------------------------------------------------------------------------------

void ExternallyConfiguredAlgorithm::RegisterParameterAccessAttempt()
{
    ExternallyConfiguredAlgorithm::GetExternalParameters()->RegisterParameterAccessAttempt();
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

ExternallyConfiguredAlgorithm::ExternalParameters::ExternalParameters() :
    m_nParameterAccessAttempts(0)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

ExternallyConfiguredAlgorithm::ExternalParameters::~ExternalParameters()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

void ExternallyConfiguredAlgorithm::ExternalParameters::RegisterParameterAccessAttempt()
{
    if (++m_nParameterAccessAttempts > 1)
        throw StatusCodeException(STATUS_CODE_NOT_ALLOWED);
}

} // namespace pandora
