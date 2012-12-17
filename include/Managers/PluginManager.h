/**
 *  @file   PandoraPFANew/Framework/include/Managers/PluginManager.h
 * 
 *  @brief  Header file for the pandora plugin manager class.
 * 
 *  $Log: $
 */
#ifndef PLUGIN_MANAGER_H
#define PLUGIN_MANAGER_H 1

#include "Pandora/PandoraInputTypes.h"
#include "Pandora/PandoraInternal.h"
#include "Pandora/StatusCodes.h"

class TiXmlHandle;

namespace pandora
{

/**
 *  @brief PluginManager class
 */
class PluginManager
{
public:
    /**
     *  @brief  Default constructor
     */
    PluginManager();

    /**
     *  @brief  Destructor
     */
    ~PluginManager();

private:
    typedef std::map<std::string, EnergyCorrectionFunction *> EnergyCorrectionFunctionMap;
    typedef std::map<std::string, ParticleIdFunction *> ParticleIdFunctionMap;

    /**
     *  @brief  Register an energy correction function
     * 
     *  @param  functionName the name/label associated with the energy correction function
     *  @param  energyCorrectionType the energy correction type
     *  @param  energyCorrectionFunction pointer to an energy correction function
     */
    StatusCode RegisterEnergyCorrectionFunction(const std::string &functionName, const EnergyCorrectionType energyCorrectionType,
        EnergyCorrectionFunction *pEnergyCorrectionFunction);

    /**
     *  @brief  Read requested function names/labels from a specified xml tag and attempt to assign the function pointers as requested
     * 
     *  @param  pXmlHandle address of the relevant xml handle
     *  @param  xmlTagName the xml tag name for a given energy correction type
     *  @param  energyCorrectionType the energy correction type
     *  @param  energyCorrectionFunctionVector to receive the addresses of the energy correction functions
     */
    StatusCode InitializeEnergyCorrectionFunctions(const TiXmlHandle *const pXmlHandle, const std::string &xmlTagName,
        const EnergyCorrectionType energyCorrectionType, EnergyCorrectionFunctionVector &energyCorrectionFunctionVector);

    /**
     *  @brief  Match a vector of names/labels to energy correction functions and store pointers to these functions in a vector
     * 
     *  @param  functionNames the vector of names/labels associated with energy correction functions
     *  @param  energyCorrectionType the energy correction type
     *  @param  energyCorrectionFunctionVector to receive the addresses of the energy correction functions
     */
    StatusCode AssignEnergyCorrectionFunctions(const StringVector &functionNames, const EnergyCorrectionType energyCorrectionType,
        EnergyCorrectionFunctionVector &energyCorrectionFunctionVector);

    /**
     *  @brief  Get the energy correction function map corresponding to the specified energy correction type
     * 
     *  @param  energyCorrectionType the energy correction type
     * 
     *  @return reference to the relevant energy correction function map
     */
    EnergyCorrectionFunctionMap &GetEnergyCorrectionFunctionMap(const EnergyCorrectionType energyCorrectionType);

    /**
     *  @brief  Register a particle id function
     * 
     *  @param  functionName the name/label associated with the particle id function
     *  @param  particleIdFunction pointer to a particle id function
     */
    StatusCode RegisterParticleIdFunction(const std::string &functionName, ParticleIdFunction *pParticleIdFunction);

    /**
     *  @brief  Read requested function name/label from a specified xml tag and attempt to assign the function pointer as requested
     * 
     *  @param  pXmlHandle address of the relevant xml handle
     *  @param  xmlTagName the xml tag name for a given particle id "slot"
     *  @param  pParticleIdFunction to receive the address of the particle id function
     */
    StatusCode InitializeParticleIdFunction(const TiXmlHandle *const pXmlHandle, const std::string &xmlTagName,
        ParticleIdFunction *&pParticleIdFunction);

    /**
     *  @brief  Match a name/label to a particle id function and assign address of the function to a function pointer
     * 
     *  @param  functionName the name/label associated with the particle id function
     *  @param  pParticleIdFunction to receive the address of the particle id function
     */
    StatusCode AssignParticleIdFunction(const std::string &functionName, ParticleIdFunction *&pParticleIdFunction) const;

    /**
     *  @brief  Initialize plugins
     * 
     *  @param  pXmlHandle address of the relevant xml handle
     */
    StatusCode InitializePlugins(const TiXmlHandle *const pXmlHandle);

    EnergyCorrectionFunctionMap     m_hadEnergyCorrectionFunctionMap;   ///< The hadronic energy correction function map
    EnergyCorrectionFunctionMap     m_emEnergyCorrectionFunctionMap;    ///< The electromagnetic energy correction function map
    ParticleIdFunctionMap           m_particleIdFunctionMap;            ///< The particle id function map

    friend class PandoraApiImpl;
    friend class PandoraImpl;
};

} // namespace pandora

#endif // #ifndef PLUGIN_MANAGER_H
