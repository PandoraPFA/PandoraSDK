/**
 *  @file   PandoraSDK/src/Objects/MCParticle.cc
 * 
 *  @brief  Implementation of the mc particle class.
 * 
 *  $Log: $
 */

#include "Objects/MCParticle.h"

#include <algorithm>

namespace pandora
{

const MCParticle *MCParticle::GetPfoTarget() const
{
    if (!m_pPfoTarget)
        throw StatusCodeException(STATUS_CODE_NOT_INITIALIZED);

    return m_pPfoTarget;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool MCParticle::operator< (const MCParticle &rhs) const
{
    const CartesianVector deltaPosition(rhs.GetVertex() - this->GetVertex());

    if (std::fabs(deltaPosition.GetZ()) > std::numeric_limits<float>::epsilon())
        return (deltaPosition.GetZ() > std::numeric_limits<float>::epsilon());

    if (std::fabs(deltaPosition.GetX()) > std::numeric_limits<float>::epsilon())
        return (deltaPosition.GetX() > std::numeric_limits<float>::epsilon());

    if (std::fabs(deltaPosition.GetY()) > std::numeric_limits<float>::epsilon())
        return (deltaPosition.GetY() > std::numeric_limits<float>::epsilon());

    return (this->GetEnergy() > rhs.GetEnergy());
}

//------------------------------------------------------------------------------------------------------------------------------------------

MCParticle::MCParticle(const object_creation::MCParticle::Parameters &parameters) :
    m_uid(parameters.m_pParentAddress.Get()),
    m_energy(parameters.m_energy.Get()),
    m_momentum(parameters.m_momentum.Get()),
    m_vertex(parameters.m_vertex.Get()),
    m_endpoint(parameters.m_endpoint.Get()),
    m_innerRadius(parameters.m_vertex.Get().GetMagnitude()),
    m_outerRadius(parameters.m_endpoint.Get().GetMagnitude()),
    m_particleId(parameters.m_particleId.Get()),
    m_mcParticleType(parameters.m_mcParticleType.Get()),
    m_pPfoTarget(nullptr)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

MCParticle::~MCParticle()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCParticle::AddDaughter(const MCParticle *const pMCParticle)
{
    if (m_daughterList.end() != std::find(m_daughterList.begin(), m_daughterList.end(), pMCParticle))
        return STATUS_CODE_ALREADY_PRESENT;

    m_daughterList.push_back(pMCParticle);
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCParticle::AddParent(const MCParticle *const pMCParticle)
{
    if (m_parentList.end() != std::find(m_parentList.begin(), m_parentList.end(), pMCParticle))
        return STATUS_CODE_ALREADY_PRESENT;

    m_parentList.push_back(pMCParticle);
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCParticle::RemoveDaughter(const MCParticle *const pMCParticle)
{
    MCParticleList::iterator iter = std::find(m_daughterList.begin(), m_daughterList.end(), pMCParticle);

    if (m_daughterList.end() == iter)
        return STATUS_CODE_NOT_FOUND;

    m_daughterList.erase(iter);
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCParticle::RemoveParent(const MCParticle *const pMCParticle)
{
    MCParticleList::iterator iter = std::find(m_parentList.begin(), m_parentList.end(), pMCParticle);

    if (m_parentList.end() == iter)
        return STATUS_CODE_NOT_FOUND;

    m_parentList.erase(iter);
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCParticle::SetPfoTarget(const MCParticle *const pMCParticle)
{
    if (!pMCParticle)
        return STATUS_CODE_FAILURE;

    m_pPfoTarget = pMCParticle;
    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCParticle::RemovePfoTarget()
{
    m_pPfoTarget = nullptr;
    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
