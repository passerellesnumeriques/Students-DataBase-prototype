if (typeof add_javascript != 'undefined') {
	var url = get_script_path("field_date.js");
	add_javascript(url+"../date_picker/date_picker.js");
	add_javascript(url+"../context_menu/context_menu.js");
}
function field_date() {
	var t = this;
	
	t.create = function(parent,data) {
		var input = document.createElement("INPUT");
		input.type = "text";
		input.size = 10;
		if (data) input.value = data;
		input.style.margin = "0px";
		input.style.padding = "0px";
		input.data = data; // keep original value
		input.onclick = function(ev) {
			var picker = new date_picker();
			if (input.value) {
				var d = t.parseDate(input.value);
				picker.setDate(d)
			}
			picker.onchange = function(p,date) {
				input.value = ""+date.getFullYear()+"-"+(date.getMonth()+1)+"-"+date.getDate();
			};
			var p = picker.getElement();
			new context_menu(p).showBelowElement(input);
			input.blur();
			return false;
		};
		return parent.appendChild(input);
	};
	t.parseDate = function(date_str) {
		if (date_str == null) return new Date();
		var s = date_str;
		var i = s.indexOf('-');
		var year = parseInt(s.substring(0,i));
		s = s.substring(i+1);
		i = s.indexOf('-');
		var month = parseInt(s.substring(0,i))-1;
		var day = parseInt(s.substring(i+1));
		return new Date(year,month,day);
	}
	
	this.isEditable = function() { return true; };
	this.hasChanged = function(input) { return t.parseDate(input.value).getTime() != t.parseDate(input.data).getTime(); };
	this.getValue = function(input) { return input.value; };
	this.getOriginalValue = function(input) { return input.data; };
}