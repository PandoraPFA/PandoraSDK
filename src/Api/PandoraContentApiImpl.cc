/**
 *  @file   PandoraSDK/src/Api/PandoraContentApiImpl.cc
 * 
 *  @brief  Implementation of the pandora content api class.
 * 
 *  $Log: $
 */

#include "Api/PandoraContentApiImpl.h"

#include "Managers/AlgorithmManager.h"
#include "Managers/CaloHitManager.h"
#include "Managers/ClusterManager.h"
#include "Managers/GeometryManager.h"
#include "Managers/MCManager.h"
#include "Managers/ParticleFlowObjectManager.h"
#include "Managers/PluginManager.h"
#include "Managers/TrackManager.h"
#include "Managers/VertexManager.h"

#include "Objects/CaloHit.h"
#include "Objects/Cluster.h"
#include "Objects/ParticleFlowObject.h"
#include "Objects/Track.h"
#include "Objects/Vertex.h"

#include "Pandora/Algorithm.h"
#include "Pandora/AlgorithmTool.h"
#include "Pandora/ObjectFactory.h"
#include "Pandora/Pandora.h"
#include "Pandora/PandoraSettings.h"

#include "Persistency/PandoraIO.h"

namespace pandora
{

// Macros for type mappings to avoid repeated template specializations
#define MANAGER_TYPE_LIST(d)                                                \
d(CaloHit,              CaloHitManager,             m_pCaloHitManager)      \
d(Track,                TrackManager,               m_pTrackManager)        \
d(MCParticle,           MCManager,                  m_pMCManager)           \
d(Cluster,              ClusterManager,             m_pClusterManager)      \
d(ParticleFlowObject,   ParticleFlowObjectManager,  m_pPfoManager)          \
d(Vertex,               VertexManager,              m_pVertexManager)       \
d(CaloHitList,          CaloHitManager,             m_pCaloHitManager)      \
d(TrackList,            TrackManager,               m_pTrackManager)        \
d(MCParticleList,       MCManager,                  m_pMCManager)           \
d(ClusterList,          ClusterManager,             m_pClusterManager)      \
d(PfoList,              ParticleFlowObjectManager,  m_pPfoManager)          \
d(VertexList,           VertexManager,              m_pVertexManager)

#define MANAGER_TYPE_MAPPING(a, b, c)                                       \
template<>                                                                  \
struct PandoraContentApiImpl::ReturnType<a>                                 \
{                                                                           \
    typedef b Type;                                                         \
};                                                                          \
                                                                            \
template <>                                                                 \
inline b *PandoraContentApiImpl::GetManager<a>() const                      \
{                                                                           \
    return m_pPandora->c;                                                   \
}

MANAGER_TYPE_LIST(MANAGER_TYPE_MAPPING)

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
StatusCode PandoraContentApiImpl::AlterMetadata(const CaloHit *const pObject, const object_creation::CaloHit::Metadata &metadata) const
{
    return this->GetManager<CaloHit>()->AlterMetadata(pObject, metadata);
}

template <>
StatusCode PandoraContentApiImpl::AlterMetadata(const Cluster *const pObject, const object_creation::Cluster::Metadata &metadata) const
{
    return this->GetManager<Cluster>()->AlterMetadata(pObject, metadata);
}

template <>
StatusCode PandoraContentApiImpl::AlterMetadata(const ParticleFlowObject *const pObject, const object_creation::ParticleFlowObject::Metadata &metadata) const
{
    return this->GetManager<ParticleFlowObject>()->AlterMetadata(pObject, metadata);
}

template <>
StatusCode PandoraContentApiImpl::AlterMetadata(const Vertex *const pObject, const object_creation::Vertex::Metadata &metadata) const
{
    return this->GetManager<Vertex>()->AlterMetadata(pObject, metadata);
}

template <typename OBJECT, typename METADATA>
StatusCode PandoraContentApiImpl::AlterMetadata(const OBJECT *const /*pObject*/, const METADATA &/*metadata*/) const
{
    return STATUS_CODE_NOT_ALLOWED;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
StatusCode PandoraContentApiImpl::Create(const object_creation::CaloHit::Parameters &parameters, const CaloHit *&pObject,
    const pandora::ObjectFactory<object_creation::CaloHit::Parameters, object_creation::CaloHit::Object> &factory) const
{
    return this->GetManager<CaloHit>()->Create(parameters, pObject, factory);
}

template <>
StatusCode PandoraContentApiImpl::Create(const object_creation::Track::Parameters &parameters, const Track *&pObject,
    const pandora::ObjectFactory<object_creation::Track::Parameters, object_creation::Track::Object> &factory) const
{
    return this->GetManager<Track>()->Create(parameters, pObject, factory);
}

template <>
StatusCode PandoraContentApiImpl::Create(const object_creation::MCParticle::Parameters &parameters, const MCParticle *&pObject,
    const pandora::ObjectFactory<object_creation::MCParticle::Parameters, object_creation::MCParticle::Object> &factory) const
{
    return this->GetManager<MCParticle>()->Create(parameters, pObject, factory);
}

template <>
StatusCode PandoraContentApiImpl::Create(const object_creation::Vertex::Parameters &parameters, const Vertex *&pObject,
    const pandora::ObjectFactory<object_creation::Vertex::Parameters, object_creation::Vertex::Object> &factory) const
{
    return this->GetManager<Vertex>()->Create(parameters, pObject, factory);
}

template <>
StatusCode PandoraContentApiImpl::Create(const object_creation::Cluster::Parameters &parameters, const Cluster *&pCluster,
    const pandora::ObjectFactory<object_creation::Cluster::Parameters, object_creation::Cluster::Object> &factory) const
{
    if (!this->GetManager<CaloHit>()->IsAvailable(&parameters.m_caloHitList) ||
        !this->GetManager<CaloHit>()->IsAvailable(&parameters.m_isolatedCaloHitList))
    {
        return STATUS_CODE_NOT_ALLOWED;
    }

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Cluster>()->Create(parameters, pCluster, factory));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<CaloHit>()->SetAvailability(&parameters.m_caloHitList, false));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<CaloHit>()->SetAvailability(&parameters.m_isolatedCaloHitList, false));

    return STATUS_CODE_SUCCESS;
}

