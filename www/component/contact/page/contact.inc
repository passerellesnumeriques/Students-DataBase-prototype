<?php
function contacts_page($page, $table_list, $table_list_fields, $table_list_field_contact_id, $edit) {
	$page_id = $page->generate_id();
	$page->add_javascript("/static/common/js/context_menu/context_menu.js");
	$page->add_stylesheet("/static/common/js/context_menu/context_menu.css");
	require_once("common/SQLQuery.inc");
	$q = SQLQuery::create()->select($table_list);
	foreach ($table_list_fields as $name=>$value)
		$q->where($name, $value);
	$q->join($table_list, "Contact", array($table_list_field_contact_id=>"id"));
	$contacts = $q->execute();

	echo "<table>";

	echo "<tr>";
	echo "<td valign=top><img src='/static/contact/email_16.png' style='vertical-align:bottom'/> E-Mail</td>";
	echo "<td>";
	echo "<table><tbody id='contact_email_table_".$page_id."'>";
	foreach ($contacts as $c) {
		if ($c["type"] == "EMail") {
			echo "<tr contact_id='".$c["id"]."'>";
			echo "<td contact_type=".json_encode($c["sub_type"]).">";
			echo "</td>";
			echo "<td>";
			echo $c["contact"];
			echo "</td>";
			echo "</tr>";
		}
	}
	if ($edit)
		echo "<tr><td colspan=2 align=right><a href='#' onclick=\"contacts_add_".$page_id."(this, 'email')\">".get_locale("contact", "Add email")."</a></td></tr>";
	echo "</table>";
	echo "</td>";
	echo "</tr>";

	echo "<tr>";
	echo "<td valign=top><img src='/static/contact/phone_16.png' style='vertical-align:bottom'/> ".get_locale("contact","Phone")."</td>";
	echo "<td>";
	echo "<table><tbody id='contact_phone_table_".$page_id."'>";
	foreach ($contacts as $c) {
		if ($c["type"] == "Phone") {
			echo "<tr contact_id='".$c["id"]."'>";
			echo "<td contact_type=".json_encode($c["sub_type"]).">";
			echo "</td>";
			echo "<td>";
			echo $c["contact"];
			echo "</td>";
			echo "</tr>";
		}
	}
	if ($edit)
		echo "<tr><td colspan=2 align=right><a href='#' onclick=\"contacts_add_".$page_id."(this, 'phone')\">".get_locale("contact", "Add phone")."</a></td></tr>";
	echo "</table>";
	echo "</td>";
	echo "</tr>";

	echo "<tr>";
	echo "<td valign=top><img src='/static/contact/im_16.png' style='vertical-align:bottom'/> ".get_locale("contact","Instant Messaging")."</td>";
	echo "<td>";
	echo "<table><tbody id='contact_im_table_".$page_id."'>";
	foreach ($contacts as $c) {
		if ($c["type"] == "IM") {
			echo "<tr contact_id='".$c["id"]."'>";
			echo "<td contact_type=".json_encode($c["sub_type"]).">";
			echo "</td>";
			echo "<td>";
			echo $c["contact"];
			echo "</td>";
			echo "</tr>";
		}
	}
	if ($edit)
		echo "<tr><td colspan=2 align=right><a href='#' onclick=\"contacts_add_".$page_id."(this, 'im')\">".get_locale("contact", "Add IM")."</a></td></tr>";
	echo "</table>";
	echo "</td>";
	echo "</tr>";

	echo "</table>";
	if ($edit) {
?>
<script type='text/javascript'>
var contacts_type = {
	email: [
	    {name:"work", icon:"work_16.png", locale: <?php echo json_encode(get_locale("contact","Work"));?>},
	    {name:"home", icon:"home_16.png", locale: <?php echo json_encode(get_locale("contact","Home"));?>}
	],
	phone: [
		{name:"mobile", icon:"mobile_16.png", locale: <?php echo json_encode(get_locale("contact","Mobile"));?>},
		{name:"work", icon:"work_16.png", locale: <?php echo json_encode(get_locale("contact","Work"));?>},
	    {name:"home", icon:"home_16.png", locale: <?php echo json_encode(get_locale("contact","Home"));?>}
	],
	im: [
		{name:"skype", icon:"skype_16.png", locale: 'Skype'},
	]
};
function contacts_add_<?php echo $page_id?>(link, contact_type) {
	var table = document.getElementById('contact_'+contact_type+'_table_<?php echo $page_id?>');
	var last_tr = link.parentNode.parentNode;
	var tr = document.createElement("TR"); table.insertBefore(tr, last_tr);
	var td = document.createElement("TD"); tr.appendChild(td);
	td.innerHTML = "<span style='font-style:italic;color:#808080;border:1px solid #808080;padding:1px 4px 1px 2px;'><?php locale("contact","Type");?></span>";
	td.style.cursor = 'pointer';
	td.onclick = function() { contacts_type_click(this, contact_type); }
	td = document.createElement("TD"); tr.appendChild(td);
	td.style.paddingLeft = "5px";
	contacts_edit(td, contact_type);
	contacts_add_actions(tr, contact_type);
}
function contacts_build_table(contact_type) {
	var table = document.getElementById('contact_'+contact_type+'_table_<?php echo $page_id?>');
	for (var i = 0; i < table.childNodes.length; ++i) {
		var tr = table.childNodes[i];
		if (tr.nodeType != 1) continue;
		var td = tr.childNodes[0];
		td.style.cursor = 'pointer';
		td.onclick = function() { contacts_type_click(this, contact_type); }
		var type = td.getAttribute("contact_type");
		contacts_set_type(td, type, false, contact_type);
		td = tr.childNodes[1];
		td.onclick = function() { contacts_edit(td, contact_type); };
		td.style.cursor = "pointer";
		td.style.paddingLeft = "5px";
		contacts_add_actions(tr, contact_type);
	}
}
function contacts_add_actions(tr, contact_type) {
	var td = document.createElement("TD");
	var img = document.createElement("IMG");
	img.src = "/static/contact/trash_16.png";
	img.onclick = function() {
		// TODO
	};
	img.style.cursor = "pointer";
	td.appendChild(img);
	tr.appendChild(td);
}
function contacts_type_click(td, contact_type) {
	var menu = new context_menu();
	for (var i = 0; i < contacts_type[contact_type].length; ++i) {
		var item = menu.addIconItem("/static/contact/"+contacts_type[contact_type][i].icon, contacts_type[contact_type][i].locale, null);
		item._onclick = item.onclick;
		item._name = contacts_type[contact_type][i].name;
		item.onclick = function() {
			contacts_set_type(td, this._name, true, contact_type);
			item._onclick();
		};
	}
	var div = document.createElement("DIV");
	div.innerHTML = "<?php locale("contact","Other")?>: ";
	var input = document.createElement("INPUT");
	input.type = 'text';
	input.size = 10;
	input.maxlength = 200;
	input.onkeydown = function(ev) {
		ev = getCompatibleKeyEvent(ev);
		if (ev.keyCode == 13) {
			contacts_set_type(td,input.value,true,contact_type);
			menu.hide();
		};
	};
	input.value = td.getAttribute("contact_type");
	for (var i = 0; i < contacts_type[contact_type].length; ++i)
		if (contacts_type[contact_type][i].name == input.value) { input.value = ""; break; }
	div.appendChild(input);
	menu.addItem(div);
	div.onclick = null;
	menu.removeOnClose = true;
	menu.showBelowElement(td);
}
function contacts_set_type(td, type, save_it, contact_type) {
	var standard_type = false;
	for (var i = 0; i < contacts_type[contact_type].length; ++i)
		if (type == contacts_type[contact_type][i].name) {
			td.innerHTML = "<img src='/static/contact/"+contacts_type[contact_type][i].icon+"' style='vertical-align:bottom'/> "+contacts_type[contact_type][i].locale;
			standard_type = true;
			break;
		}
	if (!standard_type)
		td.innerHTML = type;
	if (save_it) {
		if (td.getAttribute("contact_type") == type) return; // same
		td.setAttribute("contact_type", type);
		contacts_save(td.parentNode, contact_type);
	}
}
function contacts_edit(td, contact_type) {
	var input = document.createElement("INPUT");
	input.type = 'text';
	input.value = td.innerHTML;
	td.onclick = null;
	td.innerHTML = "";
	td.appendChild(input);
	input.onblur = function() {
		if (input.value.length == 0) return;
		td.innerHTML = input.value;
		td.onclick = function() { contacts_edit(td, contact_type); };
		td.style.cursor = "pointer";
		contacts_save(td.parentNode, contact_type);
	};
	input.focus();
}
function contacts_save(tr, contact_type) {
	var id = 0;
	if (tr.hasAttribute("contact_id")) id = tr.getAttribute("contact_id");
	var type = tr.childNodes[0].getAttribute("contact_type");
	if (type == null || type.length == 0) // not saved
	var contact = tr.childNodes[1].innerHTML;
	if (!contact || contact.length == 0) return; // not saved
	if (tr.getAttribute("saving") == "yes") return; // in progress
	if (tr.getAttribute("saving_type") == type && tr.getAttribute("saving_contact") == contact) return; // already saved
	tr.setAttribute("saving_type", type);
	tr.setAttribute("saving_contact", contact);
	tr.setAttribute("saving","yes");
	ajax.post_parse_result("/dynamic/contact/service/save_contact",{id:id,type:contact_type,sub_type:type,contact:contact},function(result) {
		if (!result) {
			tr.setAttribute("saving", "no");
			return;
		}

	});
}
</script>
<?php
	}
}
?>