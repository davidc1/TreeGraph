#ifndef MANAGER_CXX
#define MANAGER_CXX

#include "Manager.h"

namespace geotree{

  // Node initializer: create a node for each object
  void Manager::setObjects(size_t n){

    _coll.Clear();

    if (_verbose) { std::cout << "Setting " << n << " objects to prepare tree" << std::endl; }
    //_node_v.resize(_node_v.size()+n);
    for (size_t i=0; i < n; i++){
      // Assign an ID double the element number. Just because
      size_t nID = i*2;
      //std::cout << "creating node " << nID << std::endl;
      _coll.AddNode(nID);
      Node thisnode(nID);
      //std::cout << "setting debug " << std::endl;
      thisnode.setDebug(_verbose);
      //std::cout << "pushing back " << std::endl;
      _node_v.emplace_back(thisnode);
      //_node_v[i]=thisnode;
      //std::cout << "done " << std::endl;
      size_t idx = _node_v.size()-1; // index in vector for this node
      _node_map[i*2] = idx;
      _index_map[idx] = nID;
      if (_verbose) { std::cout << "Created Node Num. " << i << "/" << n <<" with ID " << _node_v.back().ID() << std::endl; }
    }
    return;
  }

  // find node ID from position in node vector
  NodeID_t Manager::FindID(size_t idx){
    
    if (_index_map.find(idx) == _index_map.end())
      throw ::geoalgo::GeoAlgoException("Looking for an index that is out of bounds!");

    return _index_map[idx];
  }

  // Function to be called when Trees will be made
  void Manager::MakeTree(){

    if (_verbose) { std::cout << "Making tree" << std::endl; }

    // loop over nodes.
    // what to do:
    // 1) if no parent or sibling -> add as head node
    // 2) if parent exists, add as child to that parent
    // 3) if sibling exists, create new parent node (example: pi0)
    for (size_t n=0; n < _node_v.size(); n++){
      if (_verbose) { std::cout << "Examining node " << n << " with ID: " << _node_v[n].ID() << std::endl; }

      // check if this node has been already added
      if (NodeAdded(_node_v[n].ID())){
	if (_verbose) { std::cout << "\tthis node has already been added. Skip" << std::endl; }
	continue;
      }

      // check if node is primary
      if (_node_v[n].isPrimary()){
	if (_verbose) { std::cout << "\tnode is primary" << std::endl; }
	_head_node_v.emplace_back(_node_v[n].ID());
      }
      // if node has a parent add it
      if (_node_v[n].hasParent()){
	if (_verbose) { std::cout << "\tnode has parent" << std::endl; }
	NodeID_t parent = _node_v[n].getParent();
	_node_v[_node_map[parent]].addChild(_node_v[n].ID());
	_node_v[_node_map[n]].setParent(parent);
      }
      // if node has a parent && a sibling
      // find vertex consistent with all 3 objects
      if (_node_v[n].hasConflict()){
	if (_verbose) { std::cout << "\tnode has conflict" << std::endl; }
	// The philosophy right now:
	// Add particle as child of its parent.
	// Do not worry about sibling relationship.
	// they are siblings since they come from the
	// same parent, but they may not share a vertex
	// worry about that later
	// So bottom line now:
	// if siblings & parent -> just add as child to parent
	continue;
      }
      // if node has sibling: make a common head node for the two siblings
      if (_node_v[n].hasSiblings()){
	if (_verbose) { std::cout << "\tnode has sibling" << std::endl; }
	if (_verbose) { std::cout << "\tadding node " << _node_v[n].ID() << " now" << std::endl; }
	// get siblings
	auto siblings = _node_v[n].getSiblings();
	if (_verbose) { std::cout << "\tnode has " << siblings.size() << " siblings" << std::endl;
	}
	// if > 1 siblings
	// Make sure all siblings share the same vertex
	if (siblings.size() > 1){
	  auto const vtx = _node_v[n].getVtx(siblings[0]);
	  for (auto &s : siblings){
	    auto const vtx2 = _node_v[n].getVtx(s);
	    if (vtx != vtx2)
	      throw ::geoalgo::GeoAlgoException("Multiple siblings @ different Vertices. Should have been solved by SortSiblings!");
	  }//for all siblings
	}// if multiple siblings
	// create new node to host the new siblings
	NodeID_t id = _node_v[n].ID()*100+siblings[0]*1000+1; 
	// Make sure this ID does not exist
	if (_node_map.find(id) != _node_map.end())
	  throw ::geoalgo::GeoAlgoException("About to create a NodeID that already exists. Not acceptable!");
	Node tmpNode(id);
	tmpNode.setDebug(_verbose);
	_node_v.push_back(tmpNode);
	_node_map[id] = _node_v.size()-1;
	// add child nodes to newly created node
	_node_v[_node_map[id]].addChild(_node_v[n].ID());
	_node_v[n].setParent(id);
	// loop over siblings and add them
	for (auto& sib : siblings){ 
	  _node_v[_node_map[id]].addChild(sib);
	  _node_v[_node_map[sib]].setParent(id);
	}
	if (_verbose) { std::cout << "\tadding node " << _node_v.back().ID() << " to _head_node_v" << std::endl; }
	_head_node_v.emplace_back(_node_v.back().ID());
	if (_verbose){ 
	  std::cout << "\tadded node " << id 
		    << " as parent of: [" << _node_v[n].ID() << ", ";
	  for (auto &sib : siblings)
	    std::cout << sib << ", ";
	  std::cout << std::endl;
	}
      }// if node has a sbinling
    }// for all nodes
    
  return;
  }

