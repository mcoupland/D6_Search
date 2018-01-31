<?php if($resultdata['numberofresults'] > 0){ ?>
<h5 class="customsearch-pager">You searched for &ldquo;<?php print $resultdata['term']; ?>&rdquo;. Showing results <?php print $resultdata['firstresultnumber']+1; ?> - <?php print $resultdata['lastresultnumber']+1; ?> of <?php print $resultdata['numberofresults']; ?></h5>
<?php } ?>