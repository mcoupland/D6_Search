<?php

/**
 * hook_block() 
 */
function customsearch_block($op = 'list', $delta = 0, $edit = array()) {
	$block = array();
	switch($op){
		case 'list':
			$block[0]['info'] = 'Customsearch Search Form';		
			$block[1]['info'] = 'Customsearch Search Results';
			$block[2]['info'] = 'Customsearch Search Result';
			$block[3]['info'] = 'Customsearch Pagination';	
			$block[4]['info'] = 'Customsearch Results Header';
			$block[5]['info'] = 'Customsearch Utility Bar Search Form';	
			break;
		case 'view':
			$block['title'] = '';
			$block['subject'] = '';
			switch($delta){
				case 0: 
					$searchform = drupal_get_form('searchform');					
					$block['content'] = theme('customsearch_searchform', $searchform);
					break;
				case 1: 
					$block = _customsearch_get_searchresultsblock($edit);					
					break;
				case 2:
					$block['content'] = theme('customsearch_searchresult', $edit);
					break;
				case 3:
					$block['content'] = theme('customsearch_pager', $edit);
					break;
				case 4:
					$block['content'] = theme('customsearch_resultgroupheader', $edit);
					break;
				case 5: 
					$utilitybar_searchform = drupal_get_form('utilitybar_searchform');					
					$block['content'] = theme('customsearch_utilitybar_searchform', $utilitybar_searchform);
					break;
			}
			break;
	}
	return $block;
}

/**
 * hook theme 
 */
function customsearch_theme($existing, $type, $theme, $path) {
	return array(
		'customsearch_searchform' => array(
			'template' => 'customsearch_searchform',
			'arguments' => array('searchform' => NULL),
		),  
		'customsearch_searchresults' => array(
			'template' => 'customsearch_searchresults',
			'arguments' => array('searchresults' => NULL, 'pagenumber' => 1),
		),
		'customsearch_searchresult' => array(
			'template' => 'customsearch_searchresult',
			'arguments' => array('searchresult' => NULL),
		),
		'customsearch_pager' => array(
			'template' => 'customsearch_pager',
			'arguments' => array('resultdata' => NULL),
		),
		'customsearch_resultgroupheader' => array(
			'template' => 'customsearch_resultgroupheader',
			'arguments' => array('groupheader' => NULL),
		),
		'customsearch_utilitybar_searchform' => array(
			'template' => 'customsearch_utilitybar_searchform',
			'arguments' => array('searchform' => NULL),
		), 
	);
}

/**
 * implements template_preprocess
 */
function customsearch_preprocess_customsearch_pager($variables){
	if($variables['resultdata']['display'] == 'summary'){
		$variables['template_files'][] ='customsearch_pagerinfo';
	}
}

/**
 * implements template_preprocess
 */
function customsearch_preprocess_customsearch_searchresult($variables){
	$searchresult = $variables['searchresult'];
	$item = node_load(array('nid'=>$searchresult['nid'], 'status'=>1));		
	$nodecontent = strip_tags(html_entity_decode(_customsearch_get_nodecontent($item), ENT_QUOTES, 'UTF-8'));
	$path = drupal_lookup_path('alias', 'node/'.$item->nid);
	$nodelink = $path ? '/'.$path : '/node/'.$item->nid;	
	if($searchresult['ishero']){
		$imagesrc = imagecache_create_url('220-Wide', $item->field_image[0]['filepath']);
		$len = strlen($nodecontent) > 350 ? strrpos(substr($nodecontent, 0, 350), ' ') : strlen($nodecontent);
		$nodecontent = substr($nodecontent, 0, $len).'...';
		$relatedlinks = _customsearch_get_relatedlinks($searchresult['category'], $item->nid, $nodelink);
		if(strtolower($searchresult['category']) == 'properties'){
			$icons = _customsearch_get_icons($item->nid);
		}
	}
	else{
		$len = strlen($nodecontent) > 250 ? strrpos(substr($nodecontent, 0, 250), ' ') : strlen($nodecontent);
		$nodecontent = substr($nodecontent, 0, $len).'...';	
	}	
	$variables['imagesrc'] = $imagesrc;
	$variables['ishero'] = $searchresult['ishero'];
	$variables['title'] = $item->title;
	$variables['path'] = $path;
	$variables['nodelink'] = $nodelink;
	$variables['nodecontent'] = $nodecontent;
	$variables['relatedlinks'] = $relatedlinks;	
	switch(strtolower($searchresult['category'])){
		case 'destinations':
			$variables['tabs']['regions'] = $nodelink;
			$variables['tabs']['properties'] = $nodelink.'#mini-panel-region-tabs-middle-tab-3';
			$variables['tabs']['special offers'] = $nodelink.'#mini-panel-region-tabs-middle-tab-5';
			break;
		case 'properties':
			$variables['tabs']['special offers'] = $nodelink.'#mini-panel-hotels-tabs-middle-tab-4';
			break;
		default:
			break;
	}
	$variables['icons'] = $icons;
}
