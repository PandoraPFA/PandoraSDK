/**
 *  @file   PandoraSDK/include/Managers/GeometryManager.h
 * 
 *  @brief  Header file for the geometry manager class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_GEOMETRY_MANAGER_H
#define PANDORA_GEOMETRY_MANAGER_H 1

#include "Pandora/ObjectCreation.h"
#include "Pandora/PandoraEnumeratedTypes.h"

namespace pandora
{

/**
 *  @brief  GeometryManager class
 */
class GeometryManager
{
public:
    /**
     *  @brief  Constructor
     * 
     *  @param  pPandora address of the associated pandora object
     */
    GeometryManager(const Pandora *const pPandora);

    /**
     *  @brief  Destructor
     */
    ~GeometryManager();

    /**
     *  @brief  Get the sub detector corresponding to a specified name
     * 
     *  @param  subDetectorName the sub detector name
     * 
     *  @return the sub detector
     */
    const SubDetector &GetSubDetector(const std::string &subDetectorName) const;

    /**
     *  @brief  Get the sub detector corresponding to a specified type.
     *          Will throw exception if there is not exactly one subdetector registered with the specified type.
     * 
     *  @param  subDetectorType the sub detector type
     * 
     *  @return the sub detector
     */
    const SubDetector &GetSubDetector(const SubDetectorType subDetectorType) const;

    /**
     *  @brief  Get the map from name to sub detector parameters
     * 
     *  @return the map from name to sub detector parameters
     */
    const SubDetectorMap &GetSubDetectorMap() const;

    /**
     *  @brief  If there is exactly one registered lar tpc instance, return it; else raise an exception
     * 
     *  @return the lar tpc instance
     */
    const LArTPC &GetLArTPC() const;

    /**
     *  @brief  Get the map from name to lar tpc parameters
     * 
     *  @return the map from name to lar tpc paramters
     */
    const LArTPCMap &GetLArTPCMap() const;

    /**
     *  @brief  Get the list of gaps in the active detector volume
     * 
     *  @return the list of gaps in the active detector volume
     */
    const DetectorGapList &GetDetectorGapList() const;

    /**
     *  @brief  Get the granularity level specified for a given calorimeter hit type
     * 
     *  @param  hitType the calorimeter hit type
     * 
     *  @return the granularity
     */
    Granularity GetHitTypeGranularity(const HitType hitType) const;

private:
    /**
     *  @brief  Create sub detector
     * 
     *  @param  parameters the sub detector parameters
     *  @param  factory the factory that performs the object allocation
     */
    StatusCode CreateSubDetector(const object_creation::Geometry::SubDetector::Parameters &parameters,
        const ObjectFactory<object_creation::Geometry::SubDetector::Parameters, object_creation::Geometry::SubDetector::Object> &factory);

    /**
     *  @brief  Create lar tpc
     * 
     *  @param  parameters the lar tpc parameters
     *  @param  factory the factory that performs the object allocation
     */
    StatusCode CreateLArTPC(const object_creation::Geometry::LArTPC::Parameters &parameters,
        const ObjectFactory<object_creation::Geometry::LArTPC::Parameters, object_creation::Geometry::LArTPC::Object> &factory);

    /**
     *  @brief  Create gap
     * 
     *  @param  parameters the gap parameters
     *  @param  factory the factory that performs the object allocation
     */
    template <typename PARAMETERS, typename OBJECT>
    StatusCode CreateGap(const PARAMETERS &parameters, const ObjectFactory<PARAMETERS, OBJECT> &factory);

    /**
     *  @brief  Erase all geometry manager content
     */
    StatusCode EraseAllContent();

    typedef std::map<HitType, Granularity> HitTypeToGranularityMap;

    /**
     *  @brief  Get the default hit type to granularity map
     * 
     *  @return the default hit type to granularity map
     */
    HitTypeToGranularityMap GetDefaultHitTypeToGranularityMap() const;

    /**
     *  @brief  Set the granularity level to be associated with a specified hit type
     * 
     *  @param  hitType the specified hit type
     *  @param  granularity the specified granularity
     */
    StatusCode SetHitTypeGranularity(const HitType hitType, const Granularity granularity);

    typedef std::multimap<SubDetectorType, const SubDetector*> SubDetectorTypeMap;

    SubDetectorMap              m_subDetectorMap;           ///< Map from sub detector name to sub detector
    SubDetectorTypeMap          m_subDetectorTypeMap;       ///< Map from sub detector type to sub detector
    LArTPCMap                   m_larTPCMap;                ///< Map from lar tpc volume id to lar tpc
    DetectorGapList             m_detectorGapList;          ///< List of gaps in the active detector volume
    HitTypeToGranularityMap     m_hitTypeToGranularityMap;  ///< The hit type to granularity map

    const Pandora *const        m_pPandora;                 ///< The associated pandora object

    friend class PandoraApiImpl;
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline const SubDetectorMap &GeometryManager::GetSubDetectorMap() const
{
    return m_subDetectorMap;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const LArTPCMap &GeometryManager::GetLArTPCMap() const
{
    return m_larTPCMap;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const DetectorGapList &GeometryManager::GetDetectorGapList() const
{
    return m_detectorGapList;
}

} // namespace pandora

#endif // #ifndef PANDORA_GEOMETRY_MANAGER_H
