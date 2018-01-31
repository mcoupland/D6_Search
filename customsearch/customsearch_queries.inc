<?php


/**
 * Private function to get the search query
 * db_query doesn't work with this query when sprintf type syntax is used
 */
function _customsearch_get_searchquery($term){
	$term = mysql_real_escape_string($term);
	return "select 
			nid
			, title
			, titlematch 
			, titlestarts 
			, titlecontains  
			, matchphrasetitle 
			, matchpartialtitle
			, matchpartialbody 
				, (titlematch + titlestarts + titlecontains + matchphrasetitle + matchpartialtitle + matchpartialbody) as score
				, type
				, category
				, category_sort_order
			, body
			from (    
				select 
				nid
				, title
				, type
				, category
				, category_sort_order
				, if(title = '$term', 3000 + -(category_sort_order), 0) as titlematch
				, if(locate('$term', a.title) = 1, 3000, 0) as titlestarts
				, if(locate('$term', a.title) > 1, 2000 - (locate('$term', a.title)), 0) as titlecontains
				, if(match(a.title) against('+$term*' in boolean mode) > 0, 1500 + match(a.title) against('+$term*' in boolean mode), 0) as matchphrasetitle
				, if(match(a.title) against('$term') > 1, 1250 + match(a.title) against('$term'), 0) as matchpartialtitle
				, if(match(a.body) against('$term') > 4, 1000 + match(a.body) against('$term'), 0) as matchpartialbody
				, body
				from customsearch_data a
			) a 
			group by nid
			having score > 0
			order by score desc";
}

/**
 * Private function to get the node content query
 */
function _customsearch_get_nodecontentquery(){
	return "select a.field_from_andrew_harper_value
				from content_type_hotels hotel
				left outer join content_field_from_andrew_harper a on a.vid = hotel.vid
				where hotel.nid = %d
				order by hotel.vid desc limit 1";
}

/**
 * Private function to get the resultscount query
 * db_query doesn't work with this query when sprintf type syntax is used
 */
function _customsearch_get_resultscountquery($term){
	$term = mysql_real_escape_string($term);
	return "select 
		count(nid) as 'count'
		from customsearch_data a
		where title = '$term'
		or locate('$term', a.title) >= 1
		or match(a.title) against('$term' in boolean mode) > 0
		or match(a.title) against('$term') > 1
		or match(a.body) against('$term') > 4";
		
	return "select 
		count(nid) as 'count'
		from customsearch_data a
		where title = '$term'
		or locate('$term', a.title) >= 1
		or match(a.title) against('\+$term\*' in boolean mode) > 0
		or match(a.title) against('$term') > 1
		or match(a.body) against('$term') > 4";
}

/**
 * Private function to get the icons query
 */
function _customsearch_get_iconsquery(){
	return "select 
			a.field_alliance_hotel_value 'MB'
			, b.field_amenities_nid as 'GA'
			, c.field_ratings_rating as 'FAV'
			, d.field_amenities_nid as 'RC'
			, e.nid as 'SO'
			from content_type_hotels a
			left outer join content_field_amenities b on b.vid = a.vid and b.field_amenities_nid in (select distinct nid from node where type = 'amenities' and title like 'grand award winners%' and status = 1)
			left outer join content_field_ratings c on c.vid = a.vid
			left outer join content_field_amenities d on d.vid = a.vid and d.field_amenities_nid in (select distinct nid from node where type = 'amenities' and title like 'readers\' choice%' and status = 1) 
			left outer join content_field_hotels e on e.field_hotels_nid = a.nid and e.nid in (select distinct nid from node where type = 'special_offers' and status = 1)
			where a.nid = %d 
			order by a.vid desc limit 1";
}

/**
 * Private function to get the cron insert query
 */
function _customsearch_get_croninsertquery0(){
	return "insert into customsearch_data (
		select 
		  a.nid   
		  , (
				case a.type
					when 'regions' then 'Destinations' 
					when 'hotels' then 'Properties'
					when 'villas' then 'Properties'
					else 'Articles'
					end
			) as category
    , (
        case a.type
        when 'regions' then 0 
        when 'hotels' then 1
        when 'villas' then 1
        else 2
        end
    ) as category_sort_order
		, b.title
		, b.body
		, b.teaser
		, a.type    
		from node a
		left outer join node_revisions b on a.vid = b.vid
		where a.status = 1 
		and a.type in ('regions', 'hotels', 'villas', 'articles', 'blog_post', 'hideaway_report', 'landing_page', 'static_pages', 'static_pages_nav', 'wp_blog_posts')  
	)";
}

