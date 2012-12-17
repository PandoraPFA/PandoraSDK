/**
 *  @file   PandoraPFANew/Framework/include/Managers/Metadata.h
 * 
 *  @brief  Header file for metadata classes.
 * 
 *  $Log: $
 */
#ifndef METADATA_MANAGER_H
#define METADATA_MANAGER_H 1

#include "Objects/CaloHit.h"

#include "Pandora/PandoraInternal.h"
#include "Pandora/StatusCodes.h"

namespace pandora
{

/**
 *  @brief  CaloHitReplacement class
 */
class CaloHitReplacement
{
public:
    CaloHitList             m_oldCaloHits;              ///< The list of old calo hits, to be replaced
    CaloHitList             m_newCaloHits;              ///< The list new calo hits, to act as replacements
};

typedef std::vector<CaloHitReplacement *> CaloHitReplacementList;
typedef std::map<CaloHit *, bool> CaloHitUsageMap;

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  CaloHitMetadata class
 */
class CaloHitMetadata
{
public:
    /**
     *  @brief  Constructor
     * 
     *  @param  pCaloHitList address of the associated calo hit list
     *  @param  caloHitListName name of the associated calo hit list
     *  @param  initialHitAvailability the initial availability of the calo hits
     */
    CaloHitMetadata(CaloHitList *pCaloHitList, const std::string &caloHitListName, const bool initialHitAvailability);

    /**
     *  @brief  Destructor
     */
    ~CaloHitMetadata();

    /**
     *  @brief  Update metadata to account for changes by daughter recluster processes
     * 
     *  @param  caloHitMetadata description of the changes made by daughter reclustering processes
     */
    StatusCode Update(const CaloHitMetadata &caloHitMetadata);

    /**
     *  @brief  Update metadata to account for a specific calo hit replacement
     * 
     *  @param  caloHitReplacement the calo hit replacement
     */
    StatusCode Update(const CaloHitReplacement &caloHitReplacement);

    /**
     *  @brief  Clear all metadata content
     */
    void Clear();

    /**
     *  @brief  Get the calo hit usage map
     * 
     *  @return the calo hit usage map
     */
    const CaloHitUsageMap &GetCaloHitUsageMap() const;

    /**
     *  @brief  Get the calo hit replacement list
     * 
     *  @return the calo hit replacement list
     */
    const CaloHitReplacementList &GetCaloHitReplacementList() const;

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
    StatusCode SetCaloHitAvailability(CaloHitList &caloHitList, bool isAvailable);

private:
    CaloHitList                *m_pCaloHitList;                     ///< Address of the associated calo hit list
    std::string                 m_caloHitListName;                  ///< The name of the associated calo hit list
    CaloHitUsageMap             m_caloHitUsageMap;                  ///< The calo hit usage map
    CaloHitReplacementList      m_caloHitReplacementList;           ///< The calo hit replacement list
};

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  ReclusterMetadata class
 */
class ReclusterMetadata
{
public:
    /**
     *  @brief  Constructor
     * 
     *  @param  pCaloHitList address of the initial calo hit list, copies of which will be used during reclustering
     */
    ReclusterMetadata(CaloHitList *pCaloHitList);

    /**
     *  @brief  Destructor
     */
    ~ReclusterMetadata();

    /**
     *  @brief  Create new calo hit metadata, associated with a new reclustering option for the calo hits
     * 
     *  @param  pCaloHitList address of the calo hit list associated with the reclustering option
     *  @param  caloHitListName name of the calo hit list associated with the reclustering option
     *  @param  reclusterListName the name of the reclustering option
     *  @param  initialHitAvailability the initial availability of the calo hits
     */
    StatusCode CreateCaloHitMetadata(CaloHitList *pCaloHitList, const std::string &caloHitListName, const std::string &reclusterListName,
        const bool initialHitAvailability);

    /**
     *  @brief  Extract specific calo hit metadata, removing entry from map and receiving a pointer to the metadata
     * 
     *  @param  reclusterListName the key/name matching the desired metadata
     *  @param  pCaloHitMetaData to receive the pointer to the metadata
     */
    StatusCode ExtractCaloHitMetadata(const std::string &reclusterListName, CaloHitMetadata *&pCaloHitMetaData);

    /**
     *  @brief  Get the initial calo hit list
     * 
     *  @return the initial calo hit list
     */
    const CaloHitList &GetCaloHitList() const;

    /**
     *  @brief  Get the current calo hit metadata
     * 
     *  @return the current calo hit metadata
     */
    CaloHitMetadata *GetCurrentCaloHitMetadata();

private:
    typedef std::map<std::string, CaloHitMetadata *> NameToMetadataMap;

    CaloHitMetadata            *m_pCurrentCaloHitMetadata;          ///< Address of the current calo hit metadata
    CaloHitList                 m_caloHitList;                      ///< Copy of the reclustering input calo hit list
    NameToMetadataMap           m_nameToMetadataMap;                ///< The recluster list name to metadata map
};

typedef std::vector<ReclusterMetadata *> ReclusterMetadataList;

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline const CaloHitUsageMap &CaloHitMetadata::GetCaloHitUsageMap() const
{
    return m_caloHitUsageMap;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const CaloHitReplacementList &CaloHitMetadata::GetCaloHitReplacementList() const
{
    return m_caloHitReplacementList;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline bool CaloHitMetadata::IsCaloHitAvailable(CaloHit *const pCaloHit) const
{
    CaloHitUsageMap::const_iterator usageMapIter = m_caloHitUsageMap.find(pCaloHit);

    if ((m_caloHitUsageMap.end()) == usageMapIter || !usageMapIter->second)
        return false;

    return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline StatusCode CaloHitMetadata::SetCaloHitAvailability(CaloHit *const pCaloHit, bool isAvailable)
{
    CaloHitUsageMap::iterator usageMapIter = m_caloHitUsageMap.find(pCaloHit);

    if (m_caloHitUsageMap.end() == usageMapIter)
        return STATUS_CODE_NOT_FOUND;

    usageMapIter->second = isAvailable;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

inline const CaloHitList &ReclusterMetadata::GetCaloHitList() const
{
    return m_caloHitList;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline CaloHitMetadata *ReclusterMetadata::GetCurrentCaloHitMetadata()
{
    return m_pCurrentCaloHitMetadata;
}

} // namespace pandora

#endif // #ifndef METADATA_MANAGER_H
