/**
 *  @file   PandoraPFANew/Framework/include/Objects/TrackState.h
 * 
 *  @brief  Header file for the track state class.
 * 
 *  $Log: $
 */
#ifndef TRACK_STATE_H
#define TRACK_STATE_H 1

#include "Objects/CartesianVector.h"

namespace pandora
{

/**
 *  @brief  TrackState class
 */
class TrackState
{
public:
    /**
     *  @brief  Constructor, from position and momentum components
     *
     *  @param  x the x position coordinate
     *  @param  y the y position coordinate
     *  @param  z the z position coordinate
     *  @param  px the momentum x component
     *  @param  py the momentum y component
     *  @param  pz the momentum z component
     */
    TrackState(float x, float y, float z, float px, float py, float pz);

    /**
     *  @brief  Constructor, from position and momentum vectors
     *
     *  @param  position the track position space pointe
     *  @param  momentum the momentum vector
     */
    TrackState(const CartesianVector &position, const CartesianVector &momentum);

    /**
     *  @brief  Get the track position vector
     * 
     *  @return the track position vector
     */
    const CartesianVector &GetPosition() const;

    /**
     *  @brief  Get the track momentum vector
     * 
     *  @return the track momentum vector
     */
    const CartesianVector &GetMomentum() const;

private:
    CartesianVector   m_position;     ///< The position space point
    CartesianVector   m_momentum;     ///< The momentum vector
};

/**
 *  @brief  Operator to dump track state properties to an ostream
 *
 *  @param  stream the target ostream
 *  @param  trackState the track state
 */
std::ostream &operator<<(std::ostream &stream, const TrackState &trackState);

//------------------------------------------------------------------------------------------------------------------------------------------

inline const CartesianVector &TrackState::GetPosition() const
{
    return m_position;
}

//------------------------------------------------------------------------------------------------------------------------------------------

inline const CartesianVector &TrackState::GetMomentum() const
{
    return m_momentum;
}

} // namespace pandora

#endif // #ifndef TRACK_STATE_H
