Curricula:<br/>
<?php
$this->add_javascript("/static/common/js/component/wizard.js");
$this->add_stylesheet("/static/common/js/component/wizard.css");
$this->add_javascript("/static/common/js/component/validation.js");
$this->add_stylesheet("/static/common/js/component/validation.css");

$res = DataBase::$conn->execute("SELECT * FROM Curriculum");
while (($r = DataBase::$conn->next_row($res)) <> null) {
	echo $r["name"];
	echo "<br/>";
}
if (PNApplication::$instance->user_management->has_right("edit_curricula")) {
?>
<button onclick="new wizard('new_curriculum_wizard').launch()"><img src='/static/common/images/add.png'/> <?php locale("New curriculum")?></button>
<div id='new_curriculum_wizard' class='wizard'
	title="<?php locale("New curriculum")?>"
	icon="/static/common/images/add.png"
>
	<div class='wizard_page'
		title='<?php locale("Curriculum")?>'
		icon='/static/curriculum/curriculum_32.png'
		validate="curriculum_wizard_validate_name"
	>
		<?php locale("Curriculum Name")?> <input type='text' size=30 maxlength=100 id='curriculum_name' onkeyup="wizard_validate(this)"/><br/>
		<span class='validation_message' id='curriculum_name_validation'></span>
	</div>
	<div class='wizard_page'>
		Page 2
	</div>
</div>
<script type='text/javascript'>
function curriculum_wizard_validate_name(wizard,handler) {
	var name = document.getElementById('curriculum_name');
	if (name.value.length == 0) {
		validation_error(name, "<?php locale("Cannot be empty");?>");
		wizard.resize();
		handler(false);
		return;
	}
	pn.ajax_service_json("/dynamic/curriculum/service/curriculum_exists",{name:name.value},function(exists){
		if (!exists)
			validation_ok(name);
		else
			validation_error(name, "<?php locale("This curriculum already exists");?>");
		wizard.resize();
		handler(!exists);
	});
}
</script>
<?php }?>