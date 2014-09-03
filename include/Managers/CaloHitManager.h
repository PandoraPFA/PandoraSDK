/**
 *  @file   PandoraSDK/include/Managers/CaloHitManager.h
 * 
 *  @brief  Header file for the calo hit manager class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_CALO_HIT_MANAGER_H
#define PANDORA_CALO_HIT_MANAGER_H 1

#include "Api/PandoraApi.h"

#include "Managers/InputObjectManager.h"
#include "Managers/Metadata.h"

#include "Objects/CaloHit.h"

#include "Pandora/PandoraInternal.h"

namespace pandora
{

/**
 *  @brief  CaloHitManager class
 */
class CaloHitManager : public InputObjectManager<CaloHit>
{
public:
    /**
     *  @brief  Constructor
     * 
     *  @param  pPandora address of the associated pandora object
     */
    CaloHitManager(const Pandora *const pPandora);

     /**
     *  @brief  Destructor
     */
    ~CaloHitManager();

private:
    /**
     *  @brief  Create calo hit
     * 
     *  @param  parameters the calo hit parameters
     *  @param  pCaloHit to receive the address of the calo hit
     */
    template <typename PARAMETERS>
    StatusCode CreateCaloHit(const PARAMETERS &parameters, CaloHit *&pCaloHit);

    /**
     *  @brief  Perform the actual calo hit instantiation
     * 
     *  @param  parameters the calo hit parameters
     * 
     *  @return address of the new calo hit
     */
    template <typename PARAMETERS>
    CaloHit *HitInstantiation(const PARAMETERS &parameters);

    using InputObjectManager<CaloHit>::CreateTemporaryListAndSetCurrent;

    /**
     *  @brief  Change the current calo hit list to the constituent hits of a cluster list
     * 
     *  @param  pAlgorithm address of the algorithm changing the current calo hit list
     *  @param  clusterList the cluster list containing the hits
     *  @param  temporaryListName to receive the name of the temporary list
     */
    StatusCode CreateTemporaryListAndSetCurrent(const Algorithm *const pAlgorithm, const ClusterList &clusterList, std::string &temporaryListName);

    /**
     *  @brief  Erase all calo hit manager content
     */
    StatusCode EraseAllContent();

    /**
     *  @brief  Match calo hits to their correct mc particles for particle flow
     * 
     *  @param  caloHitToPfoTargetsMap the calo hit uid to mc pfo target map
     */
    StatusCode MatchCaloHitsToMCPfoTargets(const UidToMCParticleWeightMap &caloHitToPfoTargetsMap);

    /**
     *  @brief  Remove all mc particle associations that have been registered with calo hits
     */
    StatusCode RemoveAllMCParticleRelationships();

    /**
     *  @brief  Is calo hit available to add to a cluster
     * 
     *  @param  pCaloHit address of the calo hit
     * 
     *  @return boolean
     */
    bool IsCaloHitAvailable(CaloHit *const pCaloHit) const;

    /**
     *  @brief  Are all calo hits in list available to add to a cluster
     * 
     *  @param  caloHitList the list of calo hits
     * 
     *  @return boolean
     */
    bool AreCaloHitsAvailable(const CaloHitList &caloHitList) const;

    /**
     *  @brief  Set availability of a calo hit to be added to a cluster
     * 
     *  @param  pCaloHit the address of the calo hit
     *  @param  isAvailable the calo hit availability
     */
    StatusCode SetCaloHitAvailability(CaloHit *const pCaloHit, bool isAvailable);

    /**
     *  @brief  Set availability of all calo hits in list
     * 
     *  @param  caloHitList the list of calo hits
     *  @param  isAvailable the calo hit availability
     */
    StatusCode SetCaloHitAvailability(const CaloHitList &caloHitList, bool isAvailable);

    /**
     *  @brief  Fragment a calo hit into two daughter calo hits, with a specified energy division
     *
     *  @param  pOriginalCaloHit address of the original calo hit, which will be deleted
     *  @param  fraction1 the fraction of energy to be assigned to daughter fragment 1
     *  @param  pDaughterCaloHit1 to receive the address of daughter fragment 1
     *  @param  pDaughterCaloHit2 to receive the address of daughter fragment 2
     */
    StatusCode FragmentCaloHit(CaloHit *pOriginalCaloHit, const float fraction1, CaloHit *&pDaughterCaloHit1, CaloHit *&pDaughterCaloHit2);

