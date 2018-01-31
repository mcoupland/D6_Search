<?php 
	if($resultdata['numberofresults'] > 0 && ($resultdata['numberofpages'] > 1)){ 
?>
		<ul class="content-list columns customsearch-pager">
<?php 
		if($resultdata['numberofpages'] > 1){ 
?>	
			<li class="label">Page</li>
<?php 
			for($i = 1; $i <= $resultdata['numberofpages']; $i++){ 
				if($i == $resultdata['numberofpages']){
					$class = 'class="last"';
				}
?>
				<li <?php print $class; ?>>
<?php
				if($resultdata['currentpage'] == $i){
?>
					<?php print $i;?>
<?php 
				}
				else{
?>
					<a href="/keyword-search/<?php print $resultdata['term']; ?>/<?php print $i;?>">
						<?php print $i;?>
					</a>
<?php
				}
			}
			if($resultdata['currentpage'] > 1){
?>
				<li class="equal-widths odd first separator-right">
					<a href="/keyword-search/<?php print $resultdata['term']; ?>/1">
						First
					</a>
				</li>
				<li class="equal-widths even<?php print $currentpage < $resultdata['numberofpages'] ? " separator-right" : ""; ?>">
					<a href="/keyword-search/<?php print $resultdata['term']; ?>/<?php print $resultdata['currentpage'] - 1;?>">
						Previous
					</a>
				</li>
<?php
			}
			if($resultdata['currentpage'] < $resultdata['numberofpages']){
?>
				<li class="equal-widths odd separator-right">
					<a href="/keyword-search/<?php print $resultdata['term']; ?>/<?php print $resultdata['currentpage'] + 1; ?>">
						Next
					</a>
				</li>
				<li class="equal-widths even last">
					<a href="/keyword-search/<?php print $resultdata['term']; ?>/<?php print $resultdata['numberofpages']; ?>">
						Last
					</a>
				</li>
<?php 
			} 
		}
?>
		</ul>
<?php 
	}
?>