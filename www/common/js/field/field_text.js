function field_text() {
	this.create = function(parent, data) {
		var text = document.createTextNode(data);
		return parent.appendChild(text);
	}
}