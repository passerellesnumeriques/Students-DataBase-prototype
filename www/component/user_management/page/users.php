<?php
require_once("component/data_model/list/DataList.inc");
$list = new DataList("Users");
$list->primary_key("Users.domain","Users.username");
$list->add("Users.domain", false);
$list->add("Users.username", false);
$list->add("UserPeople.people>first_name", false);
$list->add("UserPeople.people>last_name", false);
$list->add("UserRole.role_id>name", false);
if (PNApplication::$instance->user_management->has_right("assign_role")) {
	$list->selectable();
	$list->add_selection_action("<div class='button' onclick='um_users_assign_roles()'>".get_locale("Assign roles")."</div>");
	$list->add_selection_action("<div class='button' onclick='um_users_unassign_roles()'>".get_locale("Unassign roles")."</div>");
}
$list->add_header("<div class='button' onclick='alert(\"TODO\");'><img src='/static/common/images/add.png' style='vertical-align:bottom'/>".get_locale("Add user")."</div>");
if (PNApplication::$instance->user_management->has_right("consult_user_rights"))
	$list->add_item_action("/static/user_management/access_list.png",get_locale("Access Rights"),"user_rights?domain=%Users.domain%&username=%Users.username%");
$list->build($this);

$this->add_javascript("/static/common/js/popup_window/popup_window.js");
$this->add_stylesheet("/static/common/js/popup_window/popup_window.css");
if (PNApplication::$instance->user_management->has_right("assign_role")) {
?>
<div style='visibility:hidden;position:absolute;top:-10000px;' id='assign_roles_popup'>
<?php
echo " <span id='assign_unassign'></span> ";
echo " <span id='nb_assign'></span> ";
locale("users");
echo ":<br/>";
echo "<form name='assign_roles_form' onsubmit='return false'>";
$roles = DataModel::get()->getTable("Role")->select()->order_by('Role','name')->execute();
foreach ($roles as $role) {
	echo "<input type='checkbox' name='role_".$role['id']."'/> ".$role['name']."<br/>";
}
echo "</form>";
?>
</div>
<script type='text/javascript'>
function um_users_assign_roles() {
	var sel = datalist_grid.getSelection();
	var data = {};
	for (var i = 0; i < sel.length; ++i)
		data["user"+i] = datalist_data_keys[sel[i]];
	var content = document.getElementById('assign_roles_popup');
	document.getElementById('nb_assign').innerHTML = sel.length;
	document.getElementById('assign_unassign').innerHTML = "<?php locale("Assign the following roles to the");?>";
	var popup = new popup_window("<?php locale("Assign roles")?>","/static/user_management/role.png",content);
	popup.keep_content_on_close = true;
	popup.addOkCancelButtons(function() {
		var form = document.forms['assign_roles_form'];
		var j = 0;
		for (var i = 0; i < form.elements.length; ++i)
			if (form.elements[i].checked)
				data["role"+(j++)] = form.elements[i].name.substring(5);
		ajax.post_parse_result("/dynamic/user_management/service/assign_roles",data,function(result) {
			popup.close();
			if (result)
				location.reload();
		},true);
	});
	popup.show();
}
function um_users_unassign_roles() {
	var sel = datalist_grid.getSelection();
	var data = {};
	for (var i = 0; i < sel.length; ++i)
		data["user"+i] = datalist_data_keys[sel[i]];
	var content = document.getElementById('assign_roles_popup');
	document.getElementById('nb_assign').innerHTML = sel.length;
	document.getElementById('assign_unassign').innerHTML = "<?php locale("Unassign the following roles to the");?>";
	var popup = new popup_window("<?php locale("Unassign roles")?>","/static/user_management/role.png",content);
	popup.keep_content_on_close = true;
	popup.addOkCancelButtons(function() {
		var form = document.forms['assign_roles_form'];
		var j = 0;
		for (var i = 0; i < form.elements.length; ++i)
			if (form.elements[i].checked)
				data["role"+(j++)] = form.elements[i].name.substring(5);
		ajax.post_parse_result("/dynamic/user_management/service/unassign_roles",data,function(result) {
			popup.close();
			if (result)
				location.reload();
		},true);
	});
	popup.show();
}
</script>
<?php }?>