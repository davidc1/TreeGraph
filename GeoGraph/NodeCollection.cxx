#ifndef NODECOLLECTION_CXX
#define NODECOLLECTION_CXX

#include "NodeCollection.h"

namespace geotree{

  bool NodeCollection::NodeExists(const size_t ID){

    bool exists = false;
    
    // check that node has not been added
    for (size_t i=0; i < _nodes.size(); i++){
      if (_nodes[i].ID() == ID)
	exists = true;
    }
    
    return exists;
  }

  void NodeCollection::AddNode(const size_t ID){

    // check that node has not been added
    if (NodeExists(ID) == true)
      throw ::geoalgo::GeoAlgoException("Error: Adding a node with ID that already exists! ID must be unique!");      

    // made it this far. no problems -> save node
    Node thisnode(ID);
    _nodes.push_back(thisnode);
    _n_map[ID] = _nodes.size()-1;
      
    return;
  }


  Node& NodeCollection::GetNode(const NodeID_t ID){

    if (NodeExists(ID) == false)
      throw ::geoalgo::GeoAlgoException("Error: Node ID does not exist!");      

    return _nodes[_n_map[ID]];
  }
  
}

#endif
