/**
 *  @file   PandoraSDK/include/Api/PandoraApi.h
 *
 *  @brief  Header file for the pandora api class.
 *
 *  $Log: $
 */
#ifndef PANDORA_API_H
#define PANDORA_API_H 1

#include "Pandora/ObjectCreation.h"
#include "Pandora/Pandora.h"
#include "Pandora/PandoraObjectFactories.h"

namespace pandora { class AlgorithmFactory; class AlgorithmToolFactory; }

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  PandoraApi class
 */
class PandoraApi
{
public:
    /* Map object creation into PandoraApi */
    typedef object_creation::CaloHit CaloHit;
    typedef object_creation::MCParticle MCParticle;
    typedef object_creation::Track Track;

    /**
     *  @brief  Geometry class
     */
    class Geometry
    {
    public:
        typedef object_creation::Geometry::LayerParameters LayerParameters;
        typedef object_creation::Geometry::SubDetector SubDetector;
        typedef object_creation::Geometry::LArTPC LArTPC;
        typedef object_creation::Geometry::LineGap LineGap;
        typedef object_creation::Geometry::BoxGap BoxGap;
        typedef object_creation::Geometry::ConcentricGap ConcentricGap;
    };

    /**
     *  @brief  Process an event
     * 
     *  @param  pandora the pandora instance to process event
     */
    static pandora::StatusCode ProcessEvent(const pandora::Pandora &pandora);

    /**
     *  @brief  Read pandora settings
     * 
     *  @param  pandora the pandora instance to run the algorithms initialize
     *  @param  xmlFileName the name of the xml file containing the settings
     */
    static pandora::StatusCode ReadSettings(const pandora::Pandora &pandora, const std::string &xmlFileName);

    /**
     *  @brief  Register an algorithm factory with pandora
     * 
     *  @param  pandora the pandora instance to register the algorithm factory with
     *  @param  algorithmType the type of algorithm that the factory will create
     *  @param  pAlgorithmFactory the address of an algorithm factory instance
     */
    static pandora::StatusCode RegisterAlgorithmFactory(const pandora::Pandora &pandora, const std::string &algorithmType,
        pandora::AlgorithmFactory *const pAlgorithmFactory);

    /**
     *  @brief  Register an algorithm tool factory with pandora
     * 
     *  @param  pandora the pandora instance to register the algorithm tool factory with
     *  @param  algorithmTool Type the type of algorithm tool that the factory will create
     *  @param  pAlgorithmToolFactory the address of an algorithm tool factory instance
     */
    static pandora::StatusCode RegisterAlgorithmToolFactory(const pandora::Pandora &pandora, const std::string &algorithmToolType,
        pandora::AlgorithmToolFactory *const pAlgorithmToolFactory);

    /**
     *  @brief  Set parent-daughter mc particle relationship
     * 
     *  @param  pandora the pandora instance to register the relationship with
     *  @param  pParentAddress address of parent mc particle in the user framework
     *  @param  pDaughterAddress address of daughter mc particle in the user framework
     */
    static pandora::StatusCode SetMCParentDaughterRelationship(const pandora::Pandora &pandora, const void *const pParentAddress,
        const void *const pDaughterAddress);

    /**
     *  @brief  Set parent-daughter track relationship
     * 
     *  @param  pandora the pandora instance to register the relationship with
     *  @param  pParentAddress address of parent track in the user framework
     *  @param  pDaughterAddress address of daughter track in the user framework
     */
    static pandora::StatusCode SetTrackParentDaughterRelationship(const pandora::Pandora &pandora, const void *const pParentAddress,
        const void *const pDaughterAddress);

    /**
     *  @brief  Set sibling track relationship
     * 
     *  @param  pandora the pandora instance to register the relationship with
     *  @param  pFirstSiblingAddress address of first sibling track in the user framework
     *  @param  pSecondSiblingAddress address of second sibling track in the user framework
     */
    static pandora::StatusCode SetTrackSiblingRelationship(const pandora::Pandora &pandora, const void *const pFirstSiblingAddress,
        const void *const pSecondSiblingAddress);

    /**
     *  @brief  Set calo hit to mc particle relationship
     * 
     *  @param  pandora the pandora instance to register the relationship with
     *  @param  pCaloHitParentAddress address of calo hit in the user framework
     *  @param  pMCParticleParentAddress address of mc particle in the user framework
     *  @param  mcParticleWeight weighting to assign to the mc particle
     */
    static pandora::StatusCode SetCaloHitToMCParticleRelationship(const pandora::Pandora &pandora, const void *const pCaloHitParentAddress,
        const void *const pMCParticleParentAddress, const float mcParticleWeight = 1);

    /**
     *  @brief  Set track to mc particle relationship
     * 
     *  @param  pandora the pandora instance to register the relationship with
     *  @param  pTrackParentAddress address of track in the user framework
     *  @param  pMCParticleParentAddress address of mc particle in the user framework
     *  @param  mcParticleWeight weighting to assign to the mc particle
     */
    static pandora::StatusCode SetTrackToMCParticleRelationship(const pandora::Pandora &pandora, const void *const pTrackParentAddress,
        const void *const pMCParticleParentAddress, const float mcParticleWeight = 1);

