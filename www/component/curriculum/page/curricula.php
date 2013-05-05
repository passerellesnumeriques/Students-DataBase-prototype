<?php
$this->add_javascript("/static/common/js/component/wizard.js");
$this->add_stylesheet("/static/common/js/component/wizard.css");
$this->add_javascript("/static/common/js/component/validation.js");
$this->add_stylesheet("/static/common/js/component/validation.css");
$this->add_javascript("/static/common/js/component/form.js");

require_once("component/application/SubPageHeader.inc");
$header = new SubPageHeader($this, '/static/curriculum/curriculum_32.png', get_locale("Curricula"));
if (PNApplication::$instance->user_management->has_right("edit_curricula")) {
	$header->add_header("<div class='button' onclick=\"new wizard('new_curriculum_wizard').launch()\"><img src='/static/common/images/add.png'/> ".get_locale("New curriculum")."</div>");
}
$header->generate();

require_once("common/SQLQuery.inc");
$curricula = SQLQuery::create()->select("Curriculum")->execute();
echo "<table rules='all' style='border:1px solid black;border-collapse:collapse;margin:5px'>";
echo "<tr><th>".get_locale("common","Name")."</th></tr>";
foreach ($curricula as $curriculum) {
	echo "<tr>";
	echo "<td>";
	echo "<a href='curriculum?id=".$curriculum["id"]."'>";
	echo $curriculum["name"];
	echo "</a>";
	echo "</td>";
	echo "</tr>";
}
echo "</table>";
if (PNApplication::$instance->user_management->has_right("edit_curricula")) {
?>
<div id='new_curriculum_wizard' class='wizard'
	title="<?php locale("New curriculum")?>"
	icon="/static/common/images/add.png"
	finish="curriculum_wizard_finish"
>
	<div class='wizard_page'
		title='<?php locale("Curriculum")?>'
		icon='/static/curriculum/curriculum_32.png'
		validate="curriculum_wizard_validate"
	>
		<form name='curriculum_wizard' onsubmit='return false'>
		<?php locale("Curriculum Name")?> <input type='text' size=30 maxlength=100 name='curriculum_name' onkeyup="wizard_validate(this)"/>
		<span class='validation_message' id='curriculum_name_validation'></span>
		<br/>
		<?php locale("Create a new curriculum")?><br/>
		<input type='radio' name='creation_type' value='from_scratch' checked='checked' onchange="wizard_validate(this)"/> <?php locale("from scratch")?><br/>
		<input type='radio' name='creation_type' value='copy' onchange="wizard_validate(this)"/> <?php locale("from existing one")?>:
			<select name='copy_curriculum' onchange="wizard_validate(this)">
				<?php foreach ($curricula as $curriculum) echo "<option value='".$curriculum["id"]."'>".$curriculum["name"]."</option>";?>
			</select>
			<span class='validation_message' id='copy_curriculum_validation'></span>
			<br/>
		</form>
	</div>
</div>
<script type='text/javascript'>
function curriculum_wizard_validate(wizard,handler) {
	var form = document.forms['curriculum_wizard'];
	var name = form.elements['curriculum_name'];
	var type = form.elements['creation_type'];
	var copy = form.elements['copy_curriculum'];
	var ok = true;
	// check name not empty, and does not exist yet
	if (name.value.length == 0) {
		validation_error(name, "<?php locale("common","Cannot be empty");?>");
		ok = false;
	} else {
		// check the name does not exist yet
		for (var i = 0; i < copy.options.length; ++i)
			if (name.value == copy.options[i].innerHTML) {
				ok = false;
				validation_error(name, "<?php locale("common","__ already exists", array("name"=>get_locale("Curriculum")));?>");
			}
		if (ok)
			validation_ok(name);
	}
	if (type[0].checked) {
		// from scratch
		validation_ok(copy);
	} else {
		// copy
		if (copy.selectedIndex < 0) {
			validation_error(copy, "<?php locale("common","Please select");?>");
			ok = false;
		} else
			validation_ok(copy);
	}
	wizard.resize();
	handler(ok);
}
function curriculum_wizard_finish(wizard) {
	var form = document.forms['curriculum_wizard'];
	var name = form.elements['curriculum_name'].value;
	var type = get_radio_value(form, 'creation_type');
	var copy = null;
	if (type == 'copy') {
		copy = form.elements['copy_curriculum'];
		copy = copy.options[copy.selectedIndex];
	}
	var data = {name:name};
	if (copy) data["copy"] = copy;
	pn.ajax_service_json("/dynamic/curriculum/service/create_curriculum",data,function(result){
		if (result && result.id)
			location.href = '/dynamic/curriculum/page/curriculum?id='+result.id;
	},true);
}
</script>
<?php }?>