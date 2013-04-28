<?php 
$people = $_GET["people"];
if ($people <> PNApplication::$instance->people->user_people_id) {
	if (!PNApplication::$instance->user_management->has_right("see_other_people_details"))
		die(get_locale("common","Access Denied"));
}
$can_edit = PNApplication::$instance->user_management->has_right("edit_people_details");
require_once("common/SQLQuery.inc");
$people = SQLQuery::create()->select("People")->where("id",$people)->execute_single_row();
?>
<table>
<tr>
<td>
	<img height='150px' src='/static/people/default_<?php if ($people["sex"] == "F") echo "female"; else echo "male";?>.jpg'/>
</td>
<td>
	<table>
		<tr>
			<td><?php locale("First Name")?></td>
			<td><?php echo $people["first_name"]?></td>
<?php if ($can_edit){ ?><td><img src='/static/common/images/edit.png' onclick="people_edit(this,'first_name');" style='cursor:pointer'/></td><?php }?>
		</tr>
		<tr>
			<td><?php locale("Last Name")?></td>
			<td><?php echo $people["last_name"]?></td>
<?php if ($can_edit){ ?><td><img src='/static/common/images/edit.png' onclick="people_edit(this,'last_name');" style='cursor:pointer'/></td><?php }?>
		</tr>
		<tr>
			<td><?php locale("Sex")?></td>
			<td><?php echo $people["sex"]?></td>
<?php if ($can_edit){ ?><td><img src='/static/common/images/edit.png' onclick="people_edit(this,'sex');" style='cursor:pointer'/></td><?php }?>
		</tr>
		<tr>
			<td><?php locale("Birth Date")?></td>
			<td><?php echo $people["birth"]?></td>
<?php if ($can_edit){ ?><td><img src='/static/common/images/edit.png' onclick="people_edit(this,'birth');" style='cursor:pointer'/></td><?php }?>
		</tr>
	</table>
</td>
</tr>
</table>
<script type='text/javascript'>
function people_edit(icon, field) {
	icon.src = '/static/common/images/loading.gif';
	icon.onclick = null;
	pn.ajax_service_json("/dynamic/people/service/lock",{field:field,id:<?php echo $people["id"]?>},function(result) {
		if (result == null) {
			icon.src = '/static/common/images/edit.png';
			icon.onclick = function() { people_edit(this, field); };
			return;
		}
		pn_database_locks.add_lock(parseInt(result.lock));
		var type;
		switch (field) {
		case "first_name": case "last_name": type = "editable_text"; break;
		case "sex": type="enum"; break;
		case "birth": type = "date"; break;
		}
		pn.add_javascript("/static/common/js/field/field_"+type+".js",function() {
			var f;
			switch (field) {
			case "first_name": case "last_name": 
				f = new field_editable_text(100);
				break;
			case "sex":
				f = new field_enum(["M","F"]); 
				break;
			case "birth": 
				f = new field_date();
				break;
			}
			var td = icon.parentNode;
			td = td.previousSibling;
			while (td.nodeName != "TD") td = td.previousSibling;
			td.innerHTML = "";
			var input = f.create(td, result.value);
			icon.src = '/static/common/images/save.png';
			icon.onclick = function() { people_save(this, field, result.lock); };
			icon.data = [f,input];
			var img = document.createElement("IMG");
			img.src = '/static/common/images/no_edit.png';
			img.style.cursor = 'pointer';
			img.onclick = function() { people_cancel_edit(this, field, result.lock); };
			icon.parentNode.appendChild(img);
		});
	});
}
function people_cancel_edit(icon, field, lock_id) {
	var td = icon.parentNode;
	td.removeChild(icon);
	icon = td.childNodes[0];
	icon.src = '/static/common/images/loading.gif';
	icon.onclick = null;
	var f = icon.data;
	var value = f[0].getOriginalValue(f[1]);
	var td = icon.parentNode.previousSibling;
	while (td.nodeName != "TD") td = td.previousSibling;
	td.innerHTML = value;
	pn.ajax_service_json("/dynamic/people/service/unlock",{id:lock_id},function(result) {
		pn_database_locks.remove_lock(lock_id);
		icon.src = '/static/common/images/edit.png';
		icon.onclick = function() { people_edit(this, field); };
	});	
}
function people_save(icon, field, lock_id) {
	var td = icon.parentNode;
	td.removeChild(td.childNodes[td.childNodes.length-1]);
	icon.src = '/static/common/images/loading.gif';
	icon.onclick = null;
	var td = icon.parentNode.previousSibling;
	while (td.nodeName != "TD") td = td.previousSibling;
	var f = icon.data;
	if (!f[0].hasChanged(f[1])) {
		var value = f[0].getOriginalValue(f[1]);
		td.innerHTML = value;
		pn.ajax_service_json("/dynamic/people/service/unlock",{id:lock_id},function(result) {
			pn_database_locks.remove_lock(lock_id);
			icon.src = '/static/common/images/edit.png';
			icon.onclick = function() { people_edit(this, field); };
		});
		return;	
	}
	var value = f[0].getValue(f[1]);
	pn.ajax_service_json("/dynamic/people/service/save",{lock:lock_id,id:<?php echo $people["id"]?>,field:field,value:value},function(result) {
		pn_database_locks.remove_lock(lock_id);
		icon.src = '/static/common/images/edit.png';
		icon.onclick = function() { people_edit(this, field); };
		if (!result)
			value = f[0].getOriginalValue(f[1]);
		td.innerHTML = value;
	});
}
</script>