template <>
StatusCode PandoraContentApiImpl::Create(const object_creation::ParticleFlowObject::Parameters &pfoParameters, const ParticleFlowObject *&pPfo,
    const pandora::ObjectFactory<object_creation::ParticleFlowObject::Parameters, object_creation::ParticleFlowObject::Object> &factory) const
{
    if (!this->GetManager<Cluster>()->IsAvailable(&pfoParameters.m_clusterList) ||
        !this->GetManager<Track>()->IsAvailable(&pfoParameters.m_trackList) ||
        !this->GetManager<Vertex>()->IsAvailable(&pfoParameters.m_vertexList))
    {
        return STATUS_CODE_NOT_ALLOWED;
    }

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<ParticleFlowObject>()->Create(pfoParameters, pPfo, factory));
    this->GetManager<Cluster>()->SetAvailability(&pfoParameters.m_clusterList, false);
    this->GetManager<Track>()->SetAvailability(&pfoParameters.m_trackList, false);
    this->GetManager<Vertex>()->SetAvailability(&pfoParameters.m_vertexList, false);

    return STATUS_CODE_SUCCESS;
}

template <typename PARAMETERS, typename OBJECT>
StatusCode PandoraContentApiImpl::Create(const PARAMETERS &/*parameters*/, const OBJECT *&/*pObject*/, const pandora::ObjectFactory<PARAMETERS, OBJECT> &/*factory*/) const
{
    return STATUS_CODE_NOT_ALLOWED;
}

//------------------------------------------------------------------------------------------------------------------------------------------

const PandoraSettings *PandoraContentApiImpl::GetSettings() const
{
    return m_pPandora->GetSettings();
}

//------------------------------------------------------------------------------------------------------------------------------------------

const GeometryManager *PandoraContentApiImpl::GetGeometry() const
{
    return m_pPandora->GetGeometry();
}

//------------------------------------------------------------------------------------------------------------------------------------------

