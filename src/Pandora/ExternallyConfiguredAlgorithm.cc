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
    ExternalParametersMap::iterator iter(m_externalParametersMap.find(&(this->GetPandora())));

    if (m_externalParametersMap.end() != iter)
    {
        delete iter->second;
        m_externalParametersMap.erase(iter);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode ExternallyConfiguredAlgorithm::SetExternalParameters(const Pandora &pandora, ExternalParameters *const pExternalParameters)
{
    if (!pExternalParameters)
        return STATUS_CODE_INVALID_PARAMETER;

    if (!m_externalParametersMap.insert(ExternalParametersMap::value_type(&pandora, pExternalParameters)).second)
    {
        delete pExternalParameters;
        return STATUS_CODE_ALREADY_PRESENT;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool ExternallyConfiguredAlgorithm::ExternalParametersPresent() const
{
    return (m_externalParametersMap.count(&(this->GetPandora())) > 0);
}

//------------------------------------------------------------------------------------------------------------------------------------------

ExternallyConfiguredAlgorithm::ExternalParameters *ExternallyConfiguredAlgorithm::GetExternalParameters() const
{
    if (!this->ExternalParametersPresent())
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_externalParametersMap.at(&(this->GetPandora()));
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
