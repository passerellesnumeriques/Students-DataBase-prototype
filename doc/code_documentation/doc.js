function build_doc() {
	for (var i = 0; i < document.body.childNodes.length; ++i) {
		var e = document.body.childNodes[i];
		if (e.nodeType != 1) continue;
		if (e.nodeName == "H1") {
			
		} else if (e.nodeName == "H2") {
		}
	}
}

window.onload = build_doc;