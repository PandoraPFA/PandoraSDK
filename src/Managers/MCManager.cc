/**
 *  @file   PandoraSDK/src/Managers/MCManager.cc
 * 
 *  @brief  Implementation of the mc manager class.
 * 
 *  $Log: $
 */

#include "Managers/MCManager.h"

#include "Objects/MCParticle.h"

#include "Pandora/ObjectFactory.h"
#include "Pandora/Pandora.h"
#include "Pandora/PandoraInternal.h"
#include "Pandora/PandoraSettings.h"
#include "Pandora/PdgTable.h"

#include <algorithm>

namespace pandora
{

MCManager::MCManager(const Pandora *const pPandora) :
    InputObjectManager<MCParticle>(pPandora),
    m_selectedListName("Selected")
{
    PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->CreateInitialLists());
}

//------------------------------------------------------------------------------------------------------------------------------------------

MCManager::~MCManager()
{
    (void) this->EraseAllContent();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCManager::Create(const object_creation::MCParticle::Parameters &parameters, const MCParticle *&pMCParticle,
    const ObjectFactory<object_creation::MCParticle::Parameters, object_creation::MCParticle::Object> &factory)
{
    pMCParticle = nullptr;

    try
    {
        PANDORA_THROW_RESULT_IF(STATUS_CODE_SUCCESS, !=, factory.Create(parameters, pMCParticle));

        NameToListMap::iterator inputIter = m_nameToListMap.find(m_inputListName);

        if (!pMCParticle || (m_nameToListMap.end() == inputIter))
            throw StatusCodeException(STATUS_CODE_FAILURE);

        if (!m_uidToMCParticleMap.insert(UidToMCParticleMap::value_type(pMCParticle->GetUid(), pMCParticle)).second)
            throw StatusCodeException(STATUS_CODE_ALREADY_PRESENT);

        inputIter->second->push_back(pMCParticle);
        return STATUS_CODE_SUCCESS;
    }
    catch (StatusCodeException &statusCodeException)
    {
        std::cout << "Failed to create mc particle: " << statusCodeException.ToString() << std::endl;
        delete pMCParticle;
        pMCParticle = nullptr;
        return statusCodeException.GetStatusCode();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCManager::EraseAllContent()
{
    m_uidToMCParticleMap.clear();
    m_parentDaughterRelationMap.clear();
    m_caloHitToMCParticleMap.clear();
    m_trackToMCParticleMap.clear();

    return InputObjectManager<MCParticle>::EraseAllContent();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCManager::SetMCParentDaughterRelationship(const Uid parentUid, const Uid daughterUid)
{
    m_parentDaughterRelationMap.insert(MCParticleRelationMap::value_type(parentUid, daughterUid));

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCManager::IdentifyPfoTargets()
{
    NameToListMap::const_iterator inputIter = m_nameToListMap.find(m_inputListName);

    if (m_nameToListMap.end() == inputIter)
        return STATUS_CODE_FAILURE;

    for (const MCParticle *const pMCParticle : *inputIter->second)
    {
        MCParticleSet mcPfoSet;

        if (pMCParticle->IsRootParticle())
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ApplyPfoSelectionRules(pMCParticle, mcPfoSet));
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCManager::SelectPfoTargets()
{
    // Check for presence of input list and remove any existing selected list
    NameToListMap::const_iterator inputIter = m_nameToListMap.find(m_inputListName);

    if (m_nameToListMap.end() == inputIter)
        return STATUS_CODE_FAILURE;

    NameToListMap::iterator selectedIter = m_nameToListMap.find(m_selectedListName);

    if (m_nameToListMap.end() != selectedIter)
    {
        ObjectList *const pObjectList(selectedIter->second);
        selectedIter = m_nameToListMap.erase(selectedIter);
        delete pObjectList;
    }

    // Strip down mc particles and relationships to just those of pfo targets, if specified
    const bool shouldCollapseMCParticlesToPfoTarget(m_pPandora->GetSettings()->ShouldCollapseMCParticlesToPfoTarget());

    MCParticleList selectedMCPfoList;

    for (const MCParticle *const pMCParticle : *inputIter->second)
    {
        const bool isPfoTarget(pMCParticle->IsPfoTarget());

        if (!isPfoTarget && shouldCollapseMCParticlesToPfoTarget)
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->RemoveMCParticleRelationships(pMCParticle));
        }

        if (isPfoTarget || !shouldCollapseMCParticlesToPfoTarget)
        {
            selectedMCPfoList.push_back(pMCParticle);
        }
    }

    // Save selected pfo target list
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->SaveList(m_selectedListName, selectedMCPfoList));
    m_currentListName = m_selectedListName;

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCManager::ApplyPfoSelectionRules(const MCParticle *const pMCParticle, MCParticleSet &mcPfoSet) const
{
    const float selectionRadius(m_pPandora->GetSettings()->GetMCPfoSelectionRadius());
    const float selectionMomentum(m_pPandora->GetSettings()->GetMCPfoSelectionMomentum());
    const float selectionEnergyCutOffProtonsNeutrons(m_pPandora->GetSettings()->GetMCPfoSelectionLowEnergyNeutronProtonCutOff());

    const int particleId(pMCParticle->GetParticleId());

    // ATTN: Don't take particles from previously used decay chains; could happen because mc particles can have multiple parents.
    if (!mcPfoSet.count(pMCParticle) &&
        (pMCParticle->GetOuterRadius() > selectionRadius) &&
        (pMCParticle->GetInnerRadius() <= selectionRadius) &&
        (pMCParticle->GetMomentum().GetMagnitude() > selectionMomentum) &&
        !((particleId == PROTON || particleId == NEUTRON) && (pMCParticle->GetEnergy() < selectionEnergyCutOffProtonsNeutrons)))
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->SetPfoTargetInTree(pMCParticle, pMCParticle, true));
        mcPfoSet.insert(pMCParticle);
    }
    else
    {
        for (const MCParticle *const pDaughterMCParticle : pMCParticle->GetDaughterList())
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->ApplyPfoSelectionRules(pDaughterMCParticle, mcPfoSet));
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCManager::SetPfoTargetInTree(const MCParticle *const pMCParticle, const MCParticle *const pPfoTarget, bool onlyDaughters) const
{
    if (pMCParticle->IsPfoTargetSet())
        return STATUS_CODE_SUCCESS;

    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->Modifiable(pMCParticle)->SetPfoTarget(pPfoTarget));

    for (const MCParticle *const pDaughterMCParticle : pMCParticle->GetDaughterList())
    {
       PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->SetPfoTargetInTree(pDaughterMCParticle, pPfoTarget));
    }

    if (!onlyDaughters)
    {
        for (const MCParticle *const pParentMCParticle : pMCParticle->GetParentList())
        {
            PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->SetPfoTargetInTree(pParentMCParticle, pPfoTarget));
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCManager::AddMCParticleRelationships() const
{
    if (m_parentDaughterRelationMap.empty())
        return STATUS_CODE_SUCCESS;

    const MCParticleList *pInputList(nullptr);
    PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->GetList(m_inputListName, pInputList));

    for (const MCParticle *const pParentMCParticle : *pInputList)
    {
        const auto range(m_parentDaughterRelationMap.equal_range(pParentMCParticle->GetUid()));

        MCParticleList daughterList;
        for (MCParticleRelationMap::const_iterator relIter = range.first; relIter != range.second; ++relIter)
        {
            UidToMCParticleMap::const_iterator daughterIter = m_uidToMCParticleMap.find(relIter->second);

            if ((m_uidToMCParticleMap.end() != daughterIter) && (daughterList.end() == std::find(daughterList.begin(), daughterList.end(), daughterIter->second)))
                daughterList.push_back(daughterIter->second);
        }
        daughterList.sort(PointerLessThan<MCParticle>());

        for (const MCParticle *const pDaughterMCParticle : daughterList)
        {
            const StatusCode firstStatusCode(this->Modifiable(pParentMCParticle)->AddDaughter(pDaughterMCParticle));
            const StatusCode secondStatusCode(this->Modifiable(pDaughterMCParticle)->AddParent(pParentMCParticle));

            if (firstStatusCode != secondStatusCode)
                return STATUS_CODE_FAILURE;

            if ((firstStatusCode != STATUS_CODE_SUCCESS) && (firstStatusCode != STATUS_CODE_ALREADY_PRESENT))
                return firstStatusCode;
        }
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCManager::RemoveAllMCParticleRelationships()
{
    NameToListMap::const_iterator inputIter = m_nameToListMap.find(m_inputListName);

    if (m_nameToListMap.end() == inputIter)
        return STATUS_CODE_FAILURE;

    for (const MCParticle *const pMCParticle : *inputIter->second)
        this->RemoveMCParticleRelationships(pMCParticle);

    m_uidToMCParticleMap.clear();
    m_parentDaughterRelationMap.clear();
    m_caloHitToMCParticleMap.clear();
    m_trackToMCParticleMap.clear();

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCManager::RemoveMCParticleRelationships(const MCParticle *const pMCParticle) const
{
    const MCParticleList parentList(pMCParticle->GetParentList());

    for (const MCParticle *const pParentMCParticle : parentList)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->Modifiable(pParentMCParticle)->RemoveDaughter(pMCParticle));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->Modifiable(pMCParticle)->RemoveParent(pParentMCParticle));
    }

    const MCParticleList daughterList(pMCParticle->GetDaughterList());

    for (const MCParticle *const pDaughterMCParticle : daughterList)
    {
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->Modifiable(pDaughterMCParticle)->RemoveParent(pMCParticle));
        PANDORA_RETURN_RESULT_IF(STATUS_CODE_SUCCESS, !=, this->Modifiable(pMCParticle)->RemoveDaughter(pDaughterMCParticle));
    }

    return this->Modifiable(pMCParticle)->RemovePfoTarget();
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCManager::SetUidToMCParticleRelationship(const Uid objectUid, const Uid mcParticleUid, const float mcParticleWeight,
    ObjectRelationMap &objectRelationMap) const
{
    const bool useSingleMCParticleAssociation(m_pPandora->GetSettings()->UseSingleMCParticleAssociation());
    ObjectRelationMap::iterator iter = objectRelationMap.find(objectUid);

    if (objectRelationMap.end() != iter)
    {
        UidToWeightMap &uidToWeightMap(iter->second);

        if (useSingleMCParticleAssociation && (mcParticleWeight < uidToWeightMap.begin()->second))
            return STATUS_CODE_SUCCESS;

        if (useSingleMCParticleAssociation)
            uidToWeightMap.clear();

        uidToWeightMap[mcParticleUid] += mcParticleWeight;
    }
    else
    {
        UidToWeightMap uidToWeightMap;

        if (!uidToWeightMap.insert(UidToWeightMap::value_type(mcParticleUid, mcParticleWeight)).second)
            return STATUS_CODE_FAILURE;

        if (!objectRelationMap.insert(ObjectRelationMap::value_type(objectUid, uidToWeightMap)).second)
            return STATUS_CODE_FAILURE;
    }

    return STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

StatusCode MCManager::CreateUidToPfoTargetsMap(UidToMCParticleWeightMap &uidToMCParticleWeightMap, const ObjectRelationMap &objectRelationMap) const
{
    if (m_uidToMCParticleMap.empty())
        return STATUS_CODE_SUCCESS;

    const bool collapseToPfoTarget(m_pPandora->GetSettings()->ShouldCollapseMCParticlesToPfoTarget());

    for (const ObjectRelationMap::value_type &relationEntry : objectRelationMap)
    {
        MCParticleSet mcParticleSet;

        for (const UidToWeightMap::value_type &weightEntry : relationEntry.second)
        {
            UidToMCParticleMap::const_iterator mcParticleIter = m_uidToMCParticleMap.find(weightEntry.first);

            if (m_uidToMCParticleMap.end() != mcParticleIter)
                mcParticleSet.insert(mcParticleIter->second);
        }

        if (mcParticleSet.empty())
            continue;

        MCParticleList mcParticleList(mcParticleSet.begin(), mcParticleSet.end());
        mcParticleList.sort(PointerLessThan<MCParticle>());
        MCParticleWeightMap &mcParticleWeightMap(uidToMCParticleWeightMap[relationEntry.first]);

        for (const MCParticle *const pMCParticle : mcParticleList)
        {
            const float mcParticleWeight(relationEntry.second.at(pMCParticle->GetUid()));
            const MCParticle *const pTargetMCParticle(!collapseToPfoTarget ? pMCParticle : pMCParticle->m_pPfoTarget);

            if (!pTargetMCParticle)
                continue;

            mcParticleWeightMap[pTargetMCParticle] += mcParticleWeight;
        }
    }

    return STATUS_CODE_SUCCESS;
}

} // namespace pandora
