<?php 
/**
 * variables
 * $ishero
 * $imagesrc
 * $nodelink
 * $title
 * $nodecontent
 * $relatedlinks
 * $icons
 */
 ?>
<?php if($imagesrc){ ?>
<div <?php print $imagesrc ? 'class="grid-3 alpha"' : ''; ?>>
	<a href="<?php print $nodelink; ?>">
		<img src="<?php print $imagesrc; ?>" class="thumbnail" />
	</a>
</div>
<?php } ?>
<div <?php print $imagesrc ? 'class="grid-9 omega"' : 'class="grid-12 alpha omega"'; ?>>
	<h5>
		<a href="<?php print $nodelink; ?>">
			<?php print $title; ?>
		</a>
	</h5>
	<?php if($icons){ ?>
	<ul class="content-list columns icon-list">
		<?php foreach($icons as $icon){ ?>
		<li>
			<img src="<?php print $icon['src']; ?>" class="icon tooltip" data="<?php print $icon['data']; ?>" title="<?php print $icon['title']; ?>" />
		</li>
		<?php } ?>
	</ul>
	<?php } ?>
	<p>
		<?php print $nodecontent; ?>
		<span class="read-more">
			<a href="<?php print $nodelink; ?>">learn more<span class="bullet">&raquo;</span></a>
		</span>
	</p>
</div>
<div class="grid-12 alpha omega">
<?php foreach($relatedlinks as $group=>$items){ ?>
	<ul class="grid-3 alpha omega search-result-related">
		<?php 
			$count = 0;
			foreach($items as $title=>$url){ 
				if($count > 2){ break; } 
				$count++;
		?>
		<li class="search-result-related-item"><a href="<?php print $url; ?>"><?php print $title; ?><span class="bullet">&raquo;</span></a></li>
		<?php } ?>
	</ul>
<?php } ?>
</div>
<div class="grid-12 alpha omega">
<?php foreach($relatedlinks as $group=>$items){  ?>
	<ul class="grid-3 alpha omega search-result-related">
		<li class="search-result-related-item"><h6><a href="<?php print $tabs[$group]; ?>">see all <?php print $group; ?><span class="bullet">&raquo;</span></a></h6></li>
	</ul>
<?php } ?>
</div>
