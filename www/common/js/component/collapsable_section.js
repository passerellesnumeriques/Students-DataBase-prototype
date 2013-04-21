function collapsable_section(element) {
	if (typeof element == 'string') element = document.getElementById(element);
	// get the header and content, based on css class
	var header, content;
	for (var i = 0; i < element.childNodes.length; ++i)
		if (element.childNodes[i].className == 'collapsable_section_header')
			header = element.childNodes[i];
		else if (element.childNodes[i].className == 'collapsable_section_content')
			content = element.childNodes[i];
	// show or hide the content when user clicks on the header
	header.onclick = function() {
		if (content.style.visibility != "hidden") {
			content.style.visibility = "hidden";
			content.style.position = "absolute";
			content.style.top = "-10000px";
		} else {
			content.style.visibility = "visible";
			content.style.position = "static";
		}
	}
}