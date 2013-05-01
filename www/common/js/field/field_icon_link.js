function field_icon_link() {
	this.create = function(parent,data) {
		var link = document.createElement("A");
		link.href = data.url;
		if (data.target) link.target = data.target;
		var img = document.createElement("IMG");
		img.src = data.icon;
		img.style.border = "0px";
		if (data.text) img.alt = img.title = data.text;
		link.appendChild(img);
		return parent.appendChild(link);
	}
	this.isEditable = function() { return false; };
}