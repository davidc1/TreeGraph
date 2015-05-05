/**
 * \file AlgoMultipleParents.h
 *
 * \ingroup GeoTree
 * 
 * \brief Class def header for a class geotree::AlgoMultipleParents
 *
 * @author david caratelli
 */

/** \addtogroup GeoTree
    
    @{*/
#ifndef ALGOMULTIPLEPARENTS_H
#define ALGOMULTIPLEPARENTS_H

#include "Correlation.h"
#include "AlgoBase.h"
#include <vector>
#include "string.h"

namespace geotree{

  class AlgoMultipleParents : public AlgoBase {

  public:
    
    AlgoMultipleParents() { _name="MultipleParents"; }

    void Apply(NodeID_t ID, const std::vector<::geotree::Node>& node_v);

  };
}

#endif
/** @} */ // end of doxygen group 