    /**
     *  @brief  Get the current pfo list
     * 
     *  @param  pandora the pandora instance to get the objects from
     *  @param  pPfoList to receive the address of the particle flow objects
     */
    static pandora::StatusCode GetCurrentPfoList(const pandora::Pandora &pandora, const pandora::PfoList *&pPfoList);

    /**
     *  @brief  Get a named pfo list
     * 
     *  @param  pandora the pandora instance to get the objects from
     *  @param  pfoListName the name of the pfo list
     *  @param  pPfoList to receive the address of the pfo list
     */
    static pandora::StatusCode GetPfoList(const pandora::Pandora &pandora, const std::string &pfoListName, const pandora::PfoList *&pPfoList);

    /**
     *  @brief  Set the external parameters associated with an algorithm instance of a specific type. It is enforced that there
     *          be only a single instance of an externally-configured algorithm, per algorithm type, per Pandora instance
     *
     *  @param  pandora the pandora instance
     *  @param  algorithmType the algorithm type
     *  @param  pExternalParameters the address of the external parameters instance
     */
    static pandora::StatusCode SetExternalParameters(const pandora::Pandora &pandora, const std::string &algorithmType,
        pandora::ExternalParameters *const pExternalParameters);

    /**
     *  @brief  Set the granularity level to be associated with a specified hit type
     * 
     *  @param  pandora the pandora instance to register the hit type to granularity relationship
     *  @param  hitType the specified hit type
     *  @param  granularity the specified granularity
     */
    static pandora::StatusCode SetHitTypeGranularity(const pandora::Pandora &pandora, const pandora::HitType hitType,
        const pandora::Granularity granularity);

    /**
     *  @brief  Set the bfield plugin used by pandora
     * 
     *  @param  pandora the pandora instance to register the bfield plugin with
     *  @param  pBFieldPlugin address of the bfield plugin (will pass ownership to pandora)
     */
    static pandora::StatusCode SetBFieldPlugin(const pandora::Pandora &pandora, pandora::BFieldPlugin *const pBFieldPlugin);

    /**
     *  @brief  Set the lar transformation plugin used by pandora
     * 
     *  @param  pandora the pandora instance to register the lar transformation plugin with
     *  @param  pLArTransformationPlugin address of the lar transformation plugin (will pass ownership to pandora)
     */
    static pandora::StatusCode SetLArTransformationPlugin(const pandora::Pandora &pandora, pandora::LArTransformationPlugin *const pLArTransformationPlugin);

    /**
     *  @brief  Set the pseudo layer plugin used by pandora
     * 
     *  @param  pandora the pandora instance to register the pseudo layer plugin with
     *  @param  pPseudoLayerPlugin address of the pseudo layer plugin (will pass ownership to pandora)
     */
    static pandora::StatusCode SetPseudoLayerPlugin(const pandora::Pandora &pandora, pandora::PseudoLayerPlugin *const pPseudoLayerPlugin);

    /**
     *  @brief  Set the shower profile plugin used by pandora
     * 
     *  @param  pandora the pandora instance to register the shower profile plugin with
     *  @param  pPseudoLayerPlugin address of the pseudo layer plugin (will pass ownership to pandora)
     */
    static pandora::StatusCode SetShowerProfilePlugin(const pandora::Pandora &pandora, pandora::ShowerProfilePlugin *const pShowerProfilePlugin);

    /**
     *  @brief  Register an energy correction plugin
     * 
     *  @param  pandora the pandora instance with which to register the energy correction plugin
     *  @param  name the name/label associated with the energy correction plugin
     *  @param  energyCorrectionType the energy correction type
     *  @param  pEnergyCorrectionPlugin address of the energy correction plugin (will pass ownership to pandora)
     */
    static pandora::StatusCode RegisterEnergyCorrectionPlugin(const pandora::Pandora &pandora, const std::string &name,
        const pandora::EnergyCorrectionType energyCorrectionType, pandora::EnergyCorrectionPlugin *const pEnergyCorrectionPlugin);

    /**
     *  @brief  Register a particle id plugin
     * 
     *  @param  pandora the pandora instance with which to register the particle id plugin
     *  @param  functionName the name/label associated with the particle id plugin
     *  @param  pParticleIdPlugin address of the particle id plugin (will pass ownership to pandora)
     */
    static pandora::StatusCode RegisterParticleIdPlugin(const pandora::Pandora &pandora, const std::string &name,
        pandora::ParticleIdPlugin *const pParticleIdPlugin);

    /**
     *  @brief  Reset pandora to process another event
     * 
     *  @param  pandora the pandora instance to reset
     */
    static pandora::StatusCode Reset(const pandora::Pandora &pandora);
};

#endif // #ifndef PANDORA_API_H
