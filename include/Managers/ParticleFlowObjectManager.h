/**
 *  @file   PandoraSDK/include/Managers/ParticleFlowObjectManager.h
 * 
 *  @brief  Header file for the particle flow object manager class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_PARTICLE_FLOW_OBJECT_MANAGER_H
#define PANDORA_PARTICLE_FLOW_OBJECT_MANAGER_H 1

#include "Api/PandoraContentApi.h"

#include "Managers/AlgorithmObjectManager.h"

#include "Pandora/PandoraInternal.h"

namespace pandora
{

/**
 *  @brief  ParticleFlowObjectManager class
 */
class ParticleFlowObjectManager : public AlgorithmObjectManager<ParticleFlowObject>
{
    public:
    /**
     *  @brief  Constructor
     * 
     *  @param  pPandora address of the associated pandora object
     */
     ParticleFlowObjectManager(const Pandora *const pPandora);

    /**
     *  @brief  Destructor
     */
     ~ParticleFlowObjectManager();

private:
    /**
     *  @brief  Create a particle flow object
     * 
     *  @param  parameters the particle flow object parameters
     *  @param  pPfo to receive the address of the particle flow object created
     */
    StatusCode CreateParticleFlowObject(const PandoraContentApi::ParticleFlowObject::Parameters &parameters, ParticleFlowObject *&pPfo);

    /**
     *  @brief  Add an object to a particle flow object
     *
     *  @param  pPfo address of the particle flow object to modify
     *  @param  pT address of the object to add
     */
    template <typename T>
    StatusCode AddToPfo(ParticleFlowObject *pPfo, T *pT) const;

    /**
     *  @brief  Remove an object from a particle flow object
     *
     *  @param  pPfo address of the particle flow object to modify
     *  @param  pT address of the object to remove
     */
    template <typename T>
    StatusCode RemoveFromPfo(ParticleFlowObject *pPfo, T *pT) const;

    /**
     *  @brief  Add a parent-daughter particle flow object relationship
     *
     *  @param  pParentPfo address of parent particle flow object
     *  @param  pDaughterPfo address of daughter particle flow object
     */
    StatusCode SetParentDaughterAssociation(ParticleFlowObject *pParentPfo, ParticleFlowObject *pDaughterPfo) const;

    /**
     *  @brief  Remove a parent-daughter particle flow object relationship
     *
     *  @param  pParentPfo address of parent particle flow object
     *  @param  pDaughterPfo address of daughter particle flow object
     */
    StatusCode RemoveParentDaughterAssociation(ParticleFlowObject *pParentPfo, ParticleFlowObject *pDaughterPfo) const;

    friend class PandoraApiImpl;
    friend class PandoraContentApiImpl;
    friend class PandoraImpl;
};

} // namespace pandora

#endif // #ifndef PANDORA_PARTICLE_FLOW_OBJECT_MANAGER_H
