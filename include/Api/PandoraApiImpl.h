/**
 *  @file   PandoraSDK/include/Api/PandoraApiImpl.h
 *
 *  @brief  Header file for the pandora api implementation class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_API_IMPL_H
#define PANDORA_API_IMPL_H 1

#include "Api/PandoraApi.h"

namespace pandora
{

class Pandora;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *    @brief PandoraApiImpl class
 */
class PandoraApiImpl
{
private:
    /**
     *  @brief  Create an object for pandora
     * 
     *  @param  parameters the object parameters
     */
    template <typename PARAMETERS>
    StatusCode Create(const PARAMETERS &parameters) const;

    /**
     *  @brief  Process event
     */
    StatusCode ProcessEvent() const;

    /**
     *  @brief  Read pandora settings
     * 
     *  @param  xmlFileName the name of the xml file containing the settings
     */
    StatusCode ReadSettings(const std::string &xmlFileName) const;

    /**
     *  @brief  Register an algorithm factory with pandora
     * 
     *  @param  algorithmType the type of algorithm that the factory will create
     *  @param  pAlgorithmFactory the address of an algorithm factory instance
     */
    StatusCode RegisterAlgorithmFactory(const std::string &algorithmType, AlgorithmFactory *const pAlgorithmFactory) const;

    /**
     *  @brief  Register an algorithm tool factory with pandora
     * 
     *  @param  algorithmToolType the type of algorithm tool that the factory will create
     *  @param  pAlgorithmToolFactory the address of an algorithm tool factory instance
     */
    StatusCode RegisterAlgorithmToolFactory(const std::string &algorithmToolType, AlgorithmToolFactory *const pAlgorithmToolFactory) const;

    /**
     *  @brief  Set parent-daughter mc particle relationship
     * 
     *  @param  pParentAddress address of parent mc particle in the user framework
     *  @param  pDaughterAddress address of daughter mc particle in the user framework
     */
    StatusCode SetMCParentDaughterRelationship(const void *const pParentAddress, const void *const pDaughterAddress) const;

    /**
     *  @brief  Set parent-daughter track relationship
     * 
     *  @param  pParentAddress address of parent track in the user framework
     *  @param  pDaughterAddress address of daughter track in the user framework
     */
    StatusCode SetTrackParentDaughterRelationship(const void *const pParentAddress, const void *const pDaughterAddress) const;

    /**
     *  @brief  Set sibling track relationship
     * 
     *  @param  pFirstSiblingAddress address of first sibling track in the user framework
     *  @param  pSecondSiblingAddress address of second sibling track in the user framework
     */
    StatusCode SetTrackSiblingRelationship(const void *const pFirstSiblingAddress, const void *const pSecondSiblingAddress) const;

    /**
     *  @brief  Set calo hit to mc particle relationship
     * 
     *  @param  pCaloHitParentAddress address of calo hit in the user framework
     *  @param  pMCParticleParentAddress address of mc particle in the user framework
     *  @param  mcParticleWeight weighting to assign to the mc particle
     */
    StatusCode SetCaloHitToMCParticleRelationship(const void *const pCaloHitParentAddress, const void *const pMCParticleParentAddress,
        const float mcParticleWeight) const;

    /**
     *  @brief  Set track to mc particle relationship
     * 
     *  @param  pTrackParentAddress address of track in the user framework
     *  @param  pMCParticleParentAddress address of mc particle in the user framework
     *  @param  mcParticleWeight weighting to assign to the mc particle
     */
    StatusCode SetTrackToMCParticleRelationship(const void *const pTrackParentAddress, const void *const pMCParticleParentAddress,
        const float mcParticleWeight) const;

    /**
     *  @brief  Get the current pfo list
     * 
     *  @param  pPfoList to receive the address of the current pfo list
     *  @param  pfoListName to receive the current pfo list name
     */
    StatusCode GetCurrentPfoList(const PfoList *&pPfoList, std::string &pfoListName) const;

    /**
     *  @brief  Get a named pfo list
     * 
     *  @param  pfoListName the name of the pfo list
     *  @param  pPfoList to receive the address of the pfo list
     */
    StatusCode GetPfoList(const std::string &pfoListName, const PfoList *&pPfoList) const;

    /**
     *  @brief  Set the granularity level to be associated with a specified hit type
     * 
     *  @param  hitType the specified hit type
     *  @param  granularity the specified granularity
     */
    StatusCode SetHitTypeGranularity(const HitType hitType, const Granularity granularity) const;

    /**
     *  @brief  Set the bfield plugin used by pandora
     * 
     *  @param  pBFieldPlugin address of the bfield plugin (will pass ownership to pandora)
     */
    StatusCode SetBFieldPlugin(BFieldPlugin *const pBFieldPlugin) const;

    /**
     *  @brief  Set the pseudo layer plugin used by pandora
     * 
     *  @param  pPseudoLayerPlugin address of the pseudo layer plugin (will pass ownership to pandora)
     */
    StatusCode SetPseudoLayerPlugin(PseudoLayerPlugin *const pPseudoLayerPlugin) const;

    /**
     *  @brief  Set the shower profile plugin used by pandora
     * 
     *  @param  pPseudoLayerPlugin address of the pseudo layer plugin (will pass ownership to pandora)
     */
    StatusCode SetShowerProfilePlugin(ShowerProfilePlugin *const pShowerProfilePlugin) const;

    /**
     *  @brief  Register an energy correction plugin
     * 
     *  @param  name the name/label associated with the energy correction plugin
     *  @param  energyCorrectionType the energy correction type
     *  @param  pEnergyCorrectionPlugin address of the energy correction plugin (will pass ownership to pandora)
     */
    StatusCode RegisterEnergyCorrectionPlugin(const std::string &name, const EnergyCorrectionType energyCorrectionType,
        EnergyCorrectionPlugin *const pEnergyCorrectionPlugin) const;

    /**
     *  @brief  Register a particle id plugin
     * 
     *  @param  functionName the name/label associated with the particle id plugin
     *  @param  pParticleIdPlugin address of the particle id plugin (will pass ownership to pandora)
     */
    StatusCode RegisterParticleIdPlugin(const std::string &name, ParticleIdPlugin *const pParticleIdPlugin) const;

    /**
     *  @brief  Reset pandora to process another event
     */
    StatusCode ResetEvent() const;

    /**
     *  @brief  Constructor
     * 
     *  @param  pPandora address of the pandora object to interface
     */
    PandoraApiImpl(Pandora *const pPandora);

    Pandora    *m_pPandora;    ///< The pandora object to provide an interface to

    friend class Pandora;
    friend class PandoraImpl;
    friend class ::PandoraApi;
};

} // namespace pandora

#endif // #ifndef PANDORA_API_IMPL_H
