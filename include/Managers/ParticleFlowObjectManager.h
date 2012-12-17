/**
 *  @file   PandoraPFANew/Framework/include/Managers/ParticleFlowObjectManager.h
 * 
 *  @brief  Header file for the particle flow object manager class.
 * 
 *  $Log: $
 */
#ifndef PARTICLE_FLOW_OBJECT_MANAGER_H
#define PARTICLE_FLOW_OBJECT_MANAGER_H 1

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
     *  @brief  Default constructor
     */
     ParticleFlowObjectManager();

    /**
     *  @brief  Destructor
     */
     ~ParticleFlowObjectManager();

private:
    /**
     *  @brief  Create a particle flow object
     * 
     *  @param  particleFlowObjectParameters the particle flow object parameters
     */
    StatusCode CreateParticleFlowObject(const PandoraContentApi::ParticleFlowObjectParameters &particleFlowObjectParameters);

    /**
     *  @brief  Add a cluster to a particle flow object
     *
     *  @param  pPfo address of the particle flow object to modify
     *  @param  pCluster address of the cluster to add
     */
    StatusCode AddClusterToPfo(ParticleFlowObject *pPfo, Cluster *pCluster) const;

    /**
     *  @brief  Add a track to a particle flow object
     *
     *  @param  pPfo address of the particle flow object to modify
     *  @param  pTrack address of the track to add
     */
    StatusCode AddTrackToPfo(ParticleFlowObject *pPfo, Track *pTrack) const;

    /**
     *  @brief  Remove a cluster from a particle flow object. Note this function will not remove the final object (track or cluster)
     *          from a particle flow object, and will instead return status code "not allowed" as a prompt to delete the cluster
     *
     *  @param  algorithm the algorithm calling this function
     *  @param  pPfo address of the particle flow object to modify
     *  @param  pCluster address of the cluster to remove
     */
    StatusCode RemoveClusterFromPfo(ParticleFlowObject *pPfo, Cluster *pCluster);

    /**
     *  @brief  Remove a track from a particle flow object. Note this function will not remove the final object (track or cluster)
     *          from a particle flow object, and will instead return status code "not allowed" as a prompt to delete the cluster
     *
     *  @param  pPfo address of the particle flow object to modify
     *  @param  pTrack address of the track to remove
     */
    StatusCode RemoveTrackFromPfo(ParticleFlowObject *pPfo, Track *pTrack);

    friend class PandoraApiImpl;
    friend class PandoraContentApiImpl;
    friend class PandoraImpl;
};

} // namespace pandora

#endif // #ifndef PARTICLE_FLOW_OBJECT_MANAGER_H
