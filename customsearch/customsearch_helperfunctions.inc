<?php



function _customsearch_get_parents($nid, &$parents, &$depth = 0){
	$q = "select distinct b.nid, b.title, b.type from nodehierarchy a join node b on a.parent = b.nid and b.status = 1 where a.nid = %d order by a.parent";
	$set = db_query($q, $nid);
	while($parent = db_fetch_array($set)){
		$depth--;	
		$parent['depth'] = $depth;
		$parents[] = $parent;
		_customsearch_get_parents($parent['nid'], &$parents, &$depth);
	}
}

function _customsearch_test(){	
	//get all roots
	$q = "select distinct b.nid, b.title, b.type from nodehierarchy a join node b on a.parent = b.nid and b.status = 1 where a.parent <> 0 order by a.parent limit 20";
	$set = db_query($q);
	while($root = db_fetch_array($set)){			
		//get immediate children
		$q1 = "select a.nid, b.title, b.type from nodehierarchy a join node b on a.nid = b.nid where a.parent = %d and b.status = 1 order by a.nid";
		$set1 = db_query($q1, $root['nid']);
		while($child = db_fetch_array($set1)){
			$child['depth'] = 1;
			$root['children'][$child['nid']] = $child;
			$rows[] = sprintf("(%d, '%s', '%s', %d, '%s', '%s', %d)", 
							mysql_real_escape_string($root['nid']), 
							mysql_real_escape_string($root['title']), 
							mysql_real_escape_string($root['type']), 
							mysql_real_escape_string($child['nid']), 
							mysql_real_escape_string($child['title']), 
							mysql_real_escape_string($child['type']), 1);
		}
		$roots[$root['nid']] = $root;		
	}
	//insert all parents
	$q = "insert into related_nodes
		(nid, ntitle, ntype, rid, rtitle, rtype, depth)
		values ".implode(',', $rows);
	db_query($q);
	
	//get all child nodes
	$q = "select b.nid, b.title, b.type from nodehierarchy a join node b on a.nid = b.nid and b.status = 1 where a.parent <> 0 order by a.nid";
	$set = db_query($q);
	$i = 0;
	while($child = db_fetch_array($set)){	
		$child['parents'] = array();
		_customsearch_get_parents($child['nid'], $parents);
		dpm($child);
		$i++;
		if($i > 20){ break; }
	}	
	
	$q = "select a.nid, a.title, a.type from node a 
		join nodehierarchy b on a.nid = b.parent
		where a.status = 1
		group by a.nid";
		
	$set = db_query($q);
	while($item = db_fetch_array($set)){
		$parents[] = $item;
	}
	
	$q = "";
	$set = db_query($q);
	while($item = db_fetch_array($set)){
		$nodes[] = $item['nid'];
	}
	

	$truncate_query = _customsearch_get_crontruncatequery();	
	$select_query = _customsearch_get_cronselectquery();
	$insert_query = _customsearch_get_croninsertquery();
	
	db_query($truncate_query);	
	$results = db_query($select_query,'a:1:{s:5:"value";s:0:"";}');	
	while($result = db_fetch_array($results)){				
		switch($result['type']){
			case 'regions':
				$result['category']  = 'Destinations';
				$result['category_sort_order']  = '0';
				break;
			case 'hotels':
			case 'villas':
				$result['category']  = 'Properties';
				$result['category_sort_order']  = '1';
				break;
			default:
				$result['category']  = 'Articles';
				$result['category_sort_order']  = '2';
				break;		
		}
		$result['title'] = trim(strip_tags(html_entity_decode($result['title'], ENT_QUOTES, 'UTF-8')));	
		$result['body'] = trim(strip_tags(html_entity_decode($result['body'], ENT_QUOTES, 'UTF-8')));
		$result['teaser'] = trim(strip_tags(html_entity_decode($result['teaser'], ENT_QUOTES, 'UTF-8')));
				
		$result['nodeword_content'] = unserialize($result['nodeword_content']);
		$result['nodeword_content'] = trim(strip_tags(html_entity_decode($result['nodeword_content']['value'], ENT_QUOTES, 'UTF-8')));	
		if($result['nodeword_name'] == 'keywords'){
			$result['nodeword_keywords'] = $result['nodeword_content'];
			$result['nodeword_description'] = '';
		}
		else{
			$result['nodeword_keywords'] = '';
			$result['nodeword_description'] = $result['nodeword_content'];		
		}		
		unset($result['nodeword_name']);
		unset($result['nodeword_content']);
		
		if($result['category']  = 'Articles'){
			$articlekeyword_query = _customsearch_get_cronarticlekeywordquery();
			$results = db_query($articlekeyword_query, $result['nid']);
			while($result = db_fetch_array($results)){
				var_dump($result);
			}
			die();
		}
		$insert_query = _customsearch_get_croninsertquery();		
		db_query($insert_query, $result);
	}
}