const PluginManager *PandoraContentApiImpl::GetPlugins() const
{
    return m_pPandora->GetPlugins();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::RepeatEventPreparation() const
{
    return m_pPandora->PrepareEvent();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::CreateAlgorithmTool(TiXmlElement *const pXmlElement, AlgorithmTool *&pAlgorithmTool) const
{
    return m_pPandora->m_pAlgorithmManager->CreateAlgorithmTool(pXmlElement, pAlgorithmTool);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::CreateDaughterAlgorithm(TiXmlElement *const pXmlElement, std::string &daughterAlgorithmName) const
{
    return m_pPandora->m_pAlgorithmManager->CreateAlgorithm(pXmlElement, daughterAlgorithmName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::RunAlgorithm(const std::string &algorithmName) const
{
    AlgorithmManager::AlgorithmMap::const_iterator iter = m_pPandora->m_pAlgorithmManager->m_algorithmMap.find(algorithmName);

    if (m_pPandora->m_pAlgorithmManager->m_algorithmMap.end() == iter)
        return STATUS_CODE_NOT_FOUND;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PreRunAlgorithm(iter->second));

    try
    {
        const bool shouldDisplayAlgorithmInfo(m_pPandora->GetSettings()->ShouldDisplayAlgorithmInfo());

        if (shouldDisplayAlgorithmInfo)
        {
            for (unsigned int i = 1, iMax = this->GetManager<CaloHit>()->m_algorithmInfoMap.size(); i < iMax; ++i) std::cout << "----";
            std::cout << "> Running Algorithm: " << iter->second->GetInstanceName() << ", " << iter->second->GetType() << std::endl;
        }

        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, iter->second->Run());
    }
    catch (const StatusCodeException &exception)
    {
        std::cout << "Failure in algorithm " << iter->first << ", " << iter->second->GetType() << ", " << exception.ToString()
                  << exception.GetBackTrace() << std::endl;
    }
    catch (const StopProcessingException &exception)
    {
        std::cout << "Algorithm " << iter->first << ", " << iter->second->GetType() << " raised stop processing exception: "
                  << exception.GetDescription() << std::endl;
        throw exception;
    }
    catch (...)
    {
        std::cout << "Failure in algorithm " << iter->first << ", " << iter->second->GetType() << ", unknown exception" << std::endl;
    }

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PostRunAlgorithm(iter->second));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::RunClusteringAlgorithm(const Algorithm &algorithm, const std::string &clusteringAlgorithmName,
    const ClusterList *&pNewClusterList, std::string &newClusterListName) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Cluster>()->CreateTemporaryListAndSetCurrent(&algorithm, newClusterListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<CaloHit>()->PrepareForClustering(&algorithm, newClusterListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->RunAlgorithm(clusteringAlgorithmName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Cluster>()->GetCurrentList(pNewClusterList, newClusterListName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
StatusCode PandoraContentApiImpl::GetCurrentList(const T *&pT, std::string &listName) const
{
    return this->GetManager<T>()->GetCurrentList(pT, listName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
StatusCode PandoraContentApiImpl::GetCurrentListName(std::string &listName) const
{
    return this->GetManager<T>()->GetCurrentListName(listName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
StatusCode PandoraContentApiImpl::ReplaceCurrentList(const Algorithm &algorithm, const std::string &newListName) const
{
    return this->GetManager<T>()->ReplaceCurrentAndAlgorithmInputLists(&algorithm, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
StatusCode PandoraContentApiImpl::DropCurrentList(const Algorithm &algorithm) const
{
    return this->GetManager<T>()->DropCurrentList(&algorithm);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
StatusCode PandoraContentApiImpl::GetList(const std::string &listName, const T *&pT) const
{
    return this->GetManager<T>()->GetList(listName, pT);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
StatusCode PandoraContentApiImpl::RenameList(const std::string &oldListName, const std::string &newListName) const
{
    return this->GetManager<T>()->RenameList(oldListName, newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
StatusCode PandoraContentApiImpl::SaveList(const T &t, const std::string &newListName) const
{
    return this->GetManager<T>()->SaveList(newListName, t);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
StatusCode PandoraContentApiImpl::SaveList(const std::string &newListName) const
{
    std::string currentListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<T>()->GetCurrentListName(currentListName));
    return this->GetManager<T>()->SaveObjects(newListName, currentListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
StatusCode PandoraContentApiImpl::SaveList(const std::string &oldListName, const std::string &newListName) const
{
    return this->GetManager<T>()->SaveObjects(newListName, oldListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
StatusCode PandoraContentApiImpl::SaveList(const std::string &newListName, const T &t) const
{
    std::string currentListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<T>()->GetCurrentListName(currentListName));
    return this->GetManager<T>()->SaveObjects(newListName, currentListName, t);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
StatusCode PandoraContentApiImpl::SaveList(const std::string &oldListName, const std::string &newListName, const T &t) const
{
    return this->GetManager<T>()->SaveObjects(newListName, oldListName, t);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
StatusCode PandoraContentApiImpl::TemporarilyReplaceCurrentList(const std::string &newListName) const
{
    return this->GetManager<T>()->TemporarilyReplaceCurrentList(newListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
StatusCode PandoraContentApiImpl::CreateTemporaryListAndSetCurrent(const Algorithm &algorithm, const T *&pT, std::string &temporaryListName) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<T>()->CreateTemporaryListAndSetCurrent(&algorithm, temporaryListName));
    return this->GetManager<T>()->GetCurrentList(pT, temporaryListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
bool PandoraContentApiImpl::IsAvailable(const T *const pT) const
{
    return this->GetManager<T>()->IsAvailable(pT);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
StatusCode PandoraContentApiImpl::AddToCluster(const Cluster *const pCluster, const CaloHitList *const pCaloHitList) const
{
    for (const CaloHit *const pCaloHit : *pCaloHitList)
    {
        if (!this->IsAddToClusterAllowed(pCluster, pCaloHit))
            return STATUS_CODE_NOT_ALLOWED;
    }

    for (const CaloHit *const pCaloHit : *pCaloHitList)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Cluster>()->AddToCluster(pCluster, pCaloHit));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<CaloHit>()->SetAvailability(pCaloHit, false));
    }

    return STATUS_CODE_SUCCESS;
}

template <>
StatusCode PandoraContentApiImpl::AddToCluster(const Cluster *const pCluster, const CaloHit *const pCaloHit) const
{
    const CaloHitList caloHitList(1, pCaloHit);
    return this->AddToCluster(pCluster, &caloHitList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::RemoveFromCluster(const Cluster *const pCluster, const CaloHit *const pCaloHit) const
{
    if ((pCluster->GetNCaloHits() <= 1) && (pCluster->GetNIsolatedCaloHits() == 0))
        return STATUS_CODE_NOT_ALLOWED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Cluster>()->RemoveFromCluster(pCluster, pCaloHit));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<CaloHit>()->SetAvailability(pCaloHit, true));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
StatusCode PandoraContentApiImpl::AddIsolatedToCluster(const Cluster *const pCluster, const CaloHitList *const pCaloHitList) const
{
    for (const CaloHit *const pCaloHit : *pCaloHitList)
    {
        if (!this->IsAddToClusterAllowed(pCluster, pCaloHit))
            return STATUS_CODE_NOT_ALLOWED;
    }

    for (const CaloHit *const pCaloHit : *pCaloHitList)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Cluster>()->AddIsolatedToCluster(pCluster, pCaloHit));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<CaloHit>()->SetAvailability(pCaloHit, false));
    }

    return STATUS_CODE_SUCCESS;
}

template <>
StatusCode PandoraContentApiImpl::AddIsolatedToCluster(const Cluster *const pCluster, const CaloHit *const pCaloHit) const
{
    const CaloHitList caloHitList(1, pCaloHit);
    return this->AddIsolatedToCluster(pCluster, &caloHitList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::RemoveIsolatedFromCluster(const Cluster *const pCluster, const CaloHit *const pCaloHit) const
{
    if ((pCluster->GetNCaloHits() == 0) && (pCluster->GetNIsolatedCaloHits() <= 1))
        return STATUS_CODE_NOT_ALLOWED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Cluster>()->RemoveIsolatedFromCluster(pCluster, pCaloHit));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<CaloHit>()->SetAvailability(pCaloHit, true));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::Fragment(const CaloHit *const pOriginalCaloHit, const float fraction1, const CaloHit *&pDaughterCaloHit1,
    const CaloHit *&pDaughterCaloHit2, const ObjectFactory<object_creation::CaloHitFragment::Parameters, object_creation::CaloHitFragment::Object> &factory) const
{
    return this->GetManager<CaloHit>()->FragmentCaloHit(pOriginalCaloHit, fraction1, pDaughterCaloHit1, pDaughterCaloHit2, factory);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::MergeFragments(const CaloHit *const pFragmentCaloHit1, const CaloHit *const pFragmentCaloHit2,
    const CaloHit *&pMergedCaloHit, const ObjectFactory<object_creation::CaloHitFragment::Parameters, object_creation::CaloHitFragment::Object> &factory) const
{
    return this->GetManager<CaloHit>()->MergeCaloHitFragments(pFragmentCaloHit1, pFragmentCaloHit2, pMergedCaloHit, factory);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::AddTrackClusterAssociation(const Track *const pTrack, const Cluster *const pCluster) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Track>()->SetAssociatedCluster(pTrack, pCluster));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Cluster>()->AddTrackAssociation(pCluster, pTrack));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::RemoveTrackClusterAssociation(const Track *const pTrack, const Cluster *const pCluster) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Track>()->RemoveAssociatedCluster(pTrack, pCluster));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Cluster>()->RemoveTrackAssociation(pCluster, pTrack));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::RemoveCurrentTrackClusterAssociations() const
{
    TrackList danglingTracks;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Cluster>()->RemoveCurrentTrackAssociations(danglingTracks));

    if (!danglingTracks.empty())
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Track>()->RemoveClusterAssociations(danglingTracks));

    TrackToClusterMap danglingClusters;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Track>()->RemoveCurrentClusterAssociations(danglingClusters));

    if (!danglingClusters.empty())
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Cluster>()->RemoveTrackAssociations(danglingClusters));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::RemoveAllTrackClusterAssociations() const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Track>()->RemoveAllClusterAssociations());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Cluster>()->RemoveAllTrackAssociations());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::RemoveAllMCParticleRelationships() const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<MCParticle>()->RemoveAllMCParticleRelationships());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<CaloHit>()->RemoveAllMCParticleRelationships());
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Track>()->RemoveAllMCParticleRelationships());

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::MergeAndDeleteClusters(const Cluster *const pClusterToEnlarge, const Cluster *const pClusterToDelete) const
{
    std::string currentListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Cluster>()->GetCurrentListName(currentListName));
    return this->MergeAndDeleteClusters(pClusterToEnlarge, pClusterToDelete, currentListName, currentListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::MergeAndDeleteClusters(const Cluster *const pClusterToEnlarge, const Cluster *const pClusterToDelete,
    const std::string &enlargeListName, const std::string &deleteListName) const
{
    if ((pClusterToEnlarge == pClusterToDelete) || !this->GetManager<Cluster>()->IsAvailable(pClusterToDelete))
        return STATUS_CODE_NOT_ALLOWED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Track>()->RemoveClusterAssociations(pClusterToDelete->GetAssociatedTrackList()));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Cluster>()->MergeAndDeleteClusters(pClusterToEnlarge, pClusterToDelete,
        enlargeListName, deleteListName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
StatusCode PandoraContentApiImpl::AddToPfo(const ParticleFlowObject *const pPfo, const T *const pT) const
{
    if (!this->GetManager<T>()->IsAvailable(pT))
        return STATUS_CODE_NOT_ALLOWED;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<ParticleFlowObject>()->AddToPfo(pPfo, pT));
    this->GetManager<T>()->SetAvailability(pT, false);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
StatusCode PandoraContentApiImpl::RemoveFromPfo(const ParticleFlowObject *const pPfo, const T *const pT) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<ParticleFlowObject>()->RemoveFromPfo(pPfo, pT));
    this->GetManager<T>()->SetAvailability(pT, true);

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::SetPfoParentDaughterRelationship(const ParticleFlowObject *const pParentPfo, const ParticleFlowObject *const pDaughterPfo) const
{
    return this->GetManager<ParticleFlowObject>()->SetParentDaughterAssociation(pParentPfo, pDaughterPfo);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::RemovePfoParentDaughterRelationship(const ParticleFlowObject *const pParentPfo, const ParticleFlowObject *const pDaughterPfo) const
{
    return this->GetManager<ParticleFlowObject>()->RemoveParentDaughterAssociation(pParentPfo, pDaughterPfo);
}

//------------------------------------------------------------------------------------------------------------------------------------------

PandoraContentApiImpl::PandoraContentApiImpl(Pandora *const pPandora) :
    m_pPandora(pPandora)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool PandoraContentApiImpl::IsAddToClusterAllowed(const Cluster *const pCluster, const CaloHit *const pCaloHit) const
{
    if (!this->GetManager<CaloHit>()->IsAvailable(pCaloHit))
        return false;

    if (!m_pPandora->GetSettings()->SingleHitTypeClusteringMode())
        return true;

    const CaloHit *pFirstCaloHit(nullptr);

    if (!pCluster->GetOrderedCaloHitList().empty())
    {
        if (pCluster->GetOrderedCaloHitList().begin()->second->empty())
            throw StatusCodeException(STATUS_CODE_FAILURE);

        pFirstCaloHit = *(pCluster->GetOrderedCaloHitList().begin()->second->begin());
    }
    else if (!pCluster->GetIsolatedCaloHitList().empty())
    {
        pFirstCaloHit = *(pCluster->GetIsolatedCaloHitList().begin());
    }

    if (!pFirstCaloHit || (pFirstCaloHit->GetHitType() == pCaloHit->GetHitType()))
        return true;

    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
StatusCode PandoraContentApiImpl::PrepareForDeletion(const T *const pT) const
{
    const MANAGED_CONTAINER<const T *> objectList(1, pT);
    return this->PrepareForDeletion(&objectList);
}

template <>
StatusCode PandoraContentApiImpl::PrepareForDeletion(const ClusterList *const pClusterList) const
{
    if (!this->GetManager<Cluster>()->IsAvailable(pClusterList))
        return STATUS_CODE_NOT_ALLOWED;

    CaloHitList caloHitList;
    TrackList trackList;

    for (const Cluster *const pCluster : *pClusterList)
    {
        pCluster->GetOrderedCaloHitList().FillCaloHitList(caloHitList);
        caloHitList.insert(caloHitList.end(), pCluster->GetIsolatedCaloHitList().begin(), pCluster->GetIsolatedCaloHitList().end());
        trackList.insert(trackList.end(), pCluster->GetAssociatedTrackList().begin(), pCluster->GetAssociatedTrackList().end());
    }

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<CaloHit>()->SetAvailability(&caloHitList, true));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=,  this->GetManager<Track>()->RemoveClusterAssociations(trackList));

    return STATUS_CODE_SUCCESS;
}

template <>
StatusCode PandoraContentApiImpl::PrepareForDeletion(const PfoList *const pPfoList) const
{
    for (const ParticleFlowObject *const pPfo : *pPfoList)
    {
        this->GetManager<Cluster>()->SetAvailability(&pPfo->GetClusterList(), true);
        this->GetManager<Track>()->SetAvailability(&pPfo->GetTrackList(), true);
        this->GetManager<Vertex>()->SetAvailability(&pPfo->GetVertexList(), true);

        const PfoList parentList(pPfo->GetParentPfoList());
        const PfoList daughterList(pPfo->GetDaughterPfoList());

        for (const ParticleFlowObject *const pParentPfo : parentList)
            this->RemovePfoParentDaughterRelationship(pParentPfo, pPfo);

        for (const ParticleFlowObject *const pDaughterPfo : daughterList)
            this->RemovePfoParentDaughterRelationship(pPfo, pDaughterPfo);
    }

    return STATUS_CODE_SUCCESS;
}

template <>
StatusCode PandoraContentApiImpl::PrepareForDeletion(const VertexList *const pVertexList) const
{
    if (!this->GetManager<Vertex>()->IsAvailable(pVertexList))
        return STATUS_CODE_NOT_ALLOWED;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <>
StatusCode PandoraContentApiImpl::PrepareForReclusteringDeletion(const ClusterList *const pClusterList) const
{
    if (!this->GetManager<Cluster>()->IsAvailable(pClusterList))
        return STATUS_CODE_NOT_ALLOWED;

    TrackList trackList;

    for (const Cluster *const pCluster : *pClusterList)
        trackList.insert(trackList.end(), pCluster->GetAssociatedTrackList().begin(), pCluster->GetAssociatedTrackList().end());

    return this->GetManager<Track>()->RemoveClusterAssociations(trackList);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
StatusCode PandoraContentApiImpl::Delete(const T *const pT) const
{
    std::string currentListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<T>()->GetCurrentListName(currentListName));
    return this->Delete(pT, currentListName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
StatusCode PandoraContentApiImpl::Delete(const T *const pT, const std::string &listName) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareForDeletion(pT));
    return this->GetManager<T>()->DeleteObject(pT, listName);
}

template <>
StatusCode PandoraContentApiImpl::Delete(const ClusterList *const pT, const std::string &listName) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareForDeletion(pT));
    return this->GetManager<ClusterList>()->DeleteObjects(*pT, listName);
}

template <>
StatusCode PandoraContentApiImpl::Delete(const PfoList *const pT, const std::string &listName) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareForDeletion(pT));
    return this->GetManager<PfoList>()->DeleteObjects(*pT, listName);
}

template <>
StatusCode PandoraContentApiImpl::Delete(const VertexList *const pT, const std::string &listName) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareForDeletion(pT));
    return this->GetManager<VertexList>()->DeleteObjects(*pT, listName);
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::InitializeFragmentation(const Algorithm &algorithm, const ClusterList &inputClusterList,
    std::string &originalClustersListName, std::string &fragmentClustersListName) const
{
    std::string inputClusterListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Cluster>()->GetAlgorithmInputListName(&algorithm, inputClusterListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Cluster>()->MoveObjectsToTemporaryListAndSetCurrent(&algorithm, inputClusterListName, originalClustersListName, inputClusterList));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<CaloHit>()->InitializeReclustering(&algorithm, inputClusterList, originalClustersListName));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Cluster>()->CreateTemporaryListAndSetCurrent(&algorithm, fragmentClustersListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<CaloHit>()->PrepareForClustering(&algorithm, fragmentClustersListName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::EndFragmentation(const Algorithm &algorithm, const std::string &clusterListToSaveName,
    const std::string &clusterListToDeleteName) const
{
    std::string inputClusterListName;
    const ClusterList *pClustersToBeDeleted(nullptr);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Cluster>()->GetAlgorithmInputListName(&algorithm, inputClusterListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Cluster>()->SaveObjects(inputClusterListName, clusterListToSaveName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Cluster>()->GetList(clusterListToDeleteName, pClustersToBeDeleted));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareForReclusteringDeletion(pClustersToBeDeleted));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Cluster>()->DeleteTemporaryObjects(&algorithm, clusterListToDeleteName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Cluster>()->ResetCurrentListToAlgorithmInputList(&algorithm));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<CaloHit>()->EndReclustering(&algorithm, clusterListToSaveName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<CaloHit>()->ResetCurrentListToAlgorithmInputList(&algorithm));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::InitializeReclustering(const Algorithm &algorithm, const TrackList &inputTrackList,
    const ClusterList &inputClusterList, std::string &originalClustersListName) const
{
    std::string inputClusterListName;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Cluster>()->GetAlgorithmInputListName(&algorithm, inputClusterListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Cluster>()->MoveObjectsToTemporaryListAndSetCurrent(&algorithm, inputClusterListName, originalClustersListName, inputClusterList));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Track>()->InitializeReclustering(&algorithm, inputTrackList, originalClustersListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<CaloHit>()->InitializeReclustering(&algorithm, inputClusterList, originalClustersListName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::EndReclustering(const Algorithm &algorithm, const std::string &selectedClusterListName) const
{
    std::string inputClusterListName;
    ClusterList clustersToBeDeleted;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Cluster>()->GetAlgorithmInputListName(&algorithm, inputClusterListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Cluster>()->SaveObjects(inputClusterListName, selectedClusterListName));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Cluster>()->GetResetDeletionObjects(&algorithm, clustersToBeDeleted));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareForReclusteringDeletion(&clustersToBeDeleted));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<CaloHit>()->ResetAlgorithmInfo(&algorithm, false));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Cluster>()->ResetAlgorithmInfo(&algorithm, false));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<ParticleFlowObject>()->ResetAlgorithmInfo(&algorithm, false));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Track>()->ResetAlgorithmInfo(&algorithm, false));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<CaloHit>()->EndReclustering(&algorithm, selectedClusterListName));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::PreRunAlgorithm(Algorithm *const pAlgorithm) const
{
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<CaloHit>()->RegisterAlgorithm(pAlgorithm));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Cluster>()->RegisterAlgorithm(pAlgorithm));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<MCParticle>()->RegisterAlgorithm(pAlgorithm));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<ParticleFlowObject>()->RegisterAlgorithm(pAlgorithm));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Track>()->RegisterAlgorithm(pAlgorithm));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Vertex>()->RegisterAlgorithm(pAlgorithm));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode PandoraContentApiImpl::PostRunAlgorithm(Algorithm *const pAlgorithm) const
{
    PfoList pfosToBeDeleted;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<ParticleFlowObject>()->GetResetDeletionObjects(pAlgorithm, pfosToBeDeleted));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareForDeletion(&pfosToBeDeleted));

    ClusterList clustersToBeDeleted;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Cluster>()->GetResetDeletionObjects(pAlgorithm, clustersToBeDeleted));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareForDeletion(&clustersToBeDeleted));

    VertexList verticesToBeDeleted;
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Vertex>()->GetResetDeletionObjects(pAlgorithm, verticesToBeDeleted));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->PrepareForDeletion(&verticesToBeDeleted));

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<CaloHit>()->ResetAlgorithmInfo(pAlgorithm, true));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Cluster>()->ResetAlgorithmInfo(pAlgorithm, true));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<MCParticle>()->ResetAlgorithmInfo(pAlgorithm, true));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<ParticleFlowObject>()->ResetAlgorithmInfo(pAlgorithm, true));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Track>()->ResetAlgorithmInfo(pAlgorithm, true));
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetManager<Vertex>()->ResetAlgorithmInfo(pAlgorithm, true));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------

