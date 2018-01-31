<?php 
$limit = 20;
$count = count($searchresults);
if($count > 0){
?>
	<ul id="customsearch-searchresults" class="content-list rows">
<?php
	for($i = 0; $i < $count; $i++){
		if($i >= $limit){ break; }
		$stripe = $i % 2 ? 'searchresult-even' : 'searchresult-odd';
		$resultnumber = 'searchresult-'.$i;
		switch($i){
			case 0:
				$resultspecial = 'searchresult-first';
				break;
			case $count-1:
				$resultspecial = 'searchresult-last';
				break;
			default:
				$resultspecial = '';
				break;
		}	
		$classes = $stripe.' '.$searchresults[$i]['classes'].' '.$resultnumber.' '.$resultspecial;
?>
		<li class="<?php print $classes; ?>"><?php print $searchresults[$i]['content']; ?></li>
<?php 
	} 
?>
	</ul>
<?php
}
?>