/**
 * Helper function to initiate the process of building the search results block
 */
function _customsearch_showresults($term = NULL, $pagenumber){ 
	$edit = array('term' => $term, 'pagenumber' => $pagenumber);
	$formblock = module_invoke('customsearch', 'block', 'view', 0, $edit);
	$resultsblock = module_invoke('customsearch', 'block', 'view', 1, $edit);
    
	$output = $formblock['content'];
    $output .= $resultsblock['content'];
	return $output;
}
/**
 * Helper function to get the node content
 * usually returns node body, but not always
 */
function _customsearch_get_nodecontent($item){
	if($item->type == 'hotels'){
		$query = _customsearch_get_nodecontentquery();
		$result = db_fetch_array(db_query($query, $item->nid));
		return $result['field_from_andrew_harper_value'];
	}
	return $item->body;
}

/**
 * Helper function to get all child nodes recursively
 */ 
function _customsearch_get_children($nid, &$children){
	$children[] = $nid;
	$temp = nodehierarchy_get_node_children_list($nid);
	foreach($temp as $child){
		_customsearch_get_children($child['node']->nid, $children);
	}
}

/** 
 * Helper function to get list of links to content related to a search result
 */
function _customsearch_get_relatedlinks($category = NULL, $nid = NULL, $nodelink = NULL){
	$relatedlinks = array();		
	switch(strtolower($category)){
		case 'properties':
			//special offers
			$query = _customsearch_get_relatedspecialoffersquery();
			$data = db_query($query, $nid);
			while($result = db_fetch_array($data)){
				$relatedlinks['special offers'][$result['title']] = drupal_get_path_alias('node/'.$result['nid']);
			}
		case 'destinations':
			$regionids = array();
			_customsearch_get_children($nid, $regionids);
			$inclause = mysql_real_escape_string(implode(',', $regionids));
			
			//properties
			$query = _customsearch_get_relatedpropertiesquery();			
			$data = db_query($query, $inclause);			
			while($result = db_fetch_array($data)){
				$relatedlinks['properties'][$result['title']] = drupal_get_path_alias('node/'.$result['nid']);
				$properties[] = $result['nid'];
			}
			
			//special offers
			$query = _customsearch_get_relatedspecialoffersquery();
			$pclause = mysql_real_escape_string(implode(',', $properties));
			$data = db_query($query, $pclause);
			while($result = db_fetch_array($data)){
				$relatedlinks['special offers'][$result['title']] = drupal_get_path_alias('node/'.$result['nid']);
			}
			
			//regions
			$query = _customsearch_get_relateddestinationsquery();
			$key = array_search($nid, $regionids);
			unset($regionids[$key]);
			$inclause = mysql_real_escape_string(implode(',', $regionids));
			$data = db_query($query, $inclause);			
			while($result = db_fetch_array($data)){
				$relatedlinks['regions'][$result['title']] = drupal_get_path_alias('node/'.$result['nid']);
			}			
			break;
		case 'articles':
			break;
		default:
			break;
	}
	return $relatedlinks;
}

/**
 * Helper function to get results
 */
function _customsearch_get_searchresultsblock($edit = NULL){
	$term = $edit['term'];
	if($term == NULL){
		$block['content'] = '';
	}
	else{
		$resultsperpage = 20;
		$resultdata = _customsearch_get_results($term, $resultsperpage, $edit['pagenumber']);
		$resultdata['display'] = 'summary';
		$pagerinfo = module_invoke('customsearch', 'block', 'view', 3, $resultdata);
		$output[] = $pagerinfo;
		$resultdata['display'] = 'pager';
		$pager = module_invoke('customsearch', 'block', 'view', 3, $resultdata);
		$foundhero = FALSE;
		$addedpager = FALSE;
		foreach($resultdata['results'] as $result){		
			if($resultdata['currentpage'] == 1 && !$foundhero){
				$foundhero = TRUE;
				$result['ishero'] = 1;
				$headerblock  = module_invoke('customsearch', 'block', 'view', 4, 'featured');
				$headerblock['classes'] = 'group-header';
				$output[] = $headerblock;
				$output[] = module_invoke('customsearch', 'block', 'view', 2, $result);
				if(!$addedpager){
					$addedpager = TRUE;
					if($pager['content']){ $output[] = $pager; }
				}
			}
			else{
				if(!$addedpager){
					$addedpager = TRUE;
					if($pager['content']){ $output[] = $pager; }
				}
				$result['ishero'] = 0;
				$groupedresults[$result['category']][$result['title']] = $result; 
			}
		}
		
		uasort($groupedresults, "_customsearch_category_sort");//custom function to sort the results by category		
		foreach($groupedresults as $group){	//loop through the results (categories)	
			$foundheader = FALSE;
			foreach($group as $result){ //theme each result in the category				
				if(!$foundheader){
					$headerblock  = module_invoke('customsearch', 'block', 'view', 4, $result['category']);
					$headerblock['classes'] = 'group-header';
					$output[] = $headerblock;
					$foundheader = TRUE;
				}
				$output[] = module_invoke('customsearch', 'block', 'view', 2, $result);
			}
		}
		if($pager['content']){ $output[] = $pager; }
		
		$block['content'] .= theme('customsearch_searchresults', $output);
		$block['title'] = '';
		$block['subject'] = '';
	}
	return $block;
}