template StatusCode PandoraContentApiImpl::AlterMetadata(const MCParticle *const, const object_creation::MCParticle::Metadata &) const;
template StatusCode PandoraContentApiImpl::AlterMetadata(const Track *const, const object_creation::Track::Metadata &) const;
template StatusCode PandoraContentApiImpl::AlterMetadata(const SubDetector *const, const object_creation::Geometry::SubDetector::Metadata &) const;
template StatusCode PandoraContentApiImpl::AlterMetadata(const LArTPC *const, const object_creation::Geometry::LArTPC::Metadata &) const;
template StatusCode PandoraContentApiImpl::AlterMetadata(const LineGap *const, const object_creation::Geometry::LineGap::Metadata &) const;
template StatusCode PandoraContentApiImpl::AlterMetadata(const BoxGap *const, const object_creation::Geometry::BoxGap::Metadata &) const;
template StatusCode PandoraContentApiImpl::AlterMetadata(const ConcentricGap *const, const object_creation::Geometry::ConcentricGap::Metadata &) const;

template StatusCode PandoraContentApiImpl::Create(const object_creation::Geometry::SubDetector::Parameters &, const SubDetector *&, const ObjectFactory<object_creation::Geometry::SubDetector::Parameters, object_creation::Geometry::SubDetector::Object> &) const;
template StatusCode PandoraContentApiImpl::Create(const object_creation::Geometry::LArTPC::Parameters &, const LArTPC *&, const ObjectFactory<object_creation::Geometry::LArTPC::Parameters, object_creation::Geometry::LArTPC::Object> &) const;
template StatusCode PandoraContentApiImpl::Create(const object_creation::Geometry::LineGap::Parameters &, const LineGap *&, const ObjectFactory<object_creation::Geometry::LineGap::Parameters, object_creation::Geometry::LineGap::Object> &) const;
template StatusCode PandoraContentApiImpl::Create(const object_creation::Geometry::BoxGap::Parameters &, const BoxGap *&, const ObjectFactory<object_creation::Geometry::BoxGap::Parameters, object_creation::Geometry::BoxGap::Object> &) const;
template StatusCode PandoraContentApiImpl::Create(const object_creation::Geometry::ConcentricGap::Parameters &, const ConcentricGap *&, const ObjectFactory<object_creation::Geometry::ConcentricGap::Parameters, object_creation::Geometry::ConcentricGap::Object> &) const;

