function calendar_view_week(container, cal) {
	if (typeof container == 'string') container = document.getElementById(container);
	var t=this;
	t.container = container;
	t.cal = cal;
	t.week_start = new Date();
	t.week_start.setHours(0);
	t.week_start.setMinutes(0);
	t.week_start.setSeconds(0);
	t.week_start.setMilliseconds(0);
	while (t.week_start.getDay() != 1) t.week_start.setDate(t.week_start.getDate()-1);
	t.nb_days = 7;
	t.week_end = new Date(t.week_start.getTime()+6*24*60*60*1000);
	t.week_end.setHours(23);
	t.week_end.setMinutes(59);
	t.week_end.setSeconds(59);
	t.week_end.setMilliseconds(999);
	// 1 visible unit = 20 pixels
	// zoom corresponds to the size of an hour
	t.zoom = 40; // 2 units, meaning we will see every 30 minutes
	t._timed_events = [];
	
	t.setNbDays = function(nb) {
		t.nb_days = nb;
		t.week_end = new Date(t.week_start.getTime()+(nb-1)*24*60*60*1000);
		t.week_end.setHours(23);
		t.week_end.setMinutes(59);
		t.week_end.setSeconds(59);
		t.week_end.setMilliseconds(999);
	};
	t.zoom_plus = function() {
		t.zoom *= 2;
	};
	t.zoom_minus = function() {
		if (t.zoom == 5) return;
		t.zoom /= 2;
	};
	t.move_days = function(days) {
		t.week_start.setTime(t.week_start.getTime()+days*24*60*60*1000);
		t.week_end.setTime(t.week_end.getTime()+days*24*60*60*1000);
	};
	
	t._2digits = function(i) {
		if (i < 10) return "0"+i;
		return ""+i;
	}
	t._createView = function() {
		while (t.container.childNodes.length > 0) t.container.removeChild(t.container.childNodes[0]);
		t._days_titles = [];
		t._time_titles = [];
		t._delimiters = [];
		t._content = document.createElement("DIV");
		t._content.style.overflowY = 'scroll';
		t._content.style.position = 'absolute';
		t.container.appendChild(t._content);
		for (var i = 0; i < t.nb_days; ++i) {
			var div = document.createElement("DIV");
			div.className = 'calendar_view_week_day_title';
			div.style.position = 'absolute';
			div.style.border = "1px solid black";
			if (i != 0) div.style.borderLeft = "0px";
			var day;
			switch (new Date(t.week_start.getTime()+i*24*60*60*1000).getDay()) {
			case 0: day = locale.get_string("Sunday"); break;
			case 1: day = locale.get_string("Monday"); break;
			case 2: day = locale.get_string("Tuesday"); break;
			case 3: day = locale.get_string("Wednesday"); break;
			case 4: day = locale.get_string("Thursday"); break;
			case 5: day = locale.get_string("Friday"); break;
			case 6: day = locale.get_string("Saturday"); break;
			}
			var date = new Date(t.week_start.getTime()+i*24*60*60*1000);
			div.innerHTML = day+"<br/>"+t._2digits(date.getDate())+"/"+t._2digits(date.getMonth()+1)+"/"+date.getFullYear();
			t._days_titles.push(div);
			t.container.appendChild(div);
			t._delimiters.push([]);
		}
		var nb_times = t.zoom/20*24;
		var date = new Date();
		date.setHours(0); date.setMinutes(0); date.setSeconds(0); date.setMilliseconds(0);
		for (var i = 0; i < nb_times; ++i) {
			var div = document.createElement("DIV");
			div.className = 'calendar_view_week_hour_title';
			div.style.position = 'absolute';
			div.style.left = "0px";
			div.style.height = i > 0 ? "20px" : "19px";
			div.style.top = (i*20)+"px";
			div.style.border = "1px solid black";
			if (i > 0) div.style.borderTop = "0px";
			div.innerHTML = t._2digits(date.getHours())+":"+t._2digits(date.getMinutes());
			date.setTime(date.getTime()+24*60*60*1000/nb_times);
			t._content.appendChild(div);
			t._time_titles.push(div);
			for (var j = 0; j < t.nb_days; ++j) {
				div = document.createElement("DIV");
				div.style.position = 'absolute';
				div.style.top = (i*20)+"px";
				div.style.height = "20px";
				div.style.borderRight = "1px solid #808080";
				div.style.borderBottom = "1px dotted #808080";
				t._content.appendChild(div);
				t._delimiters[j].push(div);
			}
		}
	};
	t.layout = function() {
		var w = t.container.offsetWidth;
		var content_w = w;
		t._content.style.width = w+"px";
		w = t._content.clientWidth;
		var h = t.container.offsetHeight;
		// layout time titles
		var x = 0;
		for (var i = 0; i < t._time_titles.length; ++i) {
			var div = t._time_titles[i];
			if (div.offsetWidth > x) x = div.offsetWidth;
		}
		t._time_title_width = x;
		x--;
		
		// layout day titles
		var day_w = Math.floor((w-1-x)/t.nb_days);
		while (t._content.clientWidth > x+day_w*t.nb_days+1)
			t._content.style.width = (--content_w)+"px";
		var y = 0;
		for (var i = 0; i < t.nb_days; ++i) {
			t._days_titles[i].style.top = "0px";
			t._days_titles[i].style.left = (x+i*day_w)+"px";
			t._days_titles[i].style.width = (day_w-(i==0?1:0))+"px";
			t._days_titles[i].style.height = "";
			if (t._days_titles[i].offsetHeight > y) y = t._days_titles[i].offsetHeight; 
		}
		for (var i = 0; i < t.nb_days; ++i)
			t._days_titles[i].style.height = y+"px";
		y += 1; // border
		t._content.style.top = y+"px";
		t._content.style.height = (h-y)+"px";
		t._day_width = day_w;
		
		// layout cells
		for (var i = 0; i < t.nb_days; ++i) {
			var cells = t._delimiters[i];
			for (var j = 0; j < cells.length; ++j) {
				var div = cells[j];
				div.style.left = (x+i*day_w)+"px";
				div.style.width = day_w+"px";
			}
		}
		
		// layout timed events
		for (var i = 0; i < t._timed_events.length; ++i)
			t._timed_events[i].div.style.visibility = 'hidden';
		for (var i = 0; i < t._timed_events.length; ++i)
			t._timed_events[i].layout();
	};
	t._init_cal = function() {
		cal.on_new_timed_event = function(ev) {
			ev.color = ev.calendar.color;
			t.add_timed_event(ev);
		};
		cal.on_timed_event_changed = function(ev) {
			// TODO
		};
		cal.on_timed_event_removed = function(ev) {
			// TODO
		};
		var ev = {
			uid: 1,
			start: new Date(t.week_start.getTime()),
			end: new Date(t.week_start.getTime()),
			title: "Test",
			color: "#A0A0FF"
		};
		ev.start.setHours(9);
		ev.end.setHours(10); ev.end.setMinutes(30);
		t.add_timed_event(ev);
		ev = {
			uid: 2,
			start: new Date(t.week_start.getTime()),
			end: new Date(t.week_start.getTime()),
			title: "Test 2",
			color: "#A0A0FF"
		};
		ev.start.setHours(8);
		ev.end.setHours(12);
		t.add_timed_event(ev);
	};
	t.add_timed_event = function(event) {
		if (event.start.getDate() != event.end.getDate()) {
			// need to be split
			var ev1 = object_copy(event, true);
			var ev2 = object_copy(event, true);
			ev1.end.setDate(ev1.start.getDate());
			ev1.end.setHours(23);
			ev1.end.setMinutes(59);
			ev1.end.setSeconds(59);
			ev1.end.setMilliseconds(999);
			ev2.start.setDate(ev2.end.getDate());
			ev2.start.setHours(0);
			ev2.start.setMinutes(0);
			ev2.start.setSeconds(0);
			ev2.start.setMilliseconds(0);
			t.add_timed_event(ev1);
			t.add_timed_event(ev2);
			return;
		}
		event.div = document.createElement("DIV");
		event.div.style.backgroundColor = event.color;
		event.div.className = "calendar_view_week_timed_event";
		event.div.style.position = 'absolute';
		event.div.style.visibility = 'hidden';
		event.div.style.overflow = 'hidden';
		event.div.innerHTML = "<div class='calendar_view_week_timed_event_time'>"+t._2digits(event.start.getHours())+":"+t._2digits(event.start.getMinutes())+"-"+t._2digits(event.end.getHours())+":"+t._2digits(event.end.getMinutes())+"</div>"+event.title;
		event.div.data = event;
		event.layout = function(tentative) {
			if (this.end.getTime() < t.week_start) { this.div.style.visibility = 'hidden'; return; }
			if (this.start.getTime() > t.week_end) { this.div.style.visibility = 'hidden'; return; }
			if (this.div.parentNode != t._content)
				t._content.appendChild(this.div);
			this.div.style.visibility = 'visible';
			var day = this.start.getDate()-t.week_start.getDate();
			var time = this.start.getHours()*60+this.start.getMinutes();
			var y = Math.round(time*t.zoom/60);
			time = (this.end.getTime()-this.start.getTime())/60000;
			var h = Math.round(time*t.zoom/60-2);
			this.div.style.top = y+"px";
			this.div.style.height = h+"px";
			
			var x = (t._time_title_width+day*t._day_width);
			var w = (t._day_width-2);
			
			// check if other events already there
			var conflicts = [];
			for (var i = 0; i < t._content.childNodes.length; ++i) {
				var e = t._content.childNodes[i];
				if (e.nodeType != 1) continue;
				if (e.className != "calendar_view_week_timed_event") continue;
				if (e.style.visibility != 'visible') continue;
				if (e == this.div) continue;
				if (e.offsetLeft >= x && e.offsetLeft < x+w) {
					if (e.offsetTop < y) {
						// start before
						if (e.offsetTop+e.offsetHeight > y)
							conflicts.push(e); // end during or after
					} else if (e.offsetTop >= y && e.offsetTop < y+h)
						conflicts.push(e); // start during
				}
			}
			this.conflicts = conflicts;
			if (conflicts.length > 0) {
				// check if we have a space available
				var avail = [[x,w]];
				for (var i = 0; i < conflicts.length; ++i) {
					var cx = conflicts[i].offsetLeft;
					var cw = conflicts[i].offsetWidth;
					for (var j = 0; j < avail.length; ++j) {
						if (cx+cw-1<avail[j][0]) continue; // before
						if (cx>=avail[j][0]+avail[j][1]) continue; // after
						if (cx < avail[j][0]) {
							cw = cx+cw-avail[j][0];
							cx = avail[j][0];
						}
						if (cw <= 0) continue;
						if (cx+cw > avail[j][0]+avail[j][1])
							cw = avail[j][0]+avail[j][1]-cx;
						if (cw <= 0) continue;
						if (cx == avail[j][0]) {
							avail[j][0]+=cw;
							avail[j][1]-=cw;
							if (avail[j][1] <= 0) {
								avail.splice(j,1);
								j--;
							}
						} else {
							var a1 = [avail[j][0],cx-avail[j][0]];
							var a2 = [cx+cw,avail[j][0]+avail[j][1]-(cx+cw)];
							if (a1[1] <= 0) {
								if (a2[1] <= 0) {
									avail.splice(j,1);
									j--;
									continue;
								} else
									avail[j] = a2;
							} else {
								avail[j] = a1;
								if (a2[1] > 0)
									avail.splice(j+1,0,a2);
							}
						}
					}
				}
				if (avail.length > 0) {
					// space available: take the first one
					x = avail[0][0];
					w = avail[0][1];
				} else {
					// no available space: need to re-organize
					for (var i = 0; i < conflicts.length; ++i)
						conflicts[i].data.relayout_conflict(this.div);
					// try again
					if (!tentative) tentative = 0;
					if (tentative < 5) {
						this.layout(tentative+1);
						return;
					} else {
						// not possible !
						x += 10;
						w -= 10;
					}
				}
			}
			
			this.div.style.left = x+"px";
			this.div.style.width = w+"px";
		};
		event.relayout_conflict = function(new_conflict) {
			// difficult case with multiple conflicts: let just reduce the width, so it will probably let some space for new events
			if (!this.conflicts.contains(new_conflict)) {
				this.div.style.width = Math.floor(this.div.offsetWidth*(this.conflicts.length+1)/(this.conflicts.length+2))+"px";
				this.conflicts.push(new_conflict);
			} else {
				// this seems to be the second time !
				if (this.div.offsetWidth > 1)
					this.div.style.width = Math.floor(this.div.offsetWidth/2)+"px";
				else
					this.div.style.width = "0px"; // we cannot show it anymore!!!
			}
		};
		t._timed_events.push(event);
		event.layout();
	};
	t.reset = function() {
		t._createView();
		t.layout();
	};
	t.reset();
	addLayoutEvent(container, function(){t.layout();});
	t._init_cal();
}