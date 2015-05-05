#ifndef NODECOLLECTION_CXX
#define NODECOLLECTION_CXX

#include "NodeCollection.h"

namespace geotree{

  // is the node contained in the vector of nodes?
  bool NodeCollection::NodeExists(const size_t ID){

    // check that node has not been added
    for (size_t i=0; i < _nodes.size(); i++){
      if (_nodes[i].ID() == ID)
	return true;
    }
    
    return false;
  }


  // has the node been added to the Tree-structure?
  bool NodeCollection::NodeAdded(const NodeID_t ID){

    bool found = false;
    
    // check that node has not been added
    for (size_t i=0; i < _head_node_v.size(); i++){
      if (_head_node_v[i] == ID)
	return true;
      // check if this node exists in daughters
      found = IsSubNode(ID, _head_node_v[i]);
      if (found)
	return found;
    }
    
    return found;
  }


  // find node as subnode of other node
  bool NodeCollection::IsSubNode(NodeID_t search, NodeID_t top){

    bool found = false;

    for (size_t i=0; i < _nodes[_n_map[top]].childrenID().size(); i++){
      NodeID_t thisChildID = _nodes[_n_map[top]].childrenID()[i];
      if (thisChildID == search){
	found = true;
	return found;
      }
      found = IsSubNode(search,thisChildID);
      if (found)
	return found;
    }// loop over all children
    return found;
  }


  // function to print out full diagram for trees in manager
  void NodeCollection::Diagram(){
    
    for (size_t i=0; i < _head_node_v.size(); i++)
      Diagram(_head_node_v[i],0);

    return;
  }


  // Function to print tree diagram
  void NodeCollection::Diagram(NodeID_t id, int gen){

    // does this node exist?
    if (NodeExists(id) == false)
      throw ::geoalgo::GeoAlgoException("Node ID not found!");

    for (int g=0; g < gen; g++)
      std::cout << "..";
    std::cout << id << std::endl;
    // get list of children
    // this node:
    Node thisnode = _nodes[_n_map[id]];
    // vector of children ids
    auto const child_ids = thisnode.childrenID();
    for (size_t x=0; x < child_ids.size(); x++)
      Diagram(child_ids[x],gen+1);
    
    return;
  }


  void NodeCollection::AddNode(const size_t ID){

    // check that node has not been added
    if (NodeExists(ID) == true)
      throw ::geoalgo::GeoAlgoException("Error: Adding a node with ID that already exists! ID must be unique!");      

    // made it this far. no problems -> save node
    Node thisnode(ID);
    thisnode.setVerbose(_verbose);
    _nodes.push_back(thisnode);
    size_t idx = _nodes.size()-1;
    _n_map[ID] = idx;
    _IDs.push_back(ID);
    _idx_map[idx] = ID;
      
    return;
  }


  void NodeCollection::AddPrimaryNode(const size_t ID){

    _head_node_v.emplace_back(ID);
      
    return;
  }


  Node& NodeCollection::GetNode(const NodeID_t ID){

    if (NodeExists(ID) == false)
      throw ::geoalgo::GeoAlgoException("Error: Node ID does not exist!");      

    return _nodes[_n_map[ID]];
  }


  // find node ID from position in node vector
  NodeID_t NodeCollection::FindID(size_t idx){
    
    if (_idx_map.find(idx) == _idx_map.end())
      throw ::geoalgo::GeoAlgoException("Looking for an index that is out of bounds!");

    return _idx_map[idx];
  }

  
}

#endif
