function init_example() {
	var real = document.getElementById('example_code');
	if (!real) return;
	var code = real.innerHTML.replace(/x-script/gi, "script");
	// put the code
	var div = document.createElement("DIV");
	div.innerHTML = code;
	real.parentNode.insertBefore(div, real);
	real.parentNode.removeChild(real);
	// execute scripts
	for (var i = 0; i < div.childNodes.length; ++i)
		if (div.childNodes[i].nodeType == 1 && div.childNodes[i].nodeName == "SCRIPT")
			eval(div.childNodes[i].innerText);
	// show the code
	var show = document.getElementById('example_code_show');
	if (!show) return;
	show.innerHTML = "<pre style='margin:0px;padding:0px'><code style='margin:0px;padding:0px'>"+encode_html(code.trim())+"</code></pre>";
	colorize(show);
}
