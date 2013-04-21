function field_editable_text(max_length,onchanged,onunchanged) {
	this.create = function(parent, data) {
		var input = document.createElement("INPUT");
		input.type = "text";
		if (max_length) input.max_length = max_length;
		if (data) input.value = data;
		input.style.margin = "0px";
		input.style.padding = "0px";
		input.data = data; // keep original value
		var f = function() {
			setTimeout(function() {
				if (input.value != data) {
					if (onchanged)
						onchanged(input);
				} else {
					if (onunchanged)
						onunchanged(input);
				}
			},1);
		};
		input.onkeyup = f;
		input.onblur = f;
		return parent.appendChild(input);
	}
}