template StatusCode PandoraContentApiImpl::GetCurrentList<CaloHitList>(const CaloHitList *&, std::string &) const;
template StatusCode PandoraContentApiImpl::GetCurrentList<TrackList>(const TrackList *&, std::string &) const;
template StatusCode PandoraContentApiImpl::GetCurrentList<MCParticleList>(const MCParticleList *&, std::string &) const;
template StatusCode PandoraContentApiImpl::GetCurrentList<ClusterList>(const ClusterList *&, std::string &) const;
template StatusCode PandoraContentApiImpl::GetCurrentList<PfoList>(const PfoList *&, std::string &) const;
template StatusCode PandoraContentApiImpl::GetCurrentList<VertexList>(const VertexList *&, std::string &) const;

template StatusCode PandoraContentApiImpl::GetCurrentListName<CaloHit>(std::string &) const;
template StatusCode PandoraContentApiImpl::GetCurrentListName<Track>(std::string &) const;
template StatusCode PandoraContentApiImpl::GetCurrentListName<MCParticle>(std::string &) const;
template StatusCode PandoraContentApiImpl::GetCurrentListName<Cluster>(std::string &) const;
template StatusCode PandoraContentApiImpl::GetCurrentListName<ParticleFlowObject>(std::string &) const;
template StatusCode PandoraContentApiImpl::GetCurrentListName<Vertex>(std::string &) const;

