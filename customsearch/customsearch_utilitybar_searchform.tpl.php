<script type="text/javascript" src="/sites/all/modules/customsearch/jquery-1.7.2.min.js?T" /></script>
<script type="text/javascript" src="/sites/all/modules/customsearch/jquery-ui-1.8.22.custom.min.js?T"></script>

<script type="text/javascript">
/**
 * See http://jqueryui.com/demos/autocomplete/#remote for more documentation
 */ 
var jq172 = $.noConflict(true);
jq172(function() {	
	jq172( "#edit-searchterm" ).autocomplete({
		source: "/sites/all/modules/customsearch/json.php",
		minLength: 2,
		delay: 0,
		appendTo: "#search-autocomplete",
		select: function( event, ui ) {
			jq172('#edit-searchterm').val(ui.item.value);
			jq172('#searchform').submit();
		}
	});
	jq172( "#edit-utilitybar-searchterm" ).autocomplete({
		source: "/sites/all/modules/customsearch/json.php",
		minLength: 2,
		delay: 0,
		appendTo: "#search-autocomplete",
		select: function( event, ui ) {
			jq172('#edit-utilitybar-searchterm').val(ui.item.value);
			jq172('#utilitybar-searchform').submit();
		}
	});
});
</script>
<?php print $searchform; ?>
<div id="search-autocomplete"></div>