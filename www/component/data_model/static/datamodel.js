function datamodel_create_field(type, editable, onchange, onunchange) {
	var field = null;
	if (type.startsWith("string")) {
		if (editable)
			field = new field_editable_text(type.substring(6,1)==":" ? type.substring(7) : null,onchange,onunchange);
		else
			field = new field_text();
	} else if (type.startsWith("enum[")) {
		var values = eval(type.substring(4));
		if (editable)
			field = new field_enum(values,onchange,onunchange);
		else
			field = new field_text();
	} else if (type == "date") {
		if (editable)
			field = new field_date();
		else
			field = new field_text();
	}
	return field;
}
