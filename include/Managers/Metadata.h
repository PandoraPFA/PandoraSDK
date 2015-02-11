/**
 *  @file   PandoraSDK/include/Managers/Metadata.h
 * 
 *  @brief  Header file for metadata classes.
 * 
 *  $Log: $
 */
#ifndef PANDORA_METADATA_MANAGER_H
#define PANDORA_METADATA_MANAGER_H 1

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
typedef std::map<const CaloHit *, bool> CaloHitUsageMap;

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
    CaloHitMetadata(CaloHitList *const pCaloHitList, const std::string &caloHitListName, const bool initialHitAvailability);

    /**
     *  @brief  Destructor
     */
    ~CaloHitMetadata();

    /**
     *  @brief  Is a calo hit, or a list of calo hits, available to add to a cluster
     * 
     *  @param  pT address of the object or object list
     * 
     *  @return boolean
     */
    template <typename T>
    bool IsAvailable(const T *const pT) const;

    /**
     *  @brief  Set availability of a calo hit, or a list of calo hits, to be added to a cluster
     * 
     *  @param  pT the address of the object or object list
     *  @param  isAvailable the availability
     */
    template <typename T>
    StatusCode SetAvailability(const T *const pT, bool isAvailable);

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
    ReclusterMetadata(CaloHitList *const pCaloHitList);

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
    StatusCode CreateCaloHitMetadata(CaloHitList *const pCaloHitList, const std::string &caloHitListName, const std::string &reclusterListName,
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

#endif // #ifndef PANDORA_METADATA_MANAGER_H
