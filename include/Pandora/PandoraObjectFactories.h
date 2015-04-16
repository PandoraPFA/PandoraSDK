/**
 *  @file   PandoraSDK/include/Pandora/PandoraObjectFactories.h
 * 
 *  @brief  Header file for the pandora object factories classes.
 * 
 *  $Log: $
 */
#ifndef PANDORA_OBJECT_FACTORIES_H
#define PANDORA_OBJECT_FACTORIES_H 1

#include "Pandora/ObjectFactory.h"

namespace pandora
{

class PandoraApiImpl;
class PandoraContentApiImpl;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
 *  @brief  PandoraObjectFactory class
 */
template <typename PARAMETERS, typename OBJECT>
class PandoraObjectFactory : public ObjectFactory<PARAMETERS, OBJECT>
{
private:
    /**
     *  @brief  Create an object with the given parameters
     *
     *  @param  parameters the parameters to pass in constructor
     *  @param  pObject to receive the address of the object created
     */
    StatusCode Create(const PARAMETERS &parameters, const OBJECT *&pObject) const;

    friend class PandoraApiImpl;
    friend class PandoraContentApiImpl;
};

} // namespace pandora

#endif // #ifndef PANDORA_OBJECT_FACTORY_H
