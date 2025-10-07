/**
 *  @file   PandoraSDK/include/Pandora/PandoraImpl.h
 * 
 *  @brief  Header file for the pandora impl class.
 * 
 *  $Log: $
 */
#ifndef PANDORA_IMPL_H
#define PANDORA_IMPL_H 1

namespace pandora
{

/**
 *  @brief  PandoraImpl class
 */
class PandoraImpl
{
private:
    /**
     *  @brief  Prepare mc particles: select mc pfo targets, match tracks and calo hits to the correct mc
     *          particles for particle flow
     */
    StatusCode PrepareMCParticles() const;

    /**
     *  @brief  Prepare tracks: add track associations (parent-daughter and sibling)
     */
    StatusCode PrepareTracks() const;

    /**
     *  @brief  Prepare calo hits: order the hits by pseudo layer, calculate density weights, identify
     *          isolated hits, identify possible mip hits and calculate surrounding energy values.
     */
    StatusCode PrepareCaloHits() const;

    /**
     *  @brief  Get the list of algorithms to be run by pandora
     *
     *  @return address of the list pandora algorithms
     */
    const StringVector &GetPandoraAlgorithms() const;

    /**
     *  @brief  Run an algorithm registered with pandora
     * 
     *  @param  algorithmName the name of the algorithm instance to run
     */
    StatusCode RunAlgorithm(const std::string &algorithmName) const;

    /**
     *  @brief  Initialize pandora settings
     * 
     *  @param  pXmlHandle address of the relevant xml handle
     */
    StatusCode InitializeSettings(const TiXmlHandle *const pXmlHandle) const;

    /**
     *  @brief  Initialize pandora algorithms
     * 
     *  @param  pXmlHandle address of the relevant xml handle
     */
    StatusCode InitializeAlgorithms(const TiXmlHandle *const pXmlHandle) const;

    /**
     *  @brief  Initialize pandora plugins
     * 
     *  @param  pXmlHandle address of the relevant xml handle
     */
    StatusCode InitializePlugins(const TiXmlHandle *const pXmlHandle) const;

    /**
     *  @brief  Ï event, calling manager reset functions and any registered reset functions
     */
    StatusCode ResetEvent() const;

    /**
     *  @brief  Constructor
     * 
     *  @param  pPandora address of the pandora object to interface
     */
    PandoraImpl(Pandora *const pPandora);

    Pandora                *m_pPandora;             ///< The pandora object to provide an interface to

    friend class Pandora;
};

} // namespace pandora

#endif // #ifndef PANDORA_IMPL_H
