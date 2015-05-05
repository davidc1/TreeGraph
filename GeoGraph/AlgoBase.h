/**
 * \file AlgoBase.h
 *
 * \ingroup GeoTree
 * 
 * \brief Class def header for a class geotree::AlgoBase
 *
 * @author david caratelli
 */

/** \addtogroup GeoTree
    
    @{*/
#ifndef ALGOBASE_H
#define ALGOBASE_H

#include "Correlation.h"
#include "Node.h"
#include <vector>
#include "string.h"

namespace geotree{

  class AlgoBase{

  public:
    
    AlgoBase() { _name=""; }

    virtual ~AlgoBase(){}

    /// Getter for the correlations erased/modified/added by algo
    virtual std::vector<geotree::Correlation> GetCorrelations() { return _corr_v; }

    /// clear correlations. To be called every time aglorithm is applied to a node
    virtual void ClearCorrelations() { _corr_v.clear(); }

    /// Function that sorts conflicts
    virtual void Apply(NodeID_t ID, const std::vector<::geotree::Node>& node_v) { return; }

  protected:
    
    // Algo's name
    std::string _name;

    // vector of correlations
    std::vector<geotree::Correlation> _corr_v;

  };
}

#endif
/** @} */ // end of doxygen group 
