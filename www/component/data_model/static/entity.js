function data_entity_edit(icon, table, field, field_type, key) {
	icon.src = '/static/common/images/loading.gif';
	icon.onclick = null;
	ajax.post_parse_result("/dynamic/data_model/service/lock_entity",{table:table,key:key,field:field},function(result) {
		if (result == null) {
			icon.src = '/static/common/images/edit.png';
			icon.onclick = function() { data_entity_edit(this, table, field, field_type, key); };
			return;
		}
		pn_database_locks.add_lock(parseInt(result.lock));
		var f = datamodel_create_field(field_type, true);
		var td = icon.parentNode;
		td = td.previousSibling;
		while (td.nodeName != "TD") td = td.previousSibling;
		td.innerHTML = "";
		var input = f.create(td, result.value);
		icon.src = '/static/common/images/save.png';
		icon.onclick = function() { data_entity_save(this, table, field, field_type, key, result.lock); };
		icon.data = [f,input];
		var img = document.createElement("IMG");
		img.src = '/static/common/images/no_edit.png';
		img.style.cursor = 'pointer';
		img.onclick = function() { data_entity_cancel_edit(this, table, field, field_type, key, result.lock); };
		icon.parentNode.appendChild(img);
	});
}
function data_entity_cancel_edit(icon, table, field, field_type, key, lock_id) {
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
	ajax.post_parse_result("/dynamic/data_model/service/unlock_entity",{lock:lock_id},function(result) {
		pn_database_locks.remove_lock(lock_id);
		icon.src = '/static/common/images/edit.png';
		icon.onclick = function() { data_entity_edit(this, table, field, field_type, key); };
	});	
}
function data_entity_save(icon, table, field, field_type, key, lock_id) {
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
		ajax.post_parse_result("/dynamic/data_model/service/unlock_entity",{lock:lock_id},function(result) {
			pn_database_locks.remove_lock(lock_id);
			icon.src = '/static/common/images/edit.png';
			icon.onclick = function() { data_entity_edit(this, table, field, field_type, key); };
		});
		return;	
	}
	var value = f[0].getValue(f[1]);
	ajax.post_parse_result("/dynamic/data_model/service/save_entity",{lock:lock_id,table:table,key:key,field:field,value:value},function(result) {
		pn_database_locks.remove_lock(lock_id);
		icon.src = '/static/common/images/edit.png';
		icon.onclick = function() { data_entity_edit(this, table, field, field_type, key); };
		if (!result)
			value = f[0].getOriginalValue(f[1]);
		td.innerHTML = value;
	});
}