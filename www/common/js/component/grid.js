function grid(element) {
	if (typeof element == 'string') element = document.getElementById(element);
	var t = this;
	t.element = element;
	t.columns = [];

	t.addColumn = function(title, width, field_type) {
		var th = document.createElement('TH');
		th.innerHTML = title;
		t.header.appendChild(th);
		var col = document.createElement('COL');
		if (width) col.width = width;
		t.colgroup.appendChild(col);
		if (!field_type) field_type = new field_text();
		t.columns.push(field_type);
	}
	
	t.setData = function(data) {
		// empty table
		while (t.table.childNodes.length > 0) t.table.removeChild(t.table.childNodes[0]);
		// create rows
		for (var i = 0; i < data.length; ++i) {
			var tr = document.createElement("TR");
			for (var j = 0; j < t.columns.length; ++j) {
				var td = document.createElement("TD");
				tr.appendChild(td);
				if (data[i].length <= j) continue;
				t.columns[j].create(td, data[i][j]);
			}
			t.table.appendChild(tr);
		}
	}
	
	t.reset = function() {
		// remove data rows
		while (t.table.childNodes.length > 0) t.table.removeChild(t.table.childNodes[0]);		
		// remove columns
		while (t.header.childNodes.length > 0) t.header.removeChild(t.header.childNodes[0]);		
		while (t.colgroup.childNodes.length > 0) t.colgroup.removeChild(t.colgroup.childNodes[0]);
		t.columns = [];
	}
	
	t.startLoading = function() {
		if (t.loading_back) return;
		t.loading_back = document.createElement("DIV");
		t.loading_back.style.backgroundColor = "#A0A0A0";
		setOpacity(t.loading_back, 0.35);
		t.loading_back.style.position = "absolute";
		t.loading_back.style.top = absoluteTop(t.element)+"px";
		t.loading_back.style.left = absoluteLeft(t.element)+"px";
		t.loading_back.style.width = t.element.offsetWidth+"px";
		t.loading_back.style.height = t.element.offsetHeight+"px";
		document.body.appendChild(t.loading_back);
		t.loading_icon = document.createElement("IMG");
		t.loading_icon.src = "/static/common/images/loading.gif";
		t.loading_icon.style.position = "absolute";
		t.loading_icon.style.top = (absoluteTop(t.element)+t.element.offsetHeight/2-8)+"px";
		t.loading_icon.style.left = (absoluteLeft(t.element)+t.element.offsetWidth/2-8)+"px";
		document.body.appendChild(t.loading_icon);
	}
	t.endLoading = function() {
		if (!t.loading_back) return;
		document.body.removeChild(t.loading_back);
		document.body.removeChild(t.loading_icon);
		t.loading_back = null;
		t.loading_icon = null;
	}
	
	/* --- internal functions --- */
	t._createTable = function() {
		t.form = document.createElement('FORM');
		var table = document.createElement('TABLE');
		t.form.appendChild(table);
		table.style.width = "100%";
		t.colgroup = document.createElement('COLGROUP');
		table.appendChild(t.colgroup);
		var thead = document.createElement('THEAD');
		t.header = document.createElement('TR');
		thead.appendChild(t.header);
		table.appendChild(thead);
		t.table = document.createElement('TBODY');
		table.appendChild(t.table);
		t.element.appendChild(t.form);
		table.className = "grid";
	}
	
	/* initialization */
	t._createTable();
}