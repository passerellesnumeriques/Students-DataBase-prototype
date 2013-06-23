<?php
$this->add_javascript("/static/geography/address.js");
?>
<div id='address_container'></div>
<script type='text/javascript'>
new geography_address(document.getElementById('address_container'));
</script>