    /**
     *  @brief  Merge two calo hit fragments, originally from the same parent hit, to form a new calo hit
     *
     *  @param  pFragmentCaloHit1 address of calo hit fragment 1, which will be deleted
     *  @param  pFragmentCaloHit2 address of calo hit fragment 2, which will be deleted
     *  @param  pMergedCaloHit to receive the address of the merged calo hit
     */
    StatusCode MergeCaloHitFragments(CaloHit *pFragmentCaloHit1, CaloHit *pFragmentCaloHit2, CaloHit *&pMergedCaloHit);

    /**
     *  @brief  Whether a calo hit can be fragmented into two daughter calo hits with the specified energy division
     * 
     *  @param  pOriginalCaloHit address of the original calo hit
     *  @param  fraction1 the fraction of energy to be assigned to daughter fragment 1
     * 
     *  @return boolean
     */
    bool CanFragmentCaloHit(CaloHit *pOriginalCaloHit, const float fraction1) const;

    /**
     *  @brief  Whether two candidate calo hit fragments can be merged
     * 
     *  @param  pFragmentCaloHit1 address of first candidate calo hit fragment
     *  @param  pFragmentCaloHit2 address of second candidate calo hit fragment
     * 
     *  @return boolean
     */
    bool CanMergeCaloHitFragments(CaloHit *pFragmentCaloHit1, CaloHit *pFragmentCaloHit2) const;

    /**
     *  @brief  Initialize reclustering operations, preparing lists and metadata accordingly
     * 
     *  @param  pAlgorithm address of the algorithm controlling reclustering
     *  @param  clusterList the input cluster list
     *  @param  originalReclusterListName the list name/key for the original recluster candidates
     */
    StatusCode InitializeReclustering(const Algorithm *const pAlgorithm, const ClusterList &clusterList, const std::string &originalReclusterListName);

    /**
     *  @brief  Prepare metadata to allow for construction of new recluster candidates
     * 
     *  @param  pAlgorithm address of the algorithm controlling reclustering
     *  @param  newReclusterListName the list name/key for the new recluster candidates
     */
    StatusCode PrepareForClustering(const Algorithm *const pAlgorithm, const std::string &newReclusterListName);

    /**
     *  @brief  End reclustering operations and update calo hit lists accordingly
     * 
     *  @param  pAlgorithm address of the algorithm controlling reclustering
     *  @param  selectedReclusterListName the list name/key for the chosen recluster candidates
     */
    StatusCode EndReclustering(const Algorithm *const pAlgorithm, const std::string &selectedReclusterListName);

    /**
     *  @brief  Update all calo hit lists to account for changes by daughter recluster processes
     * 
     *  @param  caloHitMetadata description of the changes made by daughter reclustering processes
     */
    StatusCode Update(const CaloHitMetadata &caloHitMetadata);

    /**
     *  @brief  Update all calo hit lists to account for a specific calo hit replacement
     * 
     *  @param  caloHitReplacement the calo hit replacement
     */
    StatusCode Update(const CaloHitReplacement &caloHitReplacement);

    /**
     *  @brief  Update a calo hit list to account for a specific calo hit replacement
     * 
     *  @param  pCaloHitList address of the calo hit list
     *  @param  caloHitReplacement the calo hit replacement
     */
    StatusCode Update(CaloHitList *pCaloHitList, const CaloHitReplacement &caloHitReplacement);

    unsigned int                    m_nReclusteringProcesses;           ///< The number of reclustering algorithms currently in operation
    ReclusterMetadata              *m_pCurrentReclusterMetadata;        ///< Address of the current recluster metadata
    ReclusterMetadataList           m_reclusterMetadataList;            ///< The recluster metadata list

    friend class PandoraApiImpl;
    friend class PandoraContentApiImpl;
    friend class PandoraImpl;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool CaloHitManager::IsCaloHitAvailable(CaloHit *const pCaloHit) const
{
    if (0 == m_nReclusteringProcesses)
        return pCaloHit->m_isAvailable;

    return m_pCurrentReclusterMetadata->GetCurrentCaloHitMetadata()->IsCaloHitAvailable(pCaloHit);
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode CaloHitManager::SetCaloHitAvailability(CaloHit *const pCaloHit, bool isAvailable)
{
    if (0 == m_nReclusteringProcesses)
    {
        pCaloHit->m_isAvailable = isAvailable;
        return STATUS_CODE_SUCCESS;
    }

    return m_pCurrentReclusterMetadata->GetCurrentCaloHitMetadata()->SetCaloHitAvailability(pCaloHit, isAvailable);
}

} // namespace pandora

#endif // #ifndef PANDORA_CALO_HIT_MANAGER_H