template StatusCode PandoraContentApiImpl::ReplaceCurrentList<CaloHit>(const Algorithm &, const std::string &) const;
template StatusCode PandoraContentApiImpl::ReplaceCurrentList<Track>(const Algorithm &, const std::string &) const;
template StatusCode PandoraContentApiImpl::ReplaceCurrentList<MCParticle>(const Algorithm &, const std::string &) const;
template StatusCode PandoraContentApiImpl::ReplaceCurrentList<Cluster>(const Algorithm &, const std::string &) const;
template StatusCode PandoraContentApiImpl::ReplaceCurrentList<ParticleFlowObject>(const Algorithm &, const std::string &) const;
template StatusCode PandoraContentApiImpl::ReplaceCurrentList<Vertex>(const Algorithm &, const std::string &) const;

template StatusCode PandoraContentApiImpl::DropCurrentList<CaloHit>(const Algorithm &) const;
template StatusCode PandoraContentApiImpl::DropCurrentList<Track>(const Algorithm &) const;
template StatusCode PandoraContentApiImpl::DropCurrentList<MCParticle>(const Algorithm &) const;
template StatusCode PandoraContentApiImpl::DropCurrentList<Cluster>(const Algorithm &) const;
template StatusCode PandoraContentApiImpl::DropCurrentList<ParticleFlowObject>(const Algorithm &) const;
template StatusCode PandoraContentApiImpl::DropCurrentList<Vertex>(const Algorithm &) const;

