function build_doc() {
	for (var i = 0; i < document.body.childNodes.length; ++i) {
		var e = document.body.childNodes[i];
		if (e.nodeType != 1) continue;
		if (e.nodeName == "H1") {
			
		} else if (e.nodeName == "H2") {
		}
	}
}

function encode_html(code) {
	return code.replace(/&/g, "&amp;").replace(/>/g, "&gt;").replace(/</g, "&lt;").replace(/"/g, "&quot;").replace(/\n/g, "<br/>");	
}

var highlighter_loaded = false;
var wait_for_highlighter = [];
add_javascript(get_script_path("doc.js")+"highlight.js/highlight.pack.js",function(){
	highlighter_loaded = true;
	for (var i = 0; i < wait_for_highlighter.length; ++i)
		colorize(wait_for_highlighter[i]);
});
add_stylesheet(get_script_path("doc.js")+"highlight.js/styles/googlecode.css");

function colorize(container) {
	if (!highlighter_loaded)
		wait_for_highlighter.push(container);
	else
		hljs.highlightBlock(container, "  ");
}

window.onload = build_doc;