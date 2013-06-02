if (typeof add_javascript != 'undefined') {
	var url = get_script_path('date_picker.js');
	add_stylesheet(url+"date_picker.css");
	add_javascript(url+"../small_calendar/small_calendar.js");
	add_stylesheet(url+"../small_calendar/small_calendar.css");
}
function date_picker(max_year) {
	if (!max_year) max_year = new Date().getFullYear();
	var t = this;
	t.url = get_script_path('date_picker.js');
	t.element = document.createElement("DIV");
	t.element.className = 'date_picker';
	t.element.appendChild(t.header = document.createElement("DIV"));
	t.header.appendChild(t.daySelect = document.createElement("SELECT"));
	t.header.appendChild(t.monthSelect = document.createElement("SELECT"));
	t.header.appendChild(t.yearSelect = document.createElement("SELECT"));

	for (var i = 0; i < 12; ++i) {
		var o = document.createElement("OPTION");
		o.value = (i+1);
		switch (i) {
		case 0: o.innerHTML = locale.get_string("January"); break;
		case 1: o.innerHTML = locale.get_string("February"); break;
		case 2: o.innerHTML = locale.get_string("March"); break;
		case 3: o.innerHTML = locale.get_string("April"); break;
		case 4: o.innerHTML = locale.get_string("May"); break;
		case 5: o.innerHTML = locale.get_string("June"); break;
		case 6: o.innerHTML = locale.get_string("July"); break;
		case 7: o.innerHTML = locale.get_string("August"); break;
		case 8: o.innerHTML = locale.get_string("September"); break;
		case 9: o.innerHTML = locale.get_string("October"); break;
		case 10: o.innerHTML = locale.get_string("November"); break;
		case 11: o.innerHTML = locale.get_string("December"); break;
		}
		t.monthSelect.add(o);
	}
	for (var year = 1900; year <= max_year; ++year) {
		var o = document.createElement("OPTION");
		o.value = year;
		o.innerHTML = year;
		t.yearSelect.add(o);
	}
	for (var day = 1; day <= 28; ++day) {
		var o = document.createElement("OPTION");
		o.value = day;
		o.innerHTML = day;
		t.daySelect.add(o);
	}
	
	add_javascript(t.url+"../small_calendar/small_calendar.js",function() {
		t.cal = new small_calendar();
		if (t.date != null) t.cal.setDate(t.date);
		t.element.appendChild(t.cal.getElement());
		t.daySelect.onchange = function() {
			var date = t.cal.getDate();
			date.setDate(t.daySelect.selectedIndex+1);
			t.cal.setDate(date);
		};
		t.monthSelect.onchange = function() {
			var date = t.cal.getDate();
			date.setMonth(t.monthSelect.selectedIndex);
			t.cal.setDate(date);
		};
		t.yearSelect.onchange = function() {
			var date = t.cal.getDate();
			date.setFullYear(t.yearSelect.options[t.yearSelect.selectedIndex].value);
			t.cal.setDate(date);
		};

		t._date_changed = function(date) {
			for (var i = 0; i < t.yearSelect.options.length; ++i)
				if (t.yearSelect.options[i].value == date.getFullYear()) {
					t.yearSelect.selectedIndex = i;
					break;
				}
			t.monthSelect.selectedIndex = date.getMonth();
			var max_day = 28;
			do {
				var c = new Date();
				c.setFullYear(date.getFullYear());
				c.setMonth(date.getMonth());
				c.setDate(max_day+1);
				if (c.getDate() != max_day+1) break;
				max_day++;
			} while (true);
			while (t.daySelect.options.length > max_day)
				t.daySelect.remove(max_day);
			while (t.daySelect.options.length < max_day) {
				var o = document.createElement("OPTION");
				o.value = (t.daySelect.options.length+1);
				o.innerHTML = (t.daySelect.options.length+1);
				t.daySelect.add(o);
			}
			t.daySelect.selectedIndex = date.getDate()-1;
		};
		t.cal.onchange = function(cal) { 
			t._date_changed(cal.getDate());
			if (t.onchange)
				t.onchange(this, cal.getDate());
		};
		t._date_changed(new Date());

	});
	
	t.getElement = function() { return t.element; };
	t.setDate = function(date) { if (t.cal) t.cal.setDate(date); else t.date = date; };
}