template StatusCode PandoraContentApiImpl::GetList<CaloHitList>(const std::string &, const CaloHitList *&) const;
template StatusCode PandoraContentApiImpl::GetList<TrackList>(const std::string &, const TrackList *&) const;
template StatusCode PandoraContentApiImpl::GetList<MCParticleList>(const std::string &, const MCParticleList *&) const;
template StatusCode PandoraContentApiImpl::GetList<ClusterList>(const std::string &, const ClusterList *&) const;
template StatusCode PandoraContentApiImpl::GetList<PfoList>(const std::string &, const PfoList *&) const;
template StatusCode PandoraContentApiImpl::GetList<VertexList>(const std::string &, const VertexList *&) const;

template StatusCode PandoraContentApiImpl::RenameList<CaloHitList>(const std::string &, const std::string &) const;
template StatusCode PandoraContentApiImpl::RenameList<TrackList>(const std::string &, const std::string &) const;
template StatusCode PandoraContentApiImpl::RenameList<MCParticleList>(const std::string &, const std::string &) const;
template StatusCode PandoraContentApiImpl::RenameList<ClusterList>(const std::string &, const std::string &) const;
template StatusCode PandoraContentApiImpl::RenameList<PfoList>(const std::string &, const std::string &) const;
template StatusCode PandoraContentApiImpl::RenameList<VertexList>(const std::string &, const std::string &) const;

