#ifndef KDTreeLinkerToolsTemplated_h
#define KDTreeLinkerToolsTemplated_h

#include "Objects/CaloHit.h"
#include "Objects/Track.h"
#include "Objects/CartesianVector.h"
#include "Api/PandoraContentApi.h"

#include <array>

namespace pandora {
  class Algorithm;
}

namespace lc_content_fast {

// Box structure used to define 2D field.
// It's used in KDTree building step to divide the detector
// space (ECAL, HCAL...) and in searching step to create a bounding
// box around the demanded point (Track collision point, PS projection...).
template<unsigned DIM>
struct KDTreeBoxT
{
  std::array<float,DIM> dimmin, dimmax;
  
  template<typename... Ts>
  KDTreeBoxT(Ts... dimargs) {
    static_assert(sizeof...(dimargs) == 2*DIM,"Constructor requires 2*DIM args");
    std::vector<float> dims = {dimargs...};
    for( unsigned i = 0; i < DIM; ++i ) {
      dimmin[i] = dims[2*i];
      dimmax[i] = dims[2*i+1];
    }
  }

  KDTreeBoxT() {}
};

typedef KDTreeBoxT<2> KDTreeBox;
typedef KDTreeBoxT<3> KDTreeCube;
typedef KDTreeBoxT<4> KDTreeTesseract;

  
// Data stored in each KDTree node.
// The dim1/dim2 fields are usually the duplication of some PFRecHit values
// (eta/phi or x/y). But in some situations, phi field is shifted by +-2.Pi
template<typename DATA,unsigned DIM>
struct KDTreeNodeInfoT 
{
  DATA data;
  std::array<float,DIM> dims;

public:
  KDTreeNodeInfoT()
  {}
  
  template<typename... Ts>
  KDTreeNodeInfoT(const DATA& d,Ts... dimargs)
  : data(d), dims{ {dimargs...} }
  {}
};

// KDTree node.
template <typename DATA, unsigned DIM>
struct KDTreeNodeT
{
  // Data
  KDTreeNodeInfoT<DATA,DIM> info;
  
  // Right/left sons.
  KDTreeNodeT<DATA,DIM> *left, *right;
  
  // Region bounding box.
  KDTreeBoxT<DIM> region;
  
  public:
  KDTreeNodeT()
    : left(0), right(0)
  {}
  
  void setAttributs(const KDTreeBoxT<DIM>& regionBox,
		    const KDTreeNodeInfoT<DATA,DIM>& infoToStore) 
  {
    info = infoToStore;
    region = regionBox;
  }
  
