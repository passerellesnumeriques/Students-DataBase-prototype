function error_dialog(message) {
	var path = get_script_path("common_dialogs.js");
	add_stylesheet(path+"popup_window/popup_window.css");
	add_javascript(path+"popup_window/popup_window.js",function() {
		var p = new popup_window(locale.get_string("Error"), common_images_url+"error.png", message);
		p.show();
	});
}
function confirm_dialog(message, handler) {
	var path = get_script_path("common_dialogs.js");
	add_stylesheet(path+"popup_window/popup_window.css");
	add_javascript(path+"popup_window/popup_window.js",function() {
		var p = new popup_window(locale.get_string("Confirmation"), common_images_url+"question.png", message);
		var result = false;
		p.addYesNoButtons(function() {
			result = true;
			p.close();
		});
		p.onclose = function() {
			handler(result);
		};
		p.show();
	});
}
function input_dialog(icon,title,message,default_value,max_length,validation_handler,ok_handler) {
	var path = get_script_path("common_dialogs.js");
	add_stylesheet(path+"popup_window/popup_window.css");
	add_javascript(path+"popup_window/popup_window.js",function() {
		var content = document.createElement("DIV");
		content.innerHTML = message+"<br/>";
		var input = document.createElement("INPUT");
		input.type = 'text';
		input.value = default_value;
		input.maxLength = max_length;
		content.appendChild(input);
		var error_div = document.createElement("DIV");
		error_div.style.visibility = 'hidden';
		error_div.style.position = 'absolute';
		error_div.innerHTML = "<img src='"+common_images_url+"error.png' style='vertical-align:bottom'/> ";
		var error_message = document.createElement("SPAN");
		error_message.style.color = 'red';
		error_div.appendChild(error_message);
		content.appendChild(error_div);
		var p = new popup_window(title, icon, content);
		var result = null;
		p.addOkCancelButtons(function() {
			result = input.value;
			p.close();
		});
		var validate = function() {
			var error = validation_handler(input.value);
			if (error != null) {
				p.disableButton('ok');
				input.style.border = "1px solid red";
				error_message.innerHTML = error;
				error_div.style.visibility = 'visible';
				error_div.style.position = 'static';
				p.resize();
			} else {
				p.enableButton('ok');
				input.style.border = "";
				error_div.style.visibility = 'hidden';
				error_div.style.position = 'absolute';
				p.resize();
			}
		};
		validate();
		input.onkeyup = input.onblur = validate;
		p.onclose = function() {
			ok_handler(result);
		};
		p.show();
	});
}
function multiple_input_dialog(icon,title,inputs,ok_handler) {
	var path = get_script_path("common_dialogs.js");
	add_stylesheet(path+"popup_window/popup_window.css");
	add_javascript(path+"popup_window/popup_window.js",function() {
		var p = null;
		var content = document.createElement("TABLE");
		var update_dialog = function() {
			var ok = true;
			for (var i = 0; i < inputs.length; ++i)
				if (!inputs[i].validation_result) { ok = false; break; }
			if (ok)
				p.enableButton('ok');
			else
				p.disableButton('ok');
			p.resize();
		}
		for (var i = 0; i < inputs.length; ++i) {
			var tr = document.createElement("TR"); content.appendChild(tr);
			var msg = document.createElement("TD");
			msg.innerHTML = inputs[i].message;
			tr.appendChild(msg);
			var td = document.createElement("TD"); tr.appendChild(td);
			inputs[i].input = document.createElement("INPUT");
			inputs[i].input.type = 'text';
			inputs[i].input.value = inputs[i].default_value;
			inputs[i].input.maxLength = inputs[i].max_length;
			td.appendChild(inputs[i].input);
			inputs[i].error_container = document.createElement("TR"); content.appendChild(inputs[i].error_container);
			inputs[i].error_container.style.visibility = 'hidden';
			inputs[i].error_container.style.position = 'absolute';
			td = document.createElement("TD"); inputs[i].error_container.appendChild(td);
			td.colSpan=2;
			td.innerHTML = "<img src='"+common_images_url+"error.png' style='vertical-align:bottom'/> ";
			inputs[i].error_message = document.createElement("SPAN");
			inputs[i].error_message.style.color = 'red';
			td.appendChild(inputs[i].error_message);
			inputs[i].validate = function() {
				var error = this.validation_handler(this.input.value);
				if (error != null) {
					this.input.style.border = "1px solid red";
					this.error_message.innerHTML = error;
					this.error_container.style.visibility = 'visible';
					this.error_container.style.position = 'static';
					this.validation_result = false;
					update_dialog();
				} else {
					this.input.style.border = "";
					this.error_container.style.visibility = 'hidden';
					this.error_container.style.position = 'absolute';
					this.validation_result = true;
					update_dialog();
				}
			};
			inputs[i].input.data = inputs[i];
			inputs[i].input.onkeyup = inputs[i].input.onblur = function() { this.data.validate(); }
		}
		p = new popup_window(title, icon, content);
		var result = null;
		p.addOkCancelButtons(function() {
			result = [];
			for (var i = 0; i < inputs.length; ++i)
				result.push(inputs[i].input.value);
			p.close();
		});
		p.onclose = function() {
			ok_handler(result);
		};
		p.show();
		for (var i = 0; i < inputs.length; ++i)
			inputs[i].validate();
	});
}
