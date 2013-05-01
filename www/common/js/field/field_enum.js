function field_enum(values,onchanged,onunchanged) {
	this.create = function(parent, data) {
		var select = document.createElement("SELECT");
		var selected = 0;
		var o = document.createElement("OPTION");
		o.value = "";
		select.add(o);
		for (var i = 0; i < values.length; ++i) {
			o = document.createElement("OPTION");
			o.value = values[i];
			o.innerHTML = values[i];
			select.add(o);
			if (data == values[i]) selected = i+1;
		}
		select.selectedIndex = selected;
		select.style.margin = "0px";
		select.style.padding = "0px";
		select.data = data; // keep original value
		var f = function() {
			setTimeout(function() {
				var val = select.selectedIndex >= 0 ? select.options[select.selectedIndex].value : null;
				if (val != data) {
					if (onchanged)
						onchanged(select, val);
				} else {
					if (onunchanged)
						onunchanged(select, val);
				}
			},1);
		};
		select.onchange = f;
		select.onblur = f;
		return parent.appendChild(select);
	}
	this.isEditable = function() { return true; };
	this.hasChanged = function(input) { return input.options[input.selectedIndex].value != input.data; };
	this.getValue = function(input) { return input.options[input.selectedIndex].value; };
	this.getOriginalValue = function(input) { return input.data; };
}