template StatusCode PandoraContentApiImpl::SaveList<CaloHitList>(const CaloHitList &, const std::string &) const;
template StatusCode PandoraContentApiImpl::SaveList<TrackList>(const TrackList &, const std::string &) const;
template StatusCode PandoraContentApiImpl::SaveList<MCParticleList>(const MCParticleList &, const std::string &) const;

template StatusCode PandoraContentApiImpl::SaveList<Cluster>(const std::string &) const;
template StatusCode PandoraContentApiImpl::SaveList<ParticleFlowObject>(const std::string &) const;
template StatusCode PandoraContentApiImpl::SaveList<Vertex>(const std::string &) const;

template StatusCode PandoraContentApiImpl::SaveList<Cluster>(const std::string &, const std::string &) const;
template StatusCode PandoraContentApiImpl::SaveList<ParticleFlowObject>(const std::string &, const std::string &) const;
template StatusCode PandoraContentApiImpl::SaveList<Vertex>(const std::string &, const std::string &) const;

template StatusCode PandoraContentApiImpl::SaveList<ClusterList>(const std::string &, const ClusterList &) const;
template StatusCode PandoraContentApiImpl::SaveList<PfoList>(const std::string &, const PfoList &) const;
template StatusCode PandoraContentApiImpl::SaveList<VertexList>(const std::string &, const VertexList &) const;

template StatusCode PandoraContentApiImpl::SaveList<ClusterList>(const std::string &, const std::string &, const ClusterList &) const;
template StatusCode PandoraContentApiImpl::SaveList<PfoList>(const std::string &, const std::string &, const PfoList &) const;
template StatusCode PandoraContentApiImpl::SaveList<VertexList>(const std::string &, const std::string &, const VertexList &) const;

template StatusCode PandoraContentApiImpl::TemporarilyReplaceCurrentList<Cluster>(const std::string &) const;
template StatusCode PandoraContentApiImpl::TemporarilyReplaceCurrentList<ParticleFlowObject>(const std::string &) const;
template StatusCode PandoraContentApiImpl::TemporarilyReplaceCurrentList<Vertex>(const std::string &) const;

template StatusCode PandoraContentApiImpl::CreateTemporaryListAndSetCurrent<ClusterList>(const Algorithm &, const ClusterList *&, std::string &) const;
template StatusCode PandoraContentApiImpl::CreateTemporaryListAndSetCurrent<PfoList>(const Algorithm &, const PfoList *&, std::string &) const;
template StatusCode PandoraContentApiImpl::CreateTemporaryListAndSetCurrent<VertexList>(const Algorithm &, const VertexList *&, std::string &) const;

template bool PandoraContentApiImpl::IsAvailable<CaloHit>(const CaloHit *) const;
template bool PandoraContentApiImpl::IsAvailable<Track>(const Track *) const;
template bool PandoraContentApiImpl::IsAvailable<Cluster>(const Cluster *) const;
template bool PandoraContentApiImpl::IsAvailable<Vertex>(const Vertex *) const;
template bool PandoraContentApiImpl::IsAvailable<CaloHitList>(const CaloHitList *) const;
template bool PandoraContentApiImpl::IsAvailable<TrackList>(const TrackList *) const;
template bool PandoraContentApiImpl::IsAvailable<ClusterList>(const ClusterList *) const;
template bool PandoraContentApiImpl::IsAvailable<VertexList>(const VertexList *) const;

template StatusCode PandoraContentApiImpl::AddToPfo<Cluster>(const ParticleFlowObject *, const Cluster *) const;
template StatusCode PandoraContentApiImpl::AddToPfo<Track>(const ParticleFlowObject *, const Track *) const;
template StatusCode PandoraContentApiImpl::AddToPfo<Vertex>(const ParticleFlowObject *, const Vertex *) const;

template StatusCode PandoraContentApiImpl::RemoveFromPfo<Cluster>(const ParticleFlowObject *, const Cluster *) const;
template StatusCode PandoraContentApiImpl::RemoveFromPfo<Track>(const ParticleFlowObject *, const Track *) const;
template StatusCode PandoraContentApiImpl::RemoveFromPfo<Vertex>(const ParticleFlowObject *, const Vertex *) const;

template StatusCode PandoraContentApiImpl::PrepareForDeletion<Cluster>(const Cluster *const) const;
template StatusCode PandoraContentApiImpl::PrepareForDeletion<ParticleFlowObject>(const ParticleFlowObject *const) const;
template StatusCode PandoraContentApiImpl::PrepareForDeletion<Vertex>(const Vertex *const) const;

template StatusCode PandoraContentApiImpl::Delete<Cluster>(const Cluster *const) const;
template StatusCode PandoraContentApiImpl::Delete<ParticleFlowObject>(const ParticleFlowObject *const) const;
template StatusCode PandoraContentApiImpl::Delete<Vertex>(const Vertex *const) const;
template StatusCode PandoraContentApiImpl::Delete<ClusterList>(const ClusterList *const) const;
template StatusCode PandoraContentApiImpl::Delete<PfoList>(const PfoList *const) const;
template StatusCode PandoraContentApiImpl::Delete<VertexList>(const VertexList *const) const;

template StatusCode PandoraContentApiImpl::Delete<Cluster>(const Cluster *const, const std::string &) const;
template StatusCode PandoraContentApiImpl::Delete<ParticleFlowObject>(const ParticleFlowObject *const, const std::string &) const;
template StatusCode PandoraContentApiImpl::Delete<Vertex>(const Vertex *const, const std::string &) const;

} // namespace pandora