  void setAttributs(const KDTreeBoxT<DIM>&   regionBox) 
  {
    region = regionBox;
  }
};

std::pair<float,float> minmax(const float a, const float b);

template<typename T> 
struct kdtree_type_adaptor{
  static const pandora::CartesianVector& position(const T* t) { 
    return t->GetPosition(); 
  }
};

template<>
struct kdtree_type_adaptor<const pandora::Track>{
  static const pandora::CartesianVector& position(const pandora::Track* t) { 
    const pandora::TrackState & trackState(t->GetTrackStateAtCalorimeter());    
    return trackState.GetPosition(); 
  }
};

template<>
struct kdtree_type_adaptor<const pandora::CaloHit>{
  static const pandora::CartesianVector& position(const pandora::CaloHit* t) { 
    return t->GetPositionVector(); 
  }
};



template<typename T>
KDTreeCube fill_and_bound_3d_kd_tree(pandora::Algorithm* const caller,
				     const std::unordered_set<T*>& points,
				     std::vector<KDTreeNodeInfoT<T*,3> >& nodes,
				     bool passthru=false) {
  std::array<float,3> minpos{ {0.0f,0.0f,0.0f} }, maxpos{ {0.0f,0.0f,0.0f} };
  unsigned i = 0;
  for( T* point : points ) {
    if (!passthru && !PandoraContentApi::IsAvailable(*caller, point)) continue;
    const pandora::CartesianVector& pos = kdtree_type_adaptor<T>::position(point);
    nodes.emplace_back(point, (float)pos.GetX(), (float)pos.GetY(), (float)pos.GetZ());
    if( i == 0 ) {
      minpos[0] = pos.GetX(); minpos[1] = pos.GetY(); minpos[2] = pos.GetZ();
      maxpos[0] = pos.GetX(); maxpos[1] = pos.GetY(); maxpos[2] = pos.GetZ();
    } else {
      minpos[0] = std::min((float)pos.GetX(),minpos[0]);
      minpos[1] = std::min((float)pos.GetY(),minpos[1]);
      minpos[2] = std::min((float)pos.GetZ(),minpos[2]);
      maxpos[0] = std::max((float)pos.GetX(),maxpos[0]);
      maxpos[1] = std::max((float)pos.GetY(),maxpos[1]);
      maxpos[2] = std::max((float)pos.GetZ(),maxpos[2]);
    }
    ++i;
  }
  return KDTreeCube(minpos[0],maxpos[0],
		    minpos[1],maxpos[1],
		    minpos[2],maxpos[2]);
}

template<typename T>
KDTreeCube fill_and_bound_3d_kd_tree(const std::unordered_set<T*>& points,
				     std::vector<KDTreeNodeInfoT<T*,3> >& nodes) {
  std::array<float,3> minpos{ {0.0f,0.0f,0.0f} }, maxpos{ {0.0f,0.0f,0.0f} };
  unsigned i = 0;
  for( T* point : points ) {
    const pandora::CartesianVector& pos = kdtree_type_adaptor<T>::position(point);
    nodes.emplace_back(point, (float)pos.GetX(), (float)pos.GetY(), (float)pos.GetZ());
    if( i == 0 ) {
      minpos[0] = pos.GetX(); minpos[1] = pos.GetY(); minpos[2] = pos.GetZ();
      maxpos[0] = pos.GetX(); maxpos[1] = pos.GetY(); maxpos[2] = pos.GetZ();
    } else {
      minpos[0] = std::min((float)pos.GetX(),minpos[0]);
      minpos[1] = std::min((float)pos.GetY(),minpos[1]);
      minpos[2] = std::min((float)pos.GetZ(),minpos[2]);
      maxpos[0] = std::max((float)pos.GetX(),maxpos[0]);
      maxpos[1] = std::max((float)pos.GetY(),maxpos[1]);
      maxpos[2] = std::max((float)pos.GetZ(),maxpos[2]);
    }
    ++i;
  }
  return KDTreeCube(minpos[0],maxpos[0],
		    minpos[1],maxpos[1],
		    minpos[2],maxpos[2]);
}

template<typename T>
KDTreeCube fill_and_bound_3d_kd_tree_by_index(const std::vector<T*>& points,
					      std::vector<KDTreeNodeInfoT<unsigned,3> >& nodes) {
  std::array<float,3> minpos{ {0.0f,0.0f,0.0f} }, maxpos{ {0.0f,0.0f,0.0f} };
  unsigned i = 0;
  for( const T* point : points ) {
    const pandora::CartesianVector& pos = kdtree_type_adaptor<T>::position(point);
    nodes.emplace_back(i, (float)pos.GetX(), (float)pos.GetY(), (float)pos.GetZ());
    if( i == 0 ) {
      minpos[0] = pos.GetX(); minpos[1] = pos.GetY(); minpos[2] = pos.GetZ();
      maxpos[0] = pos.GetX(); maxpos[1] = pos.GetY(); maxpos[2] = pos.GetZ();
    } else {
      minpos[0] = std::min((float)pos.GetX(),minpos[0]);
      minpos[1] = std::min((float)pos.GetY(),minpos[1]);
      minpos[2] = std::min((float)pos.GetZ(),minpos[2]);
      maxpos[0] = std::max((float)pos.GetX(),maxpos[0]);
      maxpos[1] = std::max((float)pos.GetY(),maxpos[1]);
      maxpos[2] = std::max((float)pos.GetZ(),maxpos[2]);
    }
    ++i;
  }
  return KDTreeCube(minpos[0],maxpos[0],
		    minpos[1],maxpos[1],
		    minpos[2],maxpos[2]);
}

KDTreeTesseract fill_and_bound_4d_kd_tree(pandora::Algorithm* const  caller,
					  const std::unordered_set<const pandora::CaloHit*>& points,
					  std::vector<KDTreeNodeInfoT<const pandora::CaloHit*,4> >& nodes,
					  bool passthru=false);

KDTreeCube build_3d_kd_search_region( const pandora::CaloHit* point,
				      float x_span,
				      float y_span,
				      float z_span);

KDTreeTesseract build_4d_kd_search_region( const pandora::CaloHit* point,
					   float x_span,
					   float y_span,
					   float z_span,
					   float search_layer);

KDTreeTesseract build_4d_kd_search_region( const pandora::CartesianVector& point,
					   float x_span,
					   float y_span,
					   float z_span,
					   float search_layer);


} //lc_content_fast
#endif