  // Function to move node from one location to another
  void Manager::MoveNode(const NodeID_t child, const NodeID_t parent){

    // do these nodes exist?
    if (_node_map.find(child) == _node_map.end())
      throw ::geoalgo::GeoAlgoException("Node ID of Node to be moved not found!");
    if (_node_map.find(parent) == _node_map.end())
      throw ::geoalgo::GeoAlgoException("Node ID of Node destination not found!");

    // Add node as child
    _node_v[_node_map[parent]].addChild(child);
    // edit parent info for child node
    _node_v[_node_map[child]].setParent(parent);

    // if moved node was in "_head_node_v" remove it
    auto const nodepos = std::find(_head_node_v.begin(),_head_node_v.end(),child);
    if (nodepos != _head_node_v.end())
      _head_node_v.erase(nodepos);

    return;
  }

  // Get a node (const)
  const Node Manager::GetNode(NodeID_t id){

    // does this node exist?
    if (_node_map.find(id) == _node_map.end())
      throw ::geoalgo::GeoAlgoException("Node ID not found!");

    return _node_v[_node_map[id]];
  }

  // Get a node (writeable)
  Node& Manager::GetWriteableNode(NodeID_t id){

    // does this node exist?
    if (_node_map.find(id) == _node_map.end())
      throw ::geoalgo::GeoAlgoException("Node ID not found!");

    return _node_v[_node_map[id]];
  }

  // function to print out full diagram for trees in manager
  void Manager::Diagram(){
    
    for (size_t i=0; i < _head_node_v.size(); i++)
      Diagram(_head_node_v[i],0);

    return;
  }


  // Function to print tree diagram
  void Manager::Diagram(NodeID_t id, int gen){

    // does this node exist?
    if (_node_map.find(id) == _node_map.end())
      throw ::geoalgo::GeoAlgoException("Node ID not found!");

    for (int g=0; g < gen; g++)
      std::cout << "..";
    std::cout << id << std::endl;
    // get list of children
    // this node:
    Node thisnode = _node_v[_node_map[id]];
    // vector of children ids
    auto const child_ids = thisnode.childrenID();
    for (size_t x=0; x < child_ids.size(); x++)
      Diagram(child_ids[x],gen+1);
    
    return;
  }


  // find if node added to _head_node_v
  bool Manager::NodeAdded(NodeID_t n){

    bool found = false;
    
    for (size_t i=0; i < _head_node_v.size(); i++){
      if (_head_node_v[i] == n)
	return true;
      // check if this node exists in daughters
      found = IsSubNode(n, _head_node_v[i]);
      if (found)
	return found;
    }
      
    return found; 
  }

