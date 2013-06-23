<?php 
global $app;
$app->user_management->logout();
?>
<script type='text/javascript'>
window.top.location.href = "<?php echo "/".(isset($_GET["from"]) ? "?from=".$_GET["from"] : "");?>";
</script>