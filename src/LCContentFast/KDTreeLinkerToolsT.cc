#include "LCContentFast/KDTreeLinkerToolsT.h"

namespace lc_content_fast {

std::pair<float,float> minmax(const float a, const float b){
  return ( b < a ? 
	   std::pair<float,float>(b, a) : 
	   std::pair<float,float>(a, b)   );
}

template<>
KDTreeCube fill_and_bound_3d_kd_tree<const pandora::Track>(pandora::Algorithm* const  /*caller*/,
							   const std::unordered_set<const pandora::Track*>& points,
							   std::vector<KDTreeNodeInfoT<const pandora::Track*,3> >& nodes,
							   bool passthru) {
  std::array<float,3> minpos{ {0.0f,0.0f,0.0f} }, maxpos{ {0.0f,0.0f,0.0f} };
  unsigned i = 0;
  for( const pandora::Track* point : points ) {
    if (!passthru && !point->CanFormPfo()) continue;
    const pandora::CartesianVector& pos = kdtree_type_adaptor<const pandora::Track>::position(point);
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

KDTreeTesseract fill_and_bound_4d_kd_tree(pandora::Algorithm* const  caller,
					  const std::unordered_set<const pandora::CaloHit*>& points,
					  std::vector<KDTreeNodeInfoT<const pandora::CaloHit*,4> >& nodes,
					  bool passthru) {
  std::array<float,4> minpos{ {0.0f,0.0f,0.0f, 0.0f} }, maxpos{ {0.0f,0.0f,0.0f,0.0f} };
  unsigned i = 0;
  for( const pandora::CaloHit* point : points ) {
    if ( !passthru && !PandoraContentApi::IsAvailable(*caller, point) ) continue;
    const pandora::CartesianVector& pos = kdtree_type_adaptor<const pandora::CaloHit>::position(point);
    float layer = (float)point->GetPseudoLayer();
    nodes.emplace_back(point, (float)pos.GetX(), (float)pos.GetY(), (float)pos.GetZ(), layer);
    if( i == 0 ) {
      minpos[0] = pos.GetX(); minpos[1] = pos.GetY(); minpos[2] = pos.GetZ(); minpos[3] = layer;
      maxpos[0] = pos.GetX(); maxpos[1] = pos.GetY(); maxpos[2] = pos.GetZ(); maxpos[3] = layer;
    } else {
      minpos[0] = std::min((float)pos.GetX(),minpos[0]);
      minpos[1] = std::min((float)pos.GetY(),minpos[1]);
      minpos[2] = std::min((float)pos.GetZ(),minpos[2]);
      minpos[3] = std::min(layer,minpos[3]);
      maxpos[0] = std::max((float)pos.GetX(),maxpos[0]);
      maxpos[1] = std::max((float)pos.GetY(),maxpos[1]);
      maxpos[2] = std::max((float)pos.GetZ(),maxpos[2]);
      maxpos[3] = std::max(layer,maxpos[3]);
    }
    ++i;
  }
  return KDTreeTesseract(minpos[0],maxpos[0],
			 minpos[1],maxpos[1],
			 minpos[2],maxpos[2],
			 minpos[3],maxpos[3]);
}

KDTreeCube build_3d_kd_search_region( const pandora::CaloHit* point,
				      float x_span,
				      float y_span,
				      float z_span){
  const auto& pos = point->GetPositionVector();
  
  auto x_side = minmax(pos.GetX()+x_span,pos.GetX()-x_span);
  auto y_side = minmax(pos.GetY()+y_span,pos.GetY()-y_span);
  auto z_side = minmax(pos.GetZ()+z_span,pos.GetZ()-z_span);
  
  return KDTreeCube(x_side.first,x_side.second,
		    y_side.first,y_side.second,
		    z_side.first,z_side.second);
}

KDTreeTesseract build_4d_kd_search_region( const pandora::CaloHit* point,
					   float x_span,
					   float y_span,
					   float z_span,
					   float search_layer){
  const auto& pos = point->GetPositionVector();
  auto x_side = minmax(pos.GetX()+x_span,pos.GetX()-x_span);
  auto y_side = minmax(pos.GetY()+y_span,pos.GetY()-y_span);
  auto z_side = minmax(pos.GetZ()+z_span,pos.GetZ()-z_span);
  auto layer_side = minmax(search_layer+0.5,search_layer-0.5);
  return KDTreeTesseract(x_side.first,x_side.second,
			 y_side.first,y_side.second,
			 z_side.first,z_side.second,
			 layer_side.first,layer_side.second);
}

KDTreeTesseract build_4d_kd_search_region( const pandora::CartesianVector& pos,
					   float x_span,
					   float y_span,
					   float z_span,
					   float search_layer){
  auto x_side = minmax(pos.GetX()+x_span,pos.GetX()-x_span);
  auto y_side = minmax(pos.GetY()+y_span,pos.GetY()-y_span);
  auto z_side = minmax(pos.GetZ()+z_span,pos.GetZ()-z_span);
  auto layer_side = minmax(search_layer+0.5,search_layer-0.5);
  return KDTreeTesseract(x_side.first,x_side.second,
			 y_side.first,y_side.second,
			 z_side.first,z_side.second,
			 layer_side.first,layer_side.second);
}

} //lc_content_fast