/**
 * Helper function to get results count
 */
function _customsearch_get_resultscount($term){
	$query = _customsearch_get_resultscountquery($term);
	$data = db_query($query);
	$result = db_fetch_array($data);
	return $result['count'];
}

/**
 * Helper function to get icons
 */
function _customsearch_get_icons($nid){	
	$query = _customsearch_get_iconsquery();
	$result = db_fetch_array(db_query($query, $nid));
	if($result['FAV']){ 
		$icons[] = array('alt' => 'An Andrew Harper Favorite 500 Property', 
		'data' => 'An Andrew Harper Favorite 500 Property', 
		'title' => 'An Andrew Harper Favorite 500 Property', 
		'src' => '/sites/all/themes/andrew_harper/images/icons/favorite500_icon.png'
		); 
	}
	if($result['GA']){ 
		$icons[] = array('alt' => 'Grand Award Winner', 
		'data' => 'Grand Award Winner',
		'title' => 'Grand Award Winner', 
		'src' => '/sites/all/themes/andrew_harper/images/icons/grandawards_icon.png'
		); 
	}
	if($result['RC']){ 
		$icons[] = array('alt' => 'Readers\' Choice Award Winner', 
		'data' => 'Readers\' Choice Award Winner',
		'title' => 'Readers\' Choice Award Winner', 
		'src' => '/sites/all/themes/andrew_harper/images/icons/readerschoice_icon.png'
		); 
	}
	if($result['SO']){ 
		$icons[] = array('alt' => 'Special Offer', 
		'data' => 'Special Offer',
		'title' => 'Special Offer', 
		'src' => '/sites/all/themes/andrew_harper/images/icons/specialoffer_icon.png'
		); 
	}
	if($result['MB'] == "Yes"){ 
		$icons[] = array('alt' => 'Property offers member benefits', 
		'data' => 'Property offers member benefits', 
		'title' => 'Property offers member benefits', 
		'src' => '/sites/all/themes/andrew_harper/images/icons/alliancemember_icon.png'
		);
	}
	return $icons;
}

/**
 * Helper function to get results
 * $resultdata['term']
 * $resultdata['resultsperpage']
 * $resultdata['numberofresults']
 * $resultdata['numberofpages']
 * $resultdata['currentpage']
 * $resultdata['firstresultnumber']
 * $resultdata['lastresultnumber']
 * $resultdata['results']
 */
function _customsearch_get_results($term, $resultsperpage, $pagenumber){	
	$resultdata['term'] = $term;
	$resultdata['resultsperpage'] = $resultsperpage;
	$resultdata['numberofresults'] = _customsearch_get_resultscount($resultdata['term']);
	$resultdata['numberofpages'] = ceil($resultdata['numberofresults']/$resultdata['resultsperpage']);
	if($pagenumber < 1){
		$pagenumber = 1;
	}
	elseif($pagenumber > $resultdata['numberofpages']){
		$pagenumber = $resultdata['numberofpages'];
	}
	$resultdata['currentpage'] = $pagenumber;
	$resultdata['firstresultnumber'] = ($resultdata['currentpage'] - 1) * $resultdata['resultsperpage'];
	
	$query = _customsearch_get_searchquery($term);
	
	$resultdata['lastresultnumber'] = $resultdata['firstresultnumber'] - 1;//make it zero based
	$results = db_query_range($query, $resultdata['term'], $resultdata['firstresultnumber'], $resultdata['resultsperpage']);
	while($result = db_fetch_array($results)){
		$resultdata['results'][] = $result;
		$resultdata['lastresultnumber']++;
	}
	return $resultdata;
}

/**
 * Helper function to sort results by category
 */
function _customsearch_category_sort($a, $b){
	$first = reset($a);
	$second = reset($b);
	if($first['category_sort_order'] == $second['category_sort_order']){	return 0;}
	return ($first['category_sort_order'] < $second['category_sort_order']) ?  -1 : 1;
}