/**
 * Private function to get the cron insert query
 */
function _customsearch_get_croninsertquery(){
	return "insert into customsearch_data (nid, category, category_sort_order, title, body, teaser, type, nodeword_keywords, nodeword_description) 
			values (%d,'%s', %d, '%s', '%s', '%s', '%s', '%s', '%s')";
}

/**
 * Private function to get the cron select query
 */
function _customsearch_get_cronselectquery(){
	return "select 
			a.nid   
			, '' as category
			, '' as category_sort_order
			, b.title
			, b.body
			, b.teaser
			, a.type    
			, c.name as nodeword_name
			, c.content as nodeword_content
			from node a
			left outer join node_revisions b on a.vid = b.vid
			left outer join nodewords c on a.nid = c.mtid and c.name in ('description', 'keywords')	and c.content <> '%s'
			where a.status = 1 
			and a.type in ('regions', 'hotels', 'villas', 'articles', 'blog_post', 'hideaway_report', 'landing_page', 'static_pages', 'static_pages_nav', 'wp_blog_posts')
			group by a.nid
			order by a.vid desc";
}

/**
 * Private function to get the cron  query
 */
function _customsearch_get_cronarticlekeywordquery(){
	return "select *
			from term_data a
			join term_node b on a.tid = b.tid
			join node c on b.nid = c.nid and c.status = 1
			where b.nid = %d
			group by a.tid
			order by c.nid desc";
}

/**
 * Private function to get the cron keyword insert query
 */
function _customsearch_get_cronkeywordselectquery(){
	return "select 
			id as nid
			, 'Keywords' as category
			, '3' as category_sort_order
			,  (
					case name
						when 'keywords' then content  
						else ''
						end
				) as 'title'
			,  (
					case name
						when 'description' then content  
						else ''
						end
				) as 'body'				
			,  (
					case name
						when 'abstract' then content  
						else ''
						end
				) as 'teaser'				
			, 'keyword' as type
			from nodewords 
			join node b on nodewords.mtid = b.nid
			where name in ('abstract', 'description', 'keywords')
			and b.status = 1
			and content <> '%s'
			group by id
			order by mtid desc";
}

/**
 * Private function to get the cron truncate query
 */
function _customsearch_get_crontruncatequery(){
	return "truncate table customsearch_data";
}

/**
 * Private function to get the related properties query
 */
function _customsearch_get_relatedspecialoffersquery(){
	return "select b.title, b.nid from content_field_hotels a
			join node b on a.nid = b.nid and b.type = 'special_offers' and b.status = 1           
			where a.field_hotels_nid in (%s)
			group by b.nid
			order by b.vid desc";
}

/**
 * Private function to get the related destinations query
 */
function _customsearch_get_relateddestinationsquery(){
	return "select a.title, a.nid from node a where a.nid in (%s)";
}

/**
 * Private function to get the related special offers query
 */
function _customsearch_get_relatedpropertiesquery($nids){	
	return "select b.title, b.nid 
			from content_field_region_sub_category a
			join node b on a.nid = b.nid and b.type = 'hotels' and b.status = 1 
			where a.field_region_sub_category_value in (select title from node where nid in (%s))
			group by b.nid
			order by b.title desc";
}



/**
 * db creation script
 delimiter $$

CREATE TABLE `customsearch_data` (
  `nid` int(11) NOT NULL,
  `category` varchar(20) DEFAULT NULL,
  `category_sort_order` int(11) DEFAULT NULL,
  `title` varchar(255) CHARACTER SET latin1 DEFAULT NULL,
  `body` longtext CHARACTER SET latin1,
  `teaser` longtext CHARACTER SET latin1,
  `type` varchar(32) CHARACTER SET latin1 DEFAULT NULL,
  PRIMARY KEY (`nid`),
  FULLTEXT KEY `search_title` (`title`),
  FULLTEXT KEY `search_body` (`body`),
  FULLTEXT KEY `search_teaser` (`teaser`),
  FULLTEXT KEY `search_all` (`title`,`body`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8$$


 */
 






