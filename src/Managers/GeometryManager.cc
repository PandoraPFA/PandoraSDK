/**
 *  @file   PandoraSDK/src/Managers/GeometryManager.cc
 * 
 *  @brief  Implementation of the geometry manager class.
 * 
 *  $Log: $
 */

#include "Managers/GeometryManager.h"

#include "Objects/DetectorGap.h"
#include "Objects/SubDetector.h"

#include "Pandora/ObjectFactory.h"

#include <algorithm>

namespace pandora
{

GeometryManager::GeometryManager(const Pandora *const pPandora) :
    m_hitTypeToGranularityMap(this->GetDefaultHitTypeToGranularityMap()),
    m_pPandora(pPandora)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

GeometryManager::~GeometryManager()
{
    (void) this->EraseAllContent();
}

//------------------------------------------------------------------------------------------------------------------------------------------

const SubDetector &GeometryManager::GetSubDetector(const std::string &subDetectorName) const
{
    SubDetectorMap::const_iterator iter = m_subDetectorMap.find(subDetectorName);

    if (m_subDetectorMap.end() == iter)
        throw StatusCodeException(STATUS_CODE_NOT_FOUND);

    return *(iter->second);
}

//------------------------------------------------------------------------------------------------------------------------------------------

const SubDetector &GeometryManager::GetSubDetector(const SubDetectorType subDetectorType) const
{
    SubDetectorTypeMap::const_iterator iter = m_subDetectorTypeMap.find(subDetectorType);

    if (m_subDetectorTypeMap.end() == iter)
        throw StatusCodeException(STATUS_CODE_NOT_FOUND);

    if (m_subDetectorTypeMap.count(subDetectorType) != 1)
        throw StatusCodeException(STATUS_CODE_OUT_OF_RANGE);

    return *(iter->second);
}

//------------------------------------------------------------------------------------------------------------------------------------------

Granularity GeometryManager::GetHitTypeGranularity(const HitType hitType) const
{
    HitTypeToGranularityMap::const_iterator iter = m_hitTypeToGranularityMap.find(hitType);

    if (m_hitTypeToGranularityMap.end() != iter)
        return iter->second;

    std::cout << "GeometryManager: specified hitType must be registered with a specific granularity. See PandoraApi.h " << std::endl;
    throw StatusCodeException(STATUS_CODE_NOT_FOUND);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode GeometryManager::CreateSubDetector(const object_creation::Geometry::SubDetector::Parameters &inputParameters,
    const ObjectFactory<object_creation::Geometry::SubDetector::Parameters, object_creation::Geometry::SubDetector::Object> &factory)
{
    const SubDetector *pSubDetector = nullptr;

    try
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, factory.Create(inputParameters, pSubDetector));

        if (!m_subDetectorMap.insert(SubDetectorMap::value_type(pSubDetector->GetSubDetectorName(), pSubDetector)).second)
            throw StatusCodeException(STATUS_CODE_FAILURE);

        m_subDetectorTypeMap.insert(SubDetectorTypeMap::value_type(pSubDetector->GetSubDetectorType(), pSubDetector));
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "Failed to create sub detector: " << statusCodeException.ToString() << std::endl;
        delete pSubDetector;
        pSubDetector = nullptr;
        return statusCodeException.GetStatusCode();
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename PARAMETERS, typename OBJECT>
StatusCode GeometryManager::CreateGap(const PARAMETERS &parameters, const ObjectFactory<PARAMETERS, OBJECT> &factory)
{
    const OBJECT *pDetectorGap = nullptr;

    try
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, factory.Create(parameters, pDetectorGap));

        if (!pDetectorGap)
            return STATUS_CODE_FAILURE;

        m_detectorGapList.push_back(pDetectorGap);
        return STATUS_CODE_SUCCESS;
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "Failed to create gap: " << statusCodeException.ToString() << std::endl;
        delete pDetectorGap;
        pDetectorGap = nullptr;
        return statusCodeException.GetStatusCode();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode GeometryManager::EraseAllContent()
{
    for (const SubDetectorMap::value_type &mapEntry : m_subDetectorMap)
        delete mapEntry.second;

    for (const DetectorGap *const pDetectorGap : m_detectorGapList)
        delete pDetectorGap;

    m_subDetectorMap.clear();
    m_subDetectorTypeMap.clear();
    m_detectorGapList.clear();
    m_hitTypeToGranularityMap.clear();

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

GeometryManager::HitTypeToGranularityMap GeometryManager::GetDefaultHitTypeToGranularityMap() const
{
    HitTypeToGranularityMap hitTypeToGranularityMap;

    if (!hitTypeToGranularityMap.insert(HitTypeToGranularityMap::value_type(TRACKER, VERY_FINE)).second ||
        !hitTypeToGranularityMap.insert(HitTypeToGranularityMap::value_type(ECAL, FINE)).second ||
        !hitTypeToGranularityMap.insert(HitTypeToGranularityMap::value_type(HCAL, COARSE)).second ||
        !hitTypeToGranularityMap.insert(HitTypeToGranularityMap::value_type(MUON, VERY_COARSE)).second ||
        !hitTypeToGranularityMap.insert(HitTypeToGranularityMap::value_type(TPC_VIEW_U, VERY_FINE)).second ||
        !hitTypeToGranularityMap.insert(HitTypeToGranularityMap::value_type(TPC_VIEW_V, VERY_FINE)).second ||
        !hitTypeToGranularityMap.insert(HitTypeToGranularityMap::value_type(TPC_VIEW_W, VERY_FINE)).second ||
        !hitTypeToGranularityMap.insert(HitTypeToGranularityMap::value_type(TPC_3D, VERY_FINE)).second)
    {
        throw StatusCodeException(STATUS_CODE_FAILURE);
    }

    return hitTypeToGranularityMap;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode GeometryManager::SetHitTypeGranularity(const HitType hitType, const Granularity granularity)
{
    HitTypeToGranularityMap::iterator iter = m_hitTypeToGranularityMap.find(hitType);

    if (m_hitTypeToGranularityMap.end() != iter)
    {
        iter->second = granularity;
    }
    else
    {
        m_hitTypeToGranularityMap[hitType] = granularity;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

template StatusCode GeometryManager::CreateGap(const object_creation::Geometry::LineGap::Parameters &, const ObjectFactory<object_creation::Geometry::LineGap::Parameters, object_creation::Geometry::LineGap::Object> &);
template StatusCode GeometryManager::CreateGap(const object_creation::Geometry::BoxGap::Parameters &, const ObjectFactory<object_creation::Geometry::BoxGap::Parameters, object_creation::Geometry::BoxGap::Object> &);
template StatusCode GeometryManager::CreateGap(const object_creation::Geometry::ConcentricGap::Parameters &, const ObjectFactory<object_creation::Geometry::ConcentricGap::Parameters, object_creation::Geometry::ConcentricGap::Object> &);

} // namespace pandora
