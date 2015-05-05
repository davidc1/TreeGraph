/**
 * \file NodeCollection.h
 *
 * \ingroup GeoTree
 * 
 * \brief Class def header for a class geotree::NodeCollection
 *
 * @author david caratelli
 */

/** \addtogroup GeoTree
    
    @{*/
#ifndef NODECOLLECTION_H
#define NODECOLLECTION_H

#include "Node.h"
#include "GeoAlgo/GeoVector.h"

namespace geotree{


  /**
     \class geotree::Coollection
     User defined class geograph::NodeCollection
     Responsible for holding a collection
     of nodes and ensuring their uniqueness
  */
  
  class NodeCollection{

  public:

    /// Default constructor
    NodeCollection(){}

    // Default destructor
    virtual ~NodeCollection(){}

    /// Add a node to the internal vector of nodes
    void AddNode(const size_t ID);

    /// Get a node
    Node& GetNode(const NodeID_t ID);

    /// Clear collection
    void Clear() { _nodes.clear(); _n_map.clear(); }    

    /// Check if node exists in collection. Returns boolean
    bool NodeExists(const size_t ID);
    
  private:

    
    /// NodeCollection of all nodes created
    std::vector<::geotree::Node> _nodes;
    
    /// Map that goes from NodeID_t to position in node vector
    std::map<NodeID_t, size_t> _n_map;

  };
}
#endif

