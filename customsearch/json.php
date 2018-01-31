<?php
	if(sizeof($_REQUEST) > 0){		
		$con = mysql_connect("localhost","root","root");
		mysql_select_db("drupal", $con);
		$jsondata = '';
		$jsonarray = array();
		if($_REQUEST['term']){
			$value = mysql_real_escape_string($_REQUEST['term']);
			
			mysql_query('SET CHARACTER SET utf8') ;
			$select_query = "select match(a.title) against('+$value*' in boolean mode) as matched
				,(
					case a.title
						when '$value' then 10000
						else 
				(
					select if
						(
							(locate('$value', a.title) = 0), 
							-10000, 
							-(locate('$value', a.title))
						)
				)
						end
				) as score
				, a.category
				, a.title
				, a.nid
				from customsearch_data a
				having matched > 0 
				order by matched desc, score desc, title;";

			$data = mysql_query($select_query);
				
			$groupedresults = array();
			while($match = mysql_fetch_array($data)){
				$groupedresults[$match['category']][$match['title']] = $match;
			}
		}
		
		foreach($groupedresults as $key=>$result){		
			foreach($result as $key=>$result){
				if(mb_detect_encoding($key) != 'UTF-8'){
					$key = iconv(mb_detect_encoding($key), "UTF-8", $key);
				}
				$jsonarray[] = $key;				
				$jsondata .= '"'.$key.'",';
			}
		}
		$jsondata = rtrim($jsondata, ",");
		$jsondata = '['.$jsondata.']';
		echo json_encode($jsonarray, JSON_HEX_QUOT | JSON_HEX_TAG |JSON_HEX_AMP |JSON_HEX_APOS);
	}
?>