  // find node as subnode of other node
  bool Manager::IsSubNode(NodeID_t search, NodeID_t top){

    bool found = false;

    for (size_t i=0; i < _node_v[_node_map[top]].childrenID().size(); i++){
      NodeID_t thisChildID = _node_v[_node_map[top]].childrenID()[i];
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
      
    
  // Correlation provided indicates relationship between node id1 and node id2
  void Manager::AddCorrelation(const NodeID_t id1, const NodeID_t id2,
			       const double score,
			       const geoalgo::Point_t& vtx,
			       const geotree::RelationType_t type){

    // make sure nodes exist
    if (_node_map.find(id1) == _node_map.end())
      throw ::geoalgo::GeoAlgoException("Node ID not found!");
    if (_node_map.find(id2) == _node_map.end())
      throw ::geoalgo::GeoAlgoException("Node ID not found!");

    if (_verbose) { std::cout << "\tAdding Correlation..." << std::endl; }
    _node_v[_node_map[id2]].addCorrelation(id1,score,vtx,type);

    //type returned is the relation of 1 w.r.t. 2
    // find "inverse" relation to assign to 2 w.r.t. 1
    geotree::RelationType_t otherRel = geotree::RelationType_t::kUnknown;
    if (type == geotree::RelationType_t::kSibling)
      otherRel = geotree::RelationType_t::kSibling;
    else if (type == geotree::RelationType_t::kChild)
      otherRel = geotree::RelationType_t::kParent;
    else if (type == geotree::RelationType_t::kParent)
      otherRel = geotree::RelationType_t::kChild;
    
    _node_v[_node_map[id1]].addCorrelation(id2,score,vtx,otherRel);

    return;
  }


  void Manager::EditCorrelation(const NodeID_t id1, const NodeID_t id2,
				const double score,
				const geoalgo::Point_t& vtx,
				const geotree::RelationType_t type){

    // make sure nodes exist
    if (_node_map.find(id1) == _node_map.end())
      throw ::geoalgo::GeoAlgoException("Node ID not found!");
    if (_node_map.find(id2) == _node_map.end())
      throw ::geoalgo::GeoAlgoException("Node ID not found!");

    if (_verbose) { std::cout << "\tEditing Correlation..." << std::endl; }
    _node_v[_node_map[id2]].editCorrelation(id1,score,vtx,type);

    //type returned is the relation of 1 w.r.t. 2
    // find "inverse" relation to assign to 2 w.r.t. 1
    geotree::RelationType_t otherRel = geotree::RelationType_t::kUnknown;
    if (type == geotree::RelationType_t::kSibling)
      otherRel = geotree::RelationType_t::kSibling;
    else if (type == geotree::RelationType_t::kChild)
      otherRel = geotree::RelationType_t::kParent;
    else if (type == geotree::RelationType_t::kParent)
      otherRel = geotree::RelationType_t::kChild;
    
    _node_v[_node_map[id1]].editCorrelation(id2,score,vtx,otherRel);

    return;
  }


  void Manager::EditCorrelation(const NodeID_t id1, const NodeID_t id2,
				const double score){

    // make sure nodes exist
    if (_node_map.find(id1) == _node_map.end())
      throw ::geoalgo::GeoAlgoException("Node ID not found!");
    if (_node_map.find(id2) == _node_map.end())
      throw ::geoalgo::GeoAlgoException("Node ID not found!");

    if (_verbose) { std::cout << "\tEditing Correlation Score..." << std::endl; }
    _node_v[_node_map[id2]].editCorrelation(id1,score);
    _node_v[_node_map[id1]].editCorrelation(id2,score);

    return;
  }


  void Manager::EditCorrelation(const NodeID_t id1, const NodeID_t id2,
				const geoalgo::Point_t& vtx){

    // make sure nodes exist
    if (_node_map.find(id1) == _node_map.end())
      throw ::geoalgo::GeoAlgoException("Node ID not found!");
    if (_node_map.find(id2) == _node_map.end())
      throw ::geoalgo::GeoAlgoException("Node ID not found!");

    if (_verbose) { std::cout << "\tEditing Correlation Vtx..." << std::endl; }
    _node_v[_node_map[id2]].editCorrelation(id1,vtx);
    _node_v[_node_map[id1]].editCorrelation(id2,vtx);

    return;
  }


  void Manager::EditCorrelation(const NodeID_t id1, const NodeID_t id2,
				const geotree::RelationType_t type){

    // make sure nodes exist
    if (_node_map.find(id1) == _node_map.end())
      throw ::geoalgo::GeoAlgoException("Node ID not found!");
    if (_node_map.find(id2) == _node_map.end())
      throw ::geoalgo::GeoAlgoException("Node ID not found!");

    if (_verbose) { std::cout << "\tEditing Correlation Relation..." << std::endl; }
    _node_v[_node_map[id2]].editCorrelation(id1,type);

    //type returned is the relation of 1 w.r.t. 2
    // find "inverse" relation to assign to 2 w.r.t. 1
    geotree::RelationType_t otherRel = geotree::RelationType_t::kUnknown;
    if (type == geotree::RelationType_t::kSibling)
      otherRel = geotree::RelationType_t::kSibling;
    else if (type == geotree::RelationType_t::kChild)
      otherRel = geotree::RelationType_t::kParent;
    else if (type == geotree::RelationType_t::kParent)
      otherRel = geotree::RelationType_t::kChild;
    
    _node_v[_node_map[id1]].editCorrelation(id2,otherRel);

    return;
  }


  /// Erase a correlation completely
  void Manager::EraseCorrelation(const NodeID_t id1, const NodeID_t id2){

    // make sure nodes exist
    if (_node_map.find(id1) == _node_map.end())
      throw ::geoalgo::GeoAlgoException("Node ID not found!");
    if (_node_map.find(id2) == _node_map.end())
      throw ::geoalgo::GeoAlgoException("Node ID not found!");

    if (_verbose) { std::cout << "\tRemoving Correlation..." << std::endl; }
    _node_v[_node_map[id1]].eraseCorrelation(id2);
    _node_v[_node_map[id2]].eraseCorrelation(id1);

    return;
  }

  void Manager::ResolveConflicts(){

    // what is a conflict?
    // 1) two parents
    // 2) a sibling with different parent
    // 3) multiple siblings
    // 4) has both siblings & parent

    // solutions:
    // 1) choose parent with best score
    
    // NOTE: THIS IS NOT IMPLEMENTED
    // 2) compare score of possible scenarios:
    // example: A -> B and C -> D 
    // && B & D siblings
    // score 1: (A -> B) + (C -> D)
    // score 2: (A -> B) + (B && D)
    // score 3: (C -> D) + (B && D)
    // choose whichever is highest
    
    // first resolve conflict 1)
    // if multiple parents, choose the
    // one with the highest score
    if (_verbose) { std::cout << "Find Best parent of nodes if they have multiple ones..." << std::endl; }
    FindBestParent();

    // Case in which parent and sibling are siblings
    if (_verbose) { std::cout << "Make sure parent and sibling are logically consistent if they exist" << std::endl; }
    ParentAndSiblingAreNotLogicallyConsistent();

    // if there is a conflict, remove sibling relation
    if (_verbose) { std::cout << "If there is a conflict, remove sibling status" << std::endl; }
    IfConflictRemoveSibling();

    /*
    // case in which sibling does not have a parent if this ID does
    if (_verbose) { std::cout << "Make sure sibling has a parent if we do" << std::endl; }
    SiblingDoesNotHaveSameParent();
    */

    // Conflict 3)
    // Resolve conflict of multiple siblings
    if (_verbose) { std::cout << "merge or find best sibling if there are multiple siblings..." << std::endl; }
    SortSiblings();

    /*
    // Conflict 4)
    // Resolve potential conflict between siblings & parent
    if (_verbose) { std::cout << "if parent and siblings resolve conflict..." << std::endl; }
    SortSiblingsAndParent();
    */
    return;
  }


  /// Find best parent for each Node
  void Manager::FindBestParent(){

    std::map<NodeID_t,size_t>::iterator it;

    for (it = _node_map.begin(); it != _node_map.end(); it++)
      FindBestParent(it->first);

    return;
  }

  /// function to find best parent of a node (remove other parents)
  void Manager::FindBestParent(NodeID_t ID){

    if (_verbose) { std::cout << "look for best parent for node: " << ID << std::endl; }

    // iterator for correlation types
    std::map<NodeID_t,geotree::Correlation>::const_iterator it;
    // get correaltions for this node
    auto const corrs  = _node_v[_node_map[ID]].getCorrelations();
    //auto const cTypes = _node_v[_node_map[ID]].getCorrType();   
    // get correlation scores for this node
    //auto const cScores = _node_v[_node_map[ID]].getCorrScores();

    // vector where to hold parent IDs
    std::vector<size_t> parentIDs;
    // vector where to hold parent scores
    std::vector<double> parentScores;

    for (it = corrs.begin(); it != corrs.end(); it++){
      if ((it->second).Relation() == geotree::RelationType_t::kParent){
	//if (it->second == geotree::RelationType_t::kChild){
	size_t pID = it->first;
	parentIDs.push_back(pID);
	parentScores.push_back(_node_v[_node_map[ID]].getScore(pID));
      }
    }

    if (_verbose) { std::cout << "\tfound " << parentIDs.size() << " parents" << std::endl; }

    // if multiple parents erase all but the one with the highest score
    if (parentIDs.size() > 1){
      NodeID_t bestParent = 0;
      double bestScore = 0;
      for (size_t i=0; i < parentScores.size(); i++){
	if (_verbose) { std::cout << "\t\tParent ID: " << parentIDs[i] << "\tScore: " << parentScores[i] << std::endl; }
	if (parentScores[i] > bestScore){
	  bestScore = parentScores[i];
	  bestParent = parentIDs[i];
	}
      }
      // best parent is that at position i
      for (auto const& pID : parentIDs){
	if (pID != bestParent){
	  if (_verbose) { std::cout << "\t\terasing corr. between " << pID << " and " << ID << std::endl; }
	  // not the best -> eliminate!
	  _node_v[_node_map[ID]].eraseCorrelation(pID);
	  // do the same backwards! (eliminate reverse corr.)
	  _node_v[_node_map[pID]].eraseCorrelation(ID);
	}
      }

    }// if more than 1 parents
    
    return;
  }

  // Sort all siblings at once
  void Manager::SortSiblings(){
   
    std::map<NodeID_t,size_t>::iterator it;

    for (it = _node_map.begin(); it != _node_map.end(); it++)
      SortSiblings(it->first);
 
    return;
  }
  
  // Siblings sorting:
  // Either merge siblings (loose == true)
  // or only pick the best one (loose == false)
  // After decision, modify all other correlations
  // accordingly
  void Manager::SortSiblings(NodeID_t ID){

    if (_verbose) { std::cout << "sort siblings for node: " << ID << std::endl; }

    if (_node_v[_node_map[ID]].hasSiblings() == false){
      if (_verbose) { std::cout << "\tno siblings. No issue..." << std::endl; }
      return;
    }

    auto siblings = _node_v[_node_map[ID]].getSiblings();
    if (siblings.size() == 1){
      if (_verbose) { std::cout << "\tOnly 1 sibling. No issue..." << std::endl; }
      return;
    }

    // If there are multiple siblings but with the same vertex
    // -> then we are good to go. Everything is in agreement
    bool AllSame = true;
    auto const vtx1 = _node_v[_node_map[ID]].getVtx(siblings[0]);
    for (size_t i=1; i < siblings.size(); i++){
      auto const vtx2 = _node_v[_node_map[ID]].getVtx(siblings[i]);
      if (vtx1 != vtx2){
	AllSame = false;
	break;
      }
    }// for all remaining siblings
    // if all vertices are the same -> return
    if (AllSame){
      // Just make sure siblings are correlated amongst themselves
      for (size_t s1 = 0; s1 < siblings.size(); s1++){
	for (size_t s2 = s1+1; s2 < siblings.size(); s2++){
	  // are they correlated?
	  if (_loose){
	    if ( _node_v[_node_map[siblings[s1]]].isCorrelated(siblings[s2]) == true){
	      // remove that correlation and replace it with a sibling correlation
	      EraseCorrelation(siblings[s1],siblings[s2]);
	    }
	    if (_verbose) { std::cout << "\tAbout to add sibling correlation..." << std::endl; }
	    AddCorrelation(siblings[s1],siblings[s2],0.,vtx1,::geotree::RelationType_t::kSibling);
	  }
	}
      }
      return;
    }
    
    // if a particle has multiple siblings we can either:
    // - merge those siblings together into a single vtx
    // - find the best sibling
    if (_loose){
      // find all vertices of siblings
      std::vector<::geoalgo::Vector_t> siblingVtxList;
      // siblings contains NodeID of all siblings. Use to get vtx
      for (auto& sID : siblings)
	siblingVtxList.push_back(_node_v[_node_map[ID]].getVtx(sID));
      // find "average" vertex location
      if (_verbose) { 
	std::cout << "\tFind Bounding Sphere from points: " << std::endl;
	for (size_t v=0; v < siblingVtxList.size(); v++)
	  std::cout << "\tSib: " << siblings[v] << "\tVtx: "<< siblingVtxList[v] << std::endl;
      }
      ::geoalgo::Point_t newVtx = _geoAlgo.boundingSphere(siblingVtxList).Center();
      if (_verbose) { std::cout << "\taverage vtx from " << siblings.size() << " siblings is: " << newVtx << std::endl; }
      // edit all correlations so that vertices are updated.
      for (auto& sID : siblings)
	EditCorrelation(ID,sID,newVtx);
      // also, we need to add all sibling correlations from node ID to its sisters
      for (size_t s1 = 0; s1 < siblings.size()-1; s1++){
	for (size_t s2 = s1+1; s2 < siblings.size(); s2++){
	  // if this correlation already exits -> just edit the vertex info
	  if (_node_v[_node_map[siblings[s1]]].isCorrelated(siblings[s2])){
	    // if correlation is not sibling then throw exception!
	    if (_node_v[_node_map[siblings[s1]]].getRelation(siblings[s2]) != ::geotree::RelationType_t::kSibling)
	      throw ::geoalgo::GeoAlgoException("About to edit what you think is sibling relation but is not!");
	    EditCorrelation(siblings[s1],siblings[s2],newVtx);
	  }// if the two siblings are already correlated
	  else{
	    // if not
	    AddCorrelation(siblings[s1],siblings[s2],0.,newVtx,::geotree::RelationType_t::kSibling);
	  }
	}
      }
    }// if loose (accept more siblings and smear vertex)
    // if instead one should just select the best vertex
    else{
      double maxScore = 0.;
      NodeID_t bestSibling = -1;
      for (auto& sID : siblings){
	double score = _node_v[_node_map[ID]].getScore(sID);
	if (score > maxScore){
	  maxScore = score;
	  bestSibling = sID;
	}
      }// for all siblings
      if (_verbose) { std::cout << "\tBest Correlation with Node " << bestSibling << " (score = " << maxScore << ")" << std::endl; }
      // now erase correlation with all other siblings
      for (auto& sID : siblings){
	if (sID != bestSibling)
	  EraseCorrelation(ID,sID);
      }
    }// if we should just keep the best correlation
    
    return;
  }

  /*
  // Sort all SiblingsAndParent
  void Manager::SortSiblingsAndParent(){

    std::map<NodeID_t,size_t>::iterator it;

    // First resolve siblings with different parents
    for (it = _node_map.begin(); it != _node_map.end(); it++)
      ResolveSiblingsWithDifferentParent(it->first);

    // now find best vtx to describe both siblings and parent
    for (it = _node_map.begin(); it != _node_map.end(); it++)
      SortSiblingsAndParent(it->first);
 
    return;
  }


  // Sort the fact that a node has siblings and a parent
  // either accept both parent and sibling (editing vertex) (loose == true)
  // or pick the one with the highest score (loose == false)
  // either way, modify all other correlations accordingly
  // Call this function if:
  // 1) already checked that there is only 1 parent
  // 2) already sorted siblings of this node
  void Manager::SortSiblingsAndParent(NodeID_t ID){

    if (_verbose) { std::cout << "sort sibling & parent for node: " << ID << std::endl; }

    // Establish that there is indeed a conflict (sibling & parent)
    // if none -> return
    if (_node_v[_node_map[ID]].hasConflict() == false){
      if (_verbose) { std::cout << "No conflict to solve! continue..." << std::endl; }
      return;
    }

    // do the siblings have the same parent?
    // get the parent NodeID
    auto const parentID = _node_v[_node_map[ID]].getParent();
    // Now loop through siblings. Do they have the same parent?
    bool sameParent = true;
    auto const siblings  = _node_v[_node_map[ID]].getSiblings();
    for (auto& s : siblings){
      if (_node_v[_node_map[s]].hasParent() == false){
	sameParent = false;
	break;
      }// if the node does not have a parent
      else if (_node_v[_node_map[s]].getParent() != parentID){
	sameParent = false;
	break;
      }// if the node has a different parent
    }// for all siblings
    if (sameParent){
      if (_verbose) { std::cout << "\tSiblings all have the same parent! yay!" << std::endl; }
      return;
    }
    else {
      if (_verbose) { std::cout << "\tSiblings don't all have the same parent! fix..." << std::endl; }
      // Need to find the best combination
      // Assume no sibling has another parent
      // Also, assume siblings have been sorted already (so all have the same communal siblings)
      // if LOOSE  : assign parent as parent of all others
      // otherwise : which has a larger score? parent or sibling? Choose the largest score
      if (_loose){
	// Assign parent as parent to all others (with score of parent)
	auto parentScore = _node_v[_node_map[ID]].getScore(parentID);
	// vertex is average of vtx for parent and vertices for siblings
	auto parentVtx = _node_v[_node_map[ID]].getVtx(parentID);
	std::vector<::geoalgo::Vector_t> vtxList;
	vtxList.push_back(parentVtx);
	for (auto& sID : siblings)
	  vtxList.push_back(_node_v[_node_map[ID]].getVtx(sID));
	// find "average" vertex location
	if (_verbose) { 
	  std::cout << "\tFind Bounding Sphere from points: " << std::endl;
	  for (size_t v=0; v < vtxList.size(); v++){
	    if (v==0)
	      std::cout << "\tParent ID : " << parentID << "\tVtx: "<< parentVtx << std::endl;
	    else
	      std::cout << "\tSibling ID: " << parentID << "\tVtx: "<< vtxList[v] << std::endl;
	  }
	}
	::geoalgo::Point_t newVtx = _geoAlgo.boundingSphere(vtxList).Center();
	if (_verbose) { std::cout << "\taverage vtx from " << siblings.size() << " siblings is: " << newVtx << std::endl; }
	// Edit all sibling & parent correlations to match the new vertex information
	if (_verbose) { std::cout << "\tEditing Corr Vtx between this ID " << ID << " and Parent " << parentID << std::endl; }
	EditCorrelation(ID,parentID,newVtx);
	for (size_t s1 = 0; s1 < siblings.size(); s1++){
	  if (_verbose) { std::cout << "\tEditing Corr Vtx between this ID " << ID << "and sibling " << siblings[s1] << std::endl; }
	  EditCorrelation(ID,siblings[s1],newVtx);
	  // Also, edit the correlations amongst the various siblings -> STUPID!!!
	  for (size_t s2 = s1+1; s2 < siblings.size(); s2++){
	    if (s2 != ID){
	      if (_verbose) { std::cout << "\tEditing Corr Vtx between siblings " << siblings[s1] << " and " << siblings[s2] << std::endl; }
	      EditCorrelation(siblings[s1],siblings[s2],newVtx);
	    }
	  }
	  // Finally, if a parent exists edit vtx
	  // if it does not exist, add parent
	  if (_node_v[_node_map[siblings[s1]]].hasParent() == true){
	    // ASSUME SAME PARENT!!!
	    if (_verbose) { std::cout << "\tEditing Corr Vtx between Sibling " << siblings[s1] << " & Parent " << parentID << std::endl; }
	    EditCorrelation(siblings[s1],parentID,newVtx);
	  }
	  else{
	    if (_verbose) { std::cout << "\tAdding Corr Vtx between Sibling " << siblings[s1] << " & Parent " << parentID << std::endl; }
	    AddCorrelation(siblings[s1],parentID,parentScore,newVtx,::geotree::RelationType_t::kChild);
	  }
	}// for all siblings
      }// if loose
      // if not loose -> find best relation for this node
      else{
	// get parent score
	auto parentScore  = _node_v[_node_map[ID]].getScore(parentID);
	// get score for first sibling (assuming sibling sorting already happened
	auto siblingScore = _node_v[_node_map[ID]].getScore(siblings[0]);
	if (_verbose) { std::cout << "\tNot loose. Parent Score: " << parentScore << " and sibling score: " << siblingScore << std::endl; }
	if (parentScore > siblingScore){
	  // remove sibling score
	  if (_verbose) { std::cout << "\tErase sibling correlation" << std::endl; }
	  // for all siblings
	  for (auto& s : siblings)
	    EraseCorrelation(ID,s);
	}// if parent is better
	// if sibling is better
	else{
	  // remove parent score
	  if (_verbose) { std::cout << "\tErase parent correlation" << std::endl; }
	  EraseCorrelation(ID,parentID);
	}
      }// if not loose
    }// if not everything is ok and we need to resolve conflicts
    return;
  }
  */

  // Resolve conflict with a sibling having a different parent
  // if two siblings have different parent we need to choose 
  // the "best" configuration, otherwise we will have a logical
  // conflict in the tree
  // if the node has many siblings, remove sibling relationship.....
  void Manager::ResolveSiblingsWithDifferentParent(NodeID_t ID){

    if (_verbose) { std::cout << "ResolveSiblingsWithDifferentParent for node " << ID << std::endl; }
    
    // Establish that there is indeed a conflict (sibling & parent)
    // if none -> return
    if (_node_v[_node_map[ID]].hasConflict() == false){
      if (_verbose) { std::cout << "\tNo parent = no conflict to solve! continue..." << std::endl; }
      return;
    }
    
    // get parent
    auto const parentID = _node_v[_node_map[ID]].getParent();

    // If a sibling does not have a parent that is fine. This will be solved
    // later in SortSiblingsAndParent
    auto const siblings = _node_v[_node_map[ID]].getSiblings();    

    for (auto& s : siblings){
      if (_node_v[_node_map[s]].hasParent() == false)
	continue;
      auto sibParentID = _node_v[_node_map[s]].getParent();
      // if this parent ID is the same as the one for the node
      // under consideration -> fine, proceed
      if (parentID == sibParentID){
	if (_verbose) { std::cout << "\tParents: " << parentID << " and " << sibParentID << ". Same parent = no conflict to solve! continue..." << std::endl; }
	continue;
      }
      if (_verbose) { std::cout << "\tSibling and this node have differnt parents! Need to resolve the conlfict!" << std::endl; }
      
      // if multiple siblings -> remove sibling relation
      if (siblings.size() > 1){
	if (_verbose) { std::cout << "\tMany siblings: removing sibling relation because easiest now!" << std::endl; }
	EraseCorrelation(ID,s);
	continue;
      }

      // if we made it to here then we have a problem!
      // the sibling has a different parent. Fix this!
      // situation is like this:
      // (2) is the node we are examining, with ID = ID 
      // (1) parent  of (2)
      // (3) parent  of (4)
      // (2) sibling of (4)
      // compare scores:
      // (1,2) + (2,4) -> break parentage between (3) and (4)      -> case A)
      // (3,4) + (2,4) -> break parentage between (1) and (2)      -> case B)
      // (1,2) + (3,4) -> break sibling state between (2) and (4)  -> case C)
      // sibling score
      double sibScore       = _node_v[_node_map[ID]].getScore(s);
      double parentScore    = _node_v[_node_map[ID]].getScore(parentID);
      double sibParentScore = _node_v[_node_map[s]].getScore(sibParentID);

      //A)
      double A = parentScore + sibScore;
      double B = sibParentScore + sibScore;
      double C = parentScore + sibParentScore;

      if (_verbose){
	std::cout << "\t\tA) : (ID,Sibling) + (ID,parent)         = " << A << std::endl;
	std::cout << "\t\tB) : (ID,Sibling) + (sibling,sibParent) = " << B << std::endl;
	std::cout << "\t\tC) : (ID,parent)  + (sibling,sibParent) = " << C << std::endl;
      }

      if ( (A > B) and (A > C) ){
	// remove sibling's parentage correlation
	if (_verbose) { std::cout << "\tChoosing A" << std::endl; }
	EraseCorrelation(s,sibParentID);
      }
      else if (B > C){
	// remove this node's parent correlation
	if (_verbose) { std::cout << "\tChoosing B" << std::endl; }
	EraseCorrelation(ID,parentID);
      }
      else{
	// remove sibling correlation
	if (_verbose) { std::cout << "\tChoosing C" << std::endl; }
	EraseCorrelation(ID,s);
      }
    }// for all siblings

    return;
  }
	
  void Manager::ParentAndSiblingAreNotLogicallyConsistent(){

    std::map<NodeID_t,size_t>::iterator it;

    // First make sure sibling and parent's relation is not logically forbidden
    for (it = _node_map.begin(); it != _node_map.end(); it++)
      ParentAndSiblingAreNotLogicallyConsistent(it->first);

    return;
  }

  void Manager::ParentAndSiblingAreNotLogicallyConsistent(NodeID_t ID){

    if (_verbose) { std::cout << "Figuring out if node " << ID << " has parent-sibling conflict" << std::endl; }

    // if node has parent and sibling
    // make sure sibling is not sibling with parent
    if (_node_v[_node_map[ID]].hasConflict() == false)
      return;

    // get siblings
    auto const siblings = _node_v[_node_map[ID]].getSiblings();
    // get parent
    auto const parentID = _node_v[_node_map[ID]].getParent();
    
    for (auto& s : siblings){
      // check if sibling is related to parent.
      // if their relation is not that of parent-child
      // need to fix things
      if (_node_v[_node_map[s]].isCorrelated(parentID) == false)
	continue;
      // ok they are correlated. what is the correlation type
      auto rel = _node_v[_node_map[s]].getRelation(parentID);
      // if this relation is not parentID is parent of s we have a problem
      if ( rel == ::geotree::RelationType_t::kParent )
	continue;

      if (_verbose) { std::cout << "\tsibling " << s << " and parent "
			      << parentID <<  " relation is not logically consistent" << std::endl; }
      // we made it this far -> we have a problem!
      // 3 nodes: ID, s, parentID
      // scores to compare:
      // A) (ID,parentID) + (ID,s)       -> this node is child of parent and sibling of s
      // B) (ID,parentID) + (parentID,s) -> this node is child of parent and sibling is related to parent, not to this node
      // whichever is larger wins
      // basically compare (ID,s) and (parentID,s)
      double A = _node_v[_node_map[ID]].getScore(s);
      double B = _node_v[_node_map[parentID]].getScore(s);
      if (A > B){
	if (_verbose) { std::cout << "keep sibling. Remove relation between parent and sibling" << std::endl; }
	EraseCorrelation(parentID,s);
      }
      else{
	if (_verbose) { std::cout << "keep parent. Remove relation with sibling" << std::endl; }
	EraseCorrelation(ID,s);
      }
    }// for all siblings

    return;
  }

  /*
  void Manager::SiblingDoesNotHaveSameParent(){

    std::map<NodeID_t,size_t>::iterator it;

    // First make sure sibling and parent's relation is not logically forbidden
    for (it = _node_map.begin(); it != _node_map.end(); it++)
      SiblingDoesNotHaveSameParent(it->first);

    return;
  }


  /// If sibling does not have the same parent -> figure out which score is largest
  void Manager::SiblingDoesNotHaveSameParent(NodeID_t ID){

    // if node has parent and sibling
    // do something if sibling does not have a parent
    if (_node_v[_node_map[ID]].hasConflict() == false)
      return;

    if (_verbose) { std::cout << "Making sure node " << ID 
			      << "'s siblings also have a parent" << std::endl; }
    
    // get siblings
    auto const siblings = _node_v[_node_map[ID]].getSiblings();
    // get parent
    auto const parentID = _node_v[_node_map[ID]].getParent();    

    for (auto& s : siblings){

      // does the sibling have a parent? 
      if ( _node_v[_node_map[s]].hasParent() == true ){
	//true...ok...either same (good) or not.
	// if not the same this will be resolved later by
	// ResolveSiblingsWithDifferentParent
	continue;
      }
      // ok...sibling does not have a parent.
      // compare score with sibling and that with parent
      // eliminate correlation with lowest score
      double parentScore  = _node_v[_node_map[ID]].getScore(parentID);
      double siblingScore = _node_v[_node_map[ID]].getScore(s);

      if (parentScore > siblingScore){
	if (_verbose) { std::cout << "Sibling does not have a parent. Removing sibling with lower score" << std::endl; }
	EraseCorrelation(ID,s);
      }
      else{
	if (_verbose) { std::cout << "Sibling does not have a parent. Removing parent with lower score" << std::endl; }
	EraseCorrelation(ID,parentID);
      }

    }// for all siblings

    return;
  }
  */

  void Manager::IfConflictRemoveSibling(){

    std::map<NodeID_t,size_t>::iterator it;

    for (it = _node_map.begin(); it != _node_map.end(); it++)
      IfConflictRemoveSibling(it->first);
    
    return;
  }

  // If there is a conflict and siblings don't have the same parent -> remove sibling relation
  void Manager::IfConflictRemoveSibling(NodeID_t ID){

    // if node has parent and sibling
    // do something if sibling does not have a parent
    if (_node_v[_node_map[ID]].hasConflict() == false)
      return;    

    if (_verbose) { std::cout << "Node has conflict...if siblings do not agree resolve" << std::endl; } 

    // get siblings
    auto const siblings = _node_v[_node_map[ID]].getSiblings();
    // get parent
    auto const parentID = _node_v[_node_map[ID]].getParent();    

    for (auto& s : siblings){
      // if the sibling does not have the same parent -> remove sibling relation
      if (_node_v[_node_map[s]].hasParent() == false){
	if (_verbose) { std::cout << "\tsibling does not have the same parent. Remove sibling realtion " << std::endl; } 
	EraseCorrelation(ID,s);
	continue;
      }
      if (_node_v[_node_map[s]].getParent() != parentID){
	if (_verbose) { std::cout << "\tsibling parent different from this node's parent. Remove sibling realtion " << std::endl; } 
	EraseCorrelation(ID,s);
	continue;
      }
    }// for all siblings 

    return;
  }
  
}

#endif
