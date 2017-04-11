/**
 *  @file   PandoraSDK/src/Managers/TrackManager.cc
 * 
 *  @brief  Implementation of the track manager class.
 * 
 *  $Log: $
 */

#include "Managers/TrackManager.h"

#include "Objects/Track.h"

#include "Pandora/ObjectFactory.h"
#include "Pandora/PandoraInternal.h"

#include <algorithm>

namespace pandora
{

TrackManager::TrackManager(const Pandora *const pPandora) :
    InputObjectManager<Track>(pPandora)
{
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CreateInitialLists());
}

//------------------------------------------------------------------------------------------------------------------------------------------

TrackManager::~TrackManager()
{
    (void) this->EraseAllContent();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::Create(const object_creation::Track::Parameters &parameters, const Track *&pTrack,
    const ObjectFactory<object_creation::Track::Parameters, object_creation::Track::Object> &factory)
{
    pTrack = nullptr;

    try
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, factory.Create(parameters, pTrack));

        NameToListMap::iterator inputIter = m_nameToListMap.find(m_inputListName);

        if (!pTrack || (m_nameToListMap.end() == inputIter))
            throw StatusCodeException(STATUS_CODE_FAILURE);

        if (!m_uidToTrackMap.insert(UidToTrackMap::value_type(pTrack->GetParentAddress(), pTrack)).second)
            throw StatusCodeException(STATUS_CODE_ALREADY_PRESENT);

        inputIter->second->push_back(pTrack);
        return STATUS_CODE_SUCCESS;
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "Failed to create track: " << statusCodeException.ToString() << std::endl;
        delete pTrack;
        pTrack = nullptr;
        return statusCodeException.GetStatusCode();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
bool TrackManager::IsAvailable(const Track *const pTrack) const
{
    return pTrack->IsAvailable();
}

template <>
bool TrackManager::IsAvailable(const TrackList *const pTrackList) const
{
    bool isAvailable(true);

    for (const Track *const pTrack : *pTrackList)
        isAvailable &= this->IsAvailable(pTrack);

    return isAvailable;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
void TrackManager::SetAvailability(const Track *const pTrack, bool isAvailable) const
{
    this->Modifiable(pTrack)->SetAvailability(isAvailable);
}

template <>
void TrackManager::SetAvailability(const TrackList *const pTrackList, bool isAvailable) const
{
    for (const Track *const pTrack : *pTrackList)
        this->SetAvailability(pTrack, isAvailable);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::EraseAllContent()
{
    m_uidToTrackMap.clear();
    m_parentDaughterRelationMap.clear();
    m_siblingRelationMap.clear();

    return InputObjectManager<Track>::EraseAllContent();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::MatchTracksToMCPfoTargets(const UidToMCParticleWeightMap &trackToPfoTargetsMap)
{
    if (trackToPfoTargetsMap.empty())
        return STATUS_CODE_SUCCESS;

    NameToListMap::const_iterator inputIter = m_nameToListMap.find(m_inputListName);

    if (m_nameToListMap.end() == inputIter)
        return STATUS_CODE_FAILURE;

    for (const Track *const pTrack : *inputIter->second)
    {
        UidToMCParticleWeightMap::const_iterator pfoTargetIter = trackToPfoTargetsMap.find(pTrack->GetParentAddress());

        if (trackToPfoTargetsMap.end() == pfoTargetIter)
            continue;

        this->Modifiable(pTrack)->SetMCParticleWeightMap(pfoTargetIter->second);
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::RemoveAllMCParticleRelationships()
{
    NameToListMap::const_iterator inputIter = m_nameToListMap.find(m_inputListName);

    if (m_nameToListMap.end() == inputIter)
        return STATUS_CODE_FAILURE;

    for (const Track *const pTrack : *inputIter->second)
        this->Modifiable(pTrack)->RemoveMCParticles();

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::SetTrackParentDaughterRelationship(const Uid parentUid, const Uid daughterUid)
{
    m_parentDaughterRelationMap.insert(TrackRelationMap::value_type(parentUid, daughterUid));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::SetTrackSiblingRelationship(const Uid firstSiblingUid, const Uid secondSiblingUid)
{
    m_siblingRelationMap.insert(TrackRelationMap::value_type(firstSiblingUid, secondSiblingUid));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::AssociateTracks() const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AddParentDaughterAssociations());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->AddSiblingAssociations());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::AddParentDaughterAssociations() const
{
    if (m_parentDaughterRelationMap.empty())
        return STATUS_CODE_SUCCESS;

    const TrackList *pInputList(nullptr);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetList(m_inputListName, pInputList));

    for (const Track *const pParentTrack : *pInputList)
    {
        const auto range(m_parentDaughterRelationMap.equal_range(pParentTrack->GetParentAddress()));

        TrackList daughterList;
        for (TrackRelationMap::const_iterator relIter = range.first; relIter != range.second; ++relIter)
        {
            UidToTrackMap::const_iterator daughterIter = m_uidToTrackMap.find(relIter->second);

            if ((m_uidToTrackMap.end() != daughterIter) && (daughterList.end() == std::find(daughterList.begin(), daughterList.end(), daughterIter->second)))
                daughterList.push_back(daughterIter->second);
        }
        daughterList.sort(PointerLessThan<Track>());

        for (const Track *const pDaughterTrack : daughterList)
        {
            const StatusCode firstStatusCode(this->Modifiable(pParentTrack)->AddDaughter(pDaughterTrack));
            const StatusCode secondStatusCode(this->Modifiable(pDaughterTrack)->AddParent(pParentTrack));

            if (firstStatusCode != secondStatusCode)
                return STATUS_CODE_FAILURE;

            if ((firstStatusCode != STATUS_CODE_SUCCESS) && (firstStatusCode != STATUS_CODE_ALREADY_PRESENT))
                return firstStatusCode;
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::AddSiblingAssociations() const
{
    if (m_siblingRelationMap.empty())
        return STATUS_CODE_SUCCESS;

    const TrackList *pInputList(nullptr);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetList(m_inputListName, pInputList));

    for (const Track *const pTrack : *pInputList)
    {
        const auto range(m_siblingRelationMap.equal_range(pTrack->GetParentAddress()));

        TrackList siblingList;
        for (TrackRelationMap::const_iterator relIter = range.first; relIter != range.second; ++relIter)
        {
            UidToTrackMap::const_iterator siblingIter = m_uidToTrackMap.find(relIter->second);

            if ((m_uidToTrackMap.end() != siblingIter) && (siblingList.end() == std::find(siblingList.begin(), siblingList.end(), siblingIter->second)))
                siblingList.push_back(siblingIter->second);
        }
        siblingList.sort(PointerLessThan<Track>());

        for (const Track *const pSiblingTrack : siblingList)
        {
            const StatusCode firstStatusCode(this->Modifiable(pTrack)->AddSibling(pSiblingTrack));
            const StatusCode secondStatusCode(this->Modifiable(pSiblingTrack)->AddSibling(pTrack));

            if (firstStatusCode != secondStatusCode)
                return STATUS_CODE_FAILURE;

            if ((firstStatusCode != STATUS_CODE_SUCCESS) && (firstStatusCode != STATUS_CODE_ALREADY_PRESENT))
                return firstStatusCode;
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::SetAssociatedCluster(const Track *const pTrack, const Cluster *const pCluster) const
{
    return this->Modifiable(pTrack)->SetAssociatedCluster(pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::RemoveAssociatedCluster(const Track *const pTrack, const Cluster *const pCluster) const
{
    return this->Modifiable(pTrack)->RemoveAssociatedCluster(pCluster);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::RemoveAllClusterAssociations() const
{
    NameToListMap::const_iterator inputIter = m_nameToListMap.find(m_inputListName);

    if (m_nameToListMap.end() == inputIter)
        return STATUS_CODE_FAILURE;

    return this->RemoveClusterAssociations(*(inputIter->second));
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::RemoveCurrentClusterAssociations(TrackToClusterMap &danglingClusters) const
{
    NameToListMap::const_iterator listIter = m_nameToListMap.find(m_currentListName);

    if (m_nameToListMap.end() == listIter)
        return STATUS_CODE_FAILURE;

    for (const Track *const pTrack : *listIter->second)
    {
        if (!pTrack->HasAssociatedCluster())
            continue;

        if (!danglingClusters.insert(TrackToClusterMap::value_type(pTrack, pTrack->GetAssociatedCluster())).second)
            return STATUS_CODE_FAILURE;

        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->RemoveAssociatedCluster(pTrack, pTrack->GetAssociatedCluster()));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::RemoveClusterAssociations(const TrackList &trackList) const
{
    for (const Track *const pTrack : trackList)
    {
        if (pTrack->HasAssociatedCluster())
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->RemoveAssociatedCluster(pTrack, pTrack->GetAssociatedCluster()));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode TrackManager::InitializeReclustering(const Algorithm *const pAlgorithm, const TrackList &trackList, const std::string &/*originalReclusterListName*/)
{
    std::string temporaryListName;
    return this->CreateTemporaryListAndSetCurrent(pAlgorithm, trackList, temporaryListName);
}

} // namespace pandora
