<?php
$this->add_javascript("/static/common/js/splitter_vertical/splitter_vertical.js");
$this->add_stylesheet("/static/common/js/splitter_vertical/splitter_vertical.css");
?>
<div id='edit_countries_split1' style='height:100%'>
	<iframe name='geo_db' src='/dynamic/geography/page/edit_countries_db' frameBorder=0 style='height:100%'></iframe>
	<div id='edit_countries_split2' style='height:100%'>
		<iframe name='geonames' src='/dynamic/geography/page/edit_countries_db_temp?db=geonames' frameBorder=0 style='height:100%'></iframe>
		<iframe name='gadm' src='/dynamic/geography/page/edit_countries_db_temp?db=gadm' frameBorder=0 style='height:100%'></iframe>
	</div>
</div>
<script type='text/javascript'>
new splitter_vertical('edit_countries_split2',0.5);
new splitter_vertical('edit_countries_split1',0.3);
</script>