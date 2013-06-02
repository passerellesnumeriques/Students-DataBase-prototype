<?php
require_once("common/SQLQuery.inc");
$roles = SQLQuery::create()->select("Role")->field('id')->field('name')->field("UserRole","username")->join("Role","UserRole",array("id"=>"role_id"))->count("nb_users")->group_by("Role","id")->order_by("Role","name",true)->execute();

$this->add_javascript("/static/common/js/wizard/wizard.js");
$this->add_stylesheet("/static/common/js/wizard/wizard.css");
$this->add_javascript("/static/common/js/validation.js");
$this->add_stylesheet("/static/common/js/validation.css");

require_once("component/application/SubPageHeader.inc");
$header = new SubPageHeader($this, "/static/user_management/role_32.png", get_locale("Roles"));
$header->add_header("<div class='button' onclick=\"new wizard('new_role_wizard').launch()\"><img src='/static/common/images/add.png'/> ".get_locale("New Role")."</div>");
$header->generate();

echo "<table rules='all' style='border:1px solid black;margin:5px'>";
echo "<tr>";
echo "<th>".get_locale("Role")."</th>";
echo "<th>".get_locale("Users")."</th>";
echo "<th>".get_locale("common","Actions")."</th>";
echo "</tr>";
foreach ($roles as $role) {
	echo "<tr>";
	echo "<td>";
	echo $role["name"];
	echo "</td>";
	echo "<td>";
	if ($role["username"] == null) $role["nb_users"] = 0;
	echo $role["nb_users"];
	echo "</td>";
	echo "<td>";
	echo "<img src='/static/user_management/access_list.png' title=\"".get_locale("Access Rights")."\" style='cursor:pointer' onclick=\"location='role_rights?role=".$role["id"]."';\"/> ";
	echo "<img src='/static/common/images/remove.png' title=\"".get_locale("common","Remove")."\" style='cursor:pointer' onclick=\"remove_role(".$role["id"].",'".$role["name"]."',".$role["nb_users"].");\"/>";
	echo "</td>";
	echo "</tr>";
}
echo "</table>";
?>
<div id='new_role_wizard' class='wizard'
	title="<?php locale("New Role")?>"
	icon="/static/common/images/add.png"
	finish="new_role_finish"
>
	<div class='wizard_page'
		title='<?php locale("Role")?>'
		icon='/static/user_management/role_32.png'
		validate="new_role_validate"
	>
		<form name='new_role_wizard' onsubmit='return false'>
			<?php locale("Role Name")?> <input type='text' size=30 maxlength=100 name='role_name' onkeyup="wizard_validate(this)"/>
			<span class='validation_message' id='role_name_validation'></span>
		</form>
	</div>
</div>
<script type='text/javascript'>
var existing_roles = [<?php
$first = true;
foreach ($roles as $role) {
	if ($first) $first = false; else echo ",";
	echo json_encode($role["name"]);
}
?>];
function new_role_validate(wizard,handler) {
	var form = document.forms['new_role_wizard'];
	var name = form.elements['role_name'];
	var ok = true;
	// check name not empty, and does not exist yet
	if (name.value.length == 0) {
		validation_error(name, "<?php locale("common","Cannot be empty");?>");
		ok = false;
	} else {
		// check the name does not exist yet
		for (var i = 0; i < existing_roles.length; ++i)
			if (name.value == existing_roles[i]) {
				ok = false;
				validation_error(name, "<?php locale("common","__ already exists",array("name"=>get_locale("Role")));?>");
			}
		if (ok)
			validation_ok(name);
	}
	wizard.resize();
	handler(ok);
}
function new_role_finish(wizard) {
	var form = document.forms['new_role_wizard'];
	var name = form.elements['role_name'].value;
	ajax.post_parse_result("/dynamic/user_management/service/create_role",{name:name},function(result){
		if (result && result.id)
			location.href = '/dynamic/user_management/page/role?id='+result.id;
	},true);
}

function remove_role(id,name,nb_users) {
	confirm_dialog("<?php locale("common", "Are you sure you want to remove")?> <?php locale("the role")?> <i>"+name+"</i><br/><?php locale("and unassign")?> "+nb_users+" "+(nb_users>1?"<?php locale("users")?>":"<?php locale("user")?>")+" ?",function(confirmed){
		if (!confirmed) return;
		ajax.post_parse_result("/dynamic/user_management/service/remove_role",{id:id},function(result) {
			if (result) location.reload();
		}, true);
	});
}
</script>