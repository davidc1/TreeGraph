#ifndef NODE_CXX
#define NODE_CXX

#include "Node.h"

namespace geotree{

  void Node::addCorrelation(const NodeID_t id, const double score,
			    const ::geoalgo::Point_t& vtx,
			    const geotree::RelationType_t type){

    if (_debug){
      std::cout << "This node: " << this->ID()
		<< "\tCorrelation: " << id << "\tVtx: " << vtx << "\tScore: " << score << "\tType: " << type << std::endl;
    }
    _corrScores[id] = score;
    _corrVtx[id] = vtx;
    _corrType[id] = type;

    return;
  }


  void Node::editCorrelation(const NodeID_t id, const double score,
			     const ::geoalgo::Point_t& vtx,
			     const geotree::RelationType_t type){

    // if correlation between these nodes not found, raise exception
    // this function should be called only if correlation exists
    // and needs to be edited
    if ( _corrScores.find(id) == _corrScores.end() )
      throw ::geoalgo::GeoAlgoException("Error: editing correlation that does not exist!");      

    if (_debug){
      std::cout << "This node: " << this->ID()
		<< "\tCorrelation: " << id << "\tVtx: " << vtx << "\tScore: " << score << "\tType: " << type << std::endl;
    }
    _corrScores[id] = score;
    _corrVtx[id] = vtx;
    _corrType[id] = type;

    return;
  }

  
  void Node::editCorrelation(const NodeID_t id, const double score)
  {
    
    // if correlation between these nodes not found, raise exception
    // this function should be called only if correlation exists
    // and needs to be edited
    if ( _corrScores.find(id) == _corrScores.end() )
      throw ::geoalgo::GeoAlgoException("Error: editing correlation that does not exist!");      

    if (_debug){
      std::cout << "This node: " << this->ID()
		<< "\tCorrelation: " << id << "\t new Score: " << score << std::endl;
    }
    _corrScores[id] = score;

    return;
  }


  void Node::editCorrelation(const NodeID_t id, const ::geoalgo::Point_t& vtx)
  {
    
    // if correlation between these nodes not found, raise exception
    // this function should be called only if correlation exists
    // and needs to be edited
    if ( _corrScores.find(id) == _corrScores.end() )
      throw ::geoalgo::GeoAlgoException("Error: editing correlation that does not exist!");      

    if (_debug){
      std::cout << "This node: " << this->ID()
		<< "\tCorrelation: " << id << "\t new vertex: " << vtx << std::endl;
    }
    _corrVtx[id] = vtx;

    return;
  }


  void Node::editCorrelation(const NodeID_t id,
			     const geotree::RelationType_t type){

    // if correlation between these nodes not found, raise exception
    // this function should be called only if correlation exists
    // and needs to be edited
    if ( _corrScores.find(id) == _corrScores.end() )
      throw ::geoalgo::GeoAlgoException("Error: editing correlation that does not exist!");      

    if (_debug){
      std::cout << "This node: " << this->ID()
		<< "\tCorrelation: " << id << "\tType: " << type << std::endl;
    }
    _corrType[id] = type;

    return;
  }

  /// Erase a correlated element
  void Node::eraseCorrelation(const NodeID_t node){
    
    if (_debug){
      std::cout << "This node: " << this->ID()
		<< "\tRemoving Correlation with: " << node << std::endl;
    }

    _corrScores.erase(node);
    _corrVtx.erase(node);
    _corrType.erase(node);

    return;
  }

  double Node::getScore(NodeID_t node)
  {

    if (_corrScores.find(node) == _corrScores.end()){
      std::cout << "A score for this node does not exist!";
      return -1;
    }
    return _corrScores[node];
  }

  /// check if a node is primary (has no parent or sibling)
  bool Node::isPrimary() const
  {

    if (_debug) { std::cout << "\tchecking if node is primary..."; }
    std::map<NodeID_t, geotree::RelationType_t>::const_iterator it;
    for (it = _corrType.begin(); it != _corrType.end(); it++){
      if ( (it->second == geotree::RelationType_t::kParent) or
	   (it->second == geotree::RelationType_t::kSibling) ){
	if (_debug) { std::cout << "Not primary" << std::endl; }
	return false;
      }
    }
    
    if (_debug) { std::cout << "Primary!" << std::endl; }
    return true;
  }

  
  /// check if a node has a potential conflict (has parent & sibling)
  bool Node::hasConflict() const
  {

    int parents  = 0;
    int siblings = 0;

    std::map<NodeID_t, geotree::RelationType_t>::const_iterator it;
    for (it = _corrType.begin(); it != _corrType.end(); it++){
      if (it->second == geotree::RelationType_t::kParent){
	std::cout << "parent! ID: " << it->first << std::endl;
	parents += 1;
      }
      if (it->second == geotree::RelationType_t::kSibling)
	siblings += 1;
    }
    
    // if more than 1 parent something went wrong!
    if (parents > 1)
      throw ::geoalgo::GeoAlgoException("hasConflict: Node has more than 1 parent! something went wrong!");

    if ( (parents > 0) and (siblings > 0) )
      return true;

    return false;
  }


  /// check if node has a parent
  bool Node::hasParent() const
  {
    
    int parents = 0;

    std::map<NodeID_t, geotree::RelationType_t>::const_iterator it;
    for (it = _corrType.begin(); it != _corrType.end(); it++){
      if (it->second == geotree::RelationType_t::kParent)
	parents += 1;
    }

    if (parents > 1)
      throw ::geoalgo::GeoAlgoException("hasParent: Node has more than 1 parent! something went wrong!");

    if (parents == 1)
      return true;

    return false;
  }


  /// get parent ID
  NodeID_t Node::getParent() const
  {
    
    NodeID_t parent;
    int parents = 0;

    std::map<NodeID_t, geotree::RelationType_t>::const_iterator it;
    for (it = _corrType.begin(); it != _corrType.end(); it++){
      if (it->second == geotree::RelationType_t::kParent){
	parent = it->first;
	parents += 1;
      }
    }

    if (parents > 1)
      throw ::geoalgo::GeoAlgoException("getParent: Node has more than 1 parent! something went wrong!");

    if (parents == 0)
      throw ::geoalgo::GeoAlgoException("No parent when one expected! something went wrong!");

    return parent;
  }


  /// check if node has a parent
  bool Node::hasSiblings() const
  {
    
    int siblings = 0;

    std::map<NodeID_t, geotree::RelationType_t>::const_iterator it;
    for (it = _corrType.begin(); it != _corrType.end(); it++){
      if (it->second == geotree::RelationType_t::kSibling)
	siblings += 1;
    }

    if (siblings >= 1)
      return true;

    return false;
  }


  /// get sibling IDs
  std::vector<NodeID_t> Node::getSiblings() const
  {
    
    std::vector<NodeID_t> siblings;

    std::map<NodeID_t, geotree::RelationType_t>::const_iterator it;
    for (it = _corrType.begin(); it != _corrType.end(); it++){
      if (it->second == geotree::RelationType_t::kSibling)
	siblings.push_back(it->first);
    }

    if (siblings.size() == 0)
      throw ::geoalgo::GeoAlgoException("Trying to get siblings expecting >=1 but got 0! something went wrong!");
    
    return siblings;
  }

}

#endif
