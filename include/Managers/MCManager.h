/**
 *  @file   PandoraSDK/include/Managers/MCManager.h
 * 
 *  @brief  Header file for the mc particle manager class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_MC_MANAGER_H
#define PANDORA_MC_MANAGER_H 1

#include "Api/PandoraApi.h"

#include "Managers/InputObjectManager.h"

#include "Pandora/PandoraInternal.h"

namespace pandora
{

/**
 *    @brief MCManager class
 */
class MCManager : public InputObjectManager<MCParticle>
{
public:
    /**
     *  @brief  Constructor
     * 
     *  @param  pPandora address of the associated pandora object
     */
    MCManager(const Pandora *const pPandora);

    /**
     *  @brief  Destructor
     */
    ~MCManager();

private:
    /**
     *  @brief  Create a mc particle
     * 
     *  @param  parameters the mc particle parameters
     *  @param  pMCParticle address of the mc particle
     */
    StatusCode CreateMCParticle(const PandoraApi::MCParticle::Parameters &parameters, MCParticle *&pMCParticle);

    /**
     *  @brief  Erase all mc manager content
     */
    StatusCode EraseAllContent();

    /**
     *  @brief  Set mc particle relationship
     * 
     *  @param  parentUid the parent unique identifier
     *  @param  daughterUid the daughter unique identifier
     */
    StatusCode SetMCParentDaughterRelationship(const Uid parentUid, const Uid daughterUid);

    /**
     *  @brief  Set calo hit to mc particle relationship
     * 
     *  @param  caloHitUid the calo hit unique identifier
     *  @param  mcParticleUid the mc particle unique identifier
     *  @param  mcParticleWeight weighting to assign to the mc particle
     */
    StatusCode SetCaloHitToMCParticleRelationship(const Uid caloHitUid, const Uid mcParticleUid, const float mcParticleWeight);

    /**
     *  @brief  Set track to mc particle relationship
     * 
     *  @param  trackUid the track unique identifier
     *  @param  mcParticleUid the mc particle unique identifier
     *  @param  mcParticleWeight weighting to assign to the mc particle
     */
    StatusCode SetTrackToMCParticleRelationship(const Uid trackUid, const Uid mcParticleUid, const float mcParticleWeight);

    /**
     *  @brief  Identify pfo targets
     */
    StatusCode IdentifyPfoTargets();

    /**
     *  @brief  Select pfo targets
     */
    StatusCode SelectPfoTargets();

   /**
    *  @brief  Apply mc pfo selection rules 
    *
    *  @params mcRootParticle address of the mc root particle
    *  @params mcPfoList reference to list with all MCPFOs which have been selected so far
    */
    StatusCode ApplyPfoSelectionRules(MCParticle *const mcRootParticle, MCParticleList &mcPfoList) const;

   /**
     *  @brief  Create a map relating calo hit uid to mc pfo target
     * 
     *  @param  caloHitToPfoTargetMap to receive the calo hit uid to mc pfo target map
     */
    StatusCode CreateCaloHitToPfoTargetsMap(UidToMCParticleWeightMap &caloHitToPfoTargetsMap) const;

   /**
     *  @brief  Create a map relating track uid to mc pfo target
     * 
     *  @param  trackToPfoTargetMap to receive the track uid to mc pfo target map
     */
    StatusCode CreateTrackToPfoTargetsMap(UidToMCParticleWeightMap &trackToPfoTargetsMap) const;

    /**
     *  @brief  Apply mc particle associations (parent-daughter) that have been registered with the mc manager
     */
    StatusCode AddMCParticleRelationships() const;

    /**
     *  @brief  Remove all mc particle associations that have been registered with the mc manager
     */
    StatusCode RemoveAllMCParticleRelationships();

    /**
     *  @brief  Remove all parent/daughter particle links from a mc particle and from its (previously) linked particles
     * 
     *  @param  pMCParticle address of the mc particle
     */
    StatusCode RemoveMCParticleRelationships(MCParticle *const pMCParticle) const;

    typedef std::map<Uid, float> UidToWeightMap;
    typedef std::map<Uid, UidToWeightMap> ObjectRelationMap;
    typedef std::multimap<Uid, Uid> MCParticleRelationMap;

    /**
     *  @brief  Set an object (e.g. calo hit or track) to mc particle relationship
     * 
     *  @param  uid the unique identifier of the object
     *  @param  mcParticleUid the mc particle unique identifier
     *  @param  mcParticleWeight weighting to assign to the mc particle
     *  @param  objectRelationMap the uid relation map to populate
     */
    StatusCode SetUidToMCParticleRelationship(const Uid objectUid, const Uid mcParticleUid, const float mcParticleWeight,
        ObjectRelationMap &objectRelationMap) const;

   /**
     *  @brief  Create a map relating an object (calo hit or track) uid to mc pfo targets
     * 
     *  @param  uidToMCParticleWeightMap to receive the calo hit uid to mc pfo target map
     *  @param  objectRelationMap the object relation map containing the information
     */
    StatusCode CreateUidToPfoTargetsMap(UidToMCParticleWeightMap &uidToMCParticleWeightMap, const ObjectRelationMap &objectRelationMap) const;

    UidToMCParticleMap              m_uidToMCParticleMap;               ///< The uid to mc particle map
    MCParticleRelationMap           m_parentDaughterRelationMap;        ///< The mc particle parent-daughter relation map
    ObjectRelationMap               m_caloHitToMCParticleMap;           ///< The calo hit to mc particle relation map
    ObjectRelationMap               m_trackToMCParticleMap;             ///< The track to mc particle relation map

    static const std::string        SELECTED_LIST_NAME;                 ///< The name of the input list

    friend class PandoraApiImpl;
    friend class PandoraContentApiImpl;
    friend class PandoraImpl;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode MCManager::SetCaloHitToMCParticleRelationship(const Uid caloHitUid, const Uid mcParticleUid, const float mcParticleWeight)
{
    return this->SetUidToMCParticleRelationship(caloHitUid, mcParticleUid, mcParticleWeight, m_caloHitToMCParticleMap);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode MCManager::SetTrackToMCParticleRelationship(const Uid trackUid, const Uid mcParticleUid, const float mcParticleWeight)
{
    return this->SetUidToMCParticleRelationship(trackUid, mcParticleUid, mcParticleWeight, m_trackToMCParticleMap);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode MCManager::CreateCaloHitToPfoTargetsMap(UidToMCParticleWeightMap &caloHitToPfoTargetsMap) const
{
    return this->CreateUidToPfoTargetsMap(caloHitToPfoTargetsMap, m_caloHitToMCParticleMap);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode MCManager::CreateTrackToPfoTargetsMap(UidToMCParticleWeightMap &trackToPfoTargetsMap) const
{
    return this->CreateUidToPfoTargetsMap(trackToPfoTargetsMap, m_trackToMCParticleMap);
}

} // namespace pandora

#endif // #ifndef PANDORA_MC_MANAGER_H
