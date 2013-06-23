function Calendar() {
	var t=this;
	
	t.onerror = null;
	t.onloading = null;
	t.onloaded = null;
	t.onaction = null;
	t.onactiondone = null;
	
	t.on_new_timed_event = null;
	t.on_timed_event_changed = null;
	t.on_timed_event_removed = null;
	t.on_new_range_event = null;
	t.on_range_event_changed = null;
	t.on_range_event_removed = null;
	t.on_new_recurring_timed_event = null;
	t.on_recurring_timed_event_changed = null;
	t.on_recurring_timed_event_removed = null;
	t.on_new_recurring_range_event = null;
	t.on_recurring_range_event_changed = null;
	t.on_recurring_range_event_removed = null;
	
	t._calendars = [];
	t._timedEvents = [];
	t._recurringTimedEvents = [];
	t._rangeEvents = [];
	t._recurringRangeEvents = [];
	
	t.add_calendar = function(name,url,color) {
		var cal = new URLCalendar(name,url,color);
		t._calendars.push(cal);
		t.refresh_calendar(cal);
	};
	
	t.refresh_calendars = function() {
		for (var i = 0; i < t._calendars.length; ++i)
			t.refresh_calendar(t._calendars[i]);
	};
	
	t.refresh_calendar = function(cal) {
		if (typeof cal == 'string') {
			for (var i = 0; i < t._calendars.length; ++i)
				if (t._calendars[i].url == cal) { cal = t._calendars[i]; break;}
			if (typeof cal == 'string') return; // unknown calendar url
		}
		if (t.onloading) t.onloading(t, cal);
		ajax.call("GET",cal.url,null,null,
			function(error){
				if (t.onerror) t.onerror(t, cal, error);
				else error_dialog(error);
			},function(xhr){
				var output;
				var error = null;
				try { 
					output = eval('('+xhr.responseText+')'); 
					if (!output) error = "Invalid output:<br/>"+xhr.responseText;
					else if (output.errors) {
						error = "";
						for (var i = 0; i < output.errors.length; ++i)
							error += output.errors[i]+"<br/>";
					} else if (!output.result) error = "Invalid output:<br/>"+xhr.responseText;
				}
				catch (e) {
					error = "Invalid output: "+e+"<br/>"+xhr.responseText;
				}
				if (error) {
					if (t.onerror) t.onerror(t, cal, error);
					else error_dialog(error);
					return;
				}
				// mark all existing as removed
				for (var j = 0; j < t._timedEvents.length; j++)
					if (t._timedEvents[j].calendar == cal)
						t._timedEvents[j].removed = true;
				for (var j = 0; j < t._rangeEvents.length; j++)
					if (t._rangeEvents[j].calendar == cal)
						t._rangeEvents[j].removed = true;
				for (var j = 0; j < t._recurringTimedEvents.length; j++)
					if (t._recurringTimedEvents[j].calendar == cal)
						t._recurringTimedEvents[j].removed = true;
				for (var j = 0; j < t._recurringRangeEvents.length; j++)
					if (t._recurringRangeEvents[j].calendar == cal)
						t._recurringRangeEvents[j].removed = true;
				
				var events = output.result.events;
				for (var i = 0; i < events.length; ++i) {
					var ev = events[i];
					var list = null;
					var listener_changed = null, listener_added = null;
					if (typeof ev.start["hour"] == 'undefined') {
						// this is a range event
						if (typeof ev.freq == 'undefined') {
							ev = new Calendar_RangeEvent(cal, ev.uid,ev.start,ev.end,ev.modified,ev.title,ev.description,ev.location,ev.organizer,ev.attendees);
							list = t._rangeEvents;
							listener_changed = t.on_range_event_changed;
							listener_added = t.on_new_range_event;
						} else {
							ev = new Calendar_RecurringRangeEvent(cal, ev.uid,ev.start,ev.end,ev.modified,ev.title,ev.description,ev.location,ev.organizer,ev.attendees, ev.freq);
							list = t._recurringRangeEvents;
							listener_changed = t.on_recurring_range_event_changed;
							listener_added = t.on_new_recurring_range_event;
						}
					} else {
						// this is a timed event
						if (typeof ev.freq == 'undefined') {
							ev = new Calendar_TimedEvent(cal, ev.uid,ev.start,ev.end,ev.modified,ev.title,ev.description,ev.location,ev.organizer,ev.attendees);
							list = t._timedEvents;
							listener_changed = t.on_timed_event_changed;
							listener_added = t.on_new_timed_event;
						} else {
							ev = new Calendar_RecurringTimedEvent(cal, ev.uid,ev.start,ev.end,ev.modified,ev.title,ev.description,ev.location,ev.organizer,ev.attendees, ev.freq);
							list = t._recurringTimedEvents;
							listener_changed = t.on_recurring_timed_event_changed;
							listener_added = t.on_new_recurring_timed_event;
						}
					}
					var found = false;
					for (var j = 0; j < list.length; j++) {
						if (list[j].calendar == cal && list[j].uid == ev.uid) {
							list[j].removed = false;
							if (!list[j].modifed || !ev.modified || list[j].modifed < ev.modified) {
								// update
								list[j] = ev;
								if (listener_changed) listener_changed(ev);
								found = true;
							} else {
								// no change
								found = true;
								break;
							}
						}
					}
					if (found) continue;
					list.push(ev);
					if (listener_added) listener_added(ev);
				}
				// check what has been removed
				for (var j = 0; j < t._timedEvents.length; j++)
					if (t._timedEvents[j].calendar == cal)
						if (t._timedEvents[j].removed) {
							if (t.on_timed_event_removed) t.on_timed_event_removed(t._timedEvents[j]);
							t._timedEvents.splice(j,1);
							j--;
							continue;
						}
				for (var j = 0; j < t._rangeEvents.length; j++)
					if (t._rangeEvents[j].calendar == cal)
						if (t._rangeEvents[j].removed) {
							if (t.on_range_event_removed) t.on_range_event_removed(t._rangeEvents[j]);
							t._rangeEvents.splice(j,1);
							j--;
							continue;
						}
				for (var j = 0; j < t._recurringTimedEvents.length; j++)
					if (t._recurringTimedEvents[j].calendar == cal)
						if (t._recurringTimedEvents[j].removed) {
							if (t.on_recurring_timed_event_removed) t.on_recurring_timed_event_removed(t._recurringTimedEvents[j]);
							t._recurringTimedEvents.splice(j,1);
							j--;
							continue;
						}
				for (var j = 0; j < t._recurringRangeEvents.length; j++)
					if (t._recurringRangeEvents[j].calendar == cal)
						if (t._recurringRangeEvents[j].removed) {
							if (t.on_recurring_range_event_removed) t.on_recurring_range_event_removed(t._recurringRangeEvents[j]);
							t._recurringRangeEvents.splice(j,1);
							j--;
							continue;
						}
				
				if (t.onloaded) t.onloaded(t, cal);
				
				if (output.result.action) {
					var id = generate_id();
					if (t.onaction) t.onaction(id, output.result.action.message);
					ajax.call("GET",output.result.action.url,null,null,
						function(error){
							if (t.onactiondone) t.onactiondone(id);
							if (t.onerror) t.onerror(t, cal, error);
							else error_dialog(error);
						},function(xhr){
							if (t.onactiondone) t.onactiondone(id);
							var error = null;
							try { 
								output = eval('('+xhr.responseText+')'); 
								if (!output) error = "Invalid output:<br/>"+xhr.responseText;
								else if (output.errors) {
									error = "";
									for (var i = 0; i < output.errors.length; ++i)
										error += output.errors[i]+"<br/>";
								} else if (!output.result) error = "Invalid output:<br/>"+xhr.responseText;
							}
							catch (e) {
								error = "Invalid output: "+e+"<br/>"+xhr.responseText;
							}
							if (error) {
								if (t.onerror) t.onerror(t, cal, error);
								else error_dialog(error);
								return;
							}
							t.refresh_calendar(cal);
						}
					);
				}
			}
		);
	};
	
	setInterval(function(){t.refresh_calendars();},60000); // refresh every minute
}

function URLCalendar(name,url,color) {
	this.name = name;
	this.url = url;
	if (!color) color = "#A0A0FF";
	this.color = color;
}
function Calendar_Date(date) {
	if (date instanceof Date) return date;
	if (date == null) return new Date();
	var d = new Date();
	d.setUTCFullYear(parseInt(date["year"]));
	d.setUTCMonth(parseInt(date["month"])-1);
	d.setUTCDate(parseInt(date["day"]));
	if (typeof date["hour"] != 'undefined')	d.setUTCHours(parseInt(date["hour"])); else d.setUTCHours(0);
	if (typeof date["minute"] != 'undefined')	d.setUTCMinutes(parseInt(date["minute"])); else d.setUTCMinutes(0);
	if (typeof date["second"] != 'undefined')	d.setUTCSeconds(parseInt(date["second"])); else d.setSeconds(0);
	d.setUTCMilliseconds(0);
	return d;
}
function Calendar_Event(calendar,uid,start,end,modified,title,description,location,organizer,attendees) {
	this.calendar = calendar;
	this.uid = uid;
	this.title = title;
	this.description = description;
	this.start = Calendar_Date(start);
	this.end = Calendar_Date(end);
	this.modified = modified;
	this.location = location;
	this.organizer = organizer;
	this.attendees = attendees;
	
	this.show = function() {
		var t=this;
		locale.load("/locale/calendar/","calendar");
		add_javascript("/static/common/js/popup_window/popup_window.js",function(){
			var container = document.createElement("DIV");
			container.className = 'calendar_event_details';
			var title = document.createElement("H1");
			title.innerHTML = t.title;
			container.appendChild(title);
			var table = document.createElement("TABLE");

			var tr = document.createElement("TR"); table.appendChild(tr);
			var td = document.createElement("TD"); tr.appendChild(td);
			td.innerHTML = locale.get_string("calendar:When");
			td = document.createElement("TD"); tr.appendChild(td);
			// TODO when
			
			tr = document.createElement("TR"); table.appendChild(tr);
			td = document.createElement("TD"); tr.appendChild(td);
			td.innerHTML = locale.get_string("calendar:Where");
			td = document.createElement("TD"); tr.appendChild(td);
			td.innerHTML = t.location;
			
			tr = document.createElement("TR"); table.appendChild(tr);
			td = document.createElement("TD"); tr.appendChild(td);
			td.innerHTML = locale.get_string("calendar:Description");
			td = document.createElement("TD"); tr.appendChild(td);
			td.innerHTML = t.description;
			
			tr = document.createElement("TR"); table.appendChild(tr);
			td = document.createElement("TD"); tr.appendChild(td);
			td.innerHTML = locale.get_string("calendar:Organizer");
			td = document.createElement("TD"); tr.appendChild(td);
			td.innerHTML = t.organizer;
			
			tr = document.createElement("TR"); table.appendChild(tr);
			td = document.createElement("TD"); tr.appendChild(td);
			td.innerHTML = locale.get_string("calendar:Attendees");
			td = document.createElement("TD"); tr.appendChild(td);
			if (t.attendees) {
				var at = document.createElement("TABLE");
				td.appendChild(at);
				
				// chairman
				tr = document.createElement("TR"); at.appendChild(tr);
				td = document.createElement("TH"); td.colSpan = 2; tr.appendChild(td);
				td.innerHTML = locale.get_string("calendar:Chairman");
				for (var i = 0; i < t.attendees.length; ++i)
					if (t.attendees[i][1] == "CHAIRMAN") {
						tr = document.createElement("TR"); at.appendChild(tr);
						td = document.createElement("TD"); tr.appendChild(td);
						td.innerHTML = t.attendees[i][0];
						td = document.createElement("TD"); tr.appendChild(td);
						switch (t.attendees[i][2]) {
						case "YES": td.innerHTML = locale.get_string("Yes"); break;
						case "NO": td.innerHTML = locale.get_string("No"); break;
						case "TENTATIVE": td.innerHTML = locale.get_string("calendar:Tentative"); break;
						case "DELEGATE": td.innerHTML = locale.get_string("calendar:Delegated"); break;
						case "UNKNOWN": td.innerHTML = locale.get_string("calendar:Wait answer"); break;
						default: td.innerHTML = "?"; break;
						}
					}
				// requested
				tr = document.createElement("TR"); at.appendChild(tr);
				td = document.createElement("TH"); td.colSpan = 2; tr.appendChild(td);
				td.innerHTML = locale.get_string("calendar:Requested");
				for (var i = 0; i < t.attendees.length; ++i)
					if (t.attendees[i][1] == "REQUESTED") {
						tr = document.createElement("TR"); at.appendChild(tr);
						td = document.createElement("TD"); tr.appendChild(td);
						td.innerHTML = t.attendees[i][0];
						td = document.createElement("TD"); tr.appendChild(td);
						switch (t.attendees[i][2]) {
						case "YES": td.innerHTML = locale.get_string("Yes"); break;
						case "NO": td.innerHTML = locale.get_string("No"); break;
						case "TENTATIVE": td.innerHTML = locale.get_string("calendar:Tentative"); break;
						case "DELEGATE": td.innerHTML = locale.get_string("calendar:Delegated"); break;
						case "UNKNOWN": td.innerHTML = locale.get_string("calendar:Wait answer"); break;
						default: td.innerHTML = "?"; break;
						}
					}
				// optional
				tr = document.createElement("TR"); at.appendChild(tr);
				td = document.createElement("TH"); td.colSpan = 2; tr.appendChild(td);
				td.innerHTML = locale.get_string("calendar:Optional");
				for (var i = 0; i < t.attendees.length; ++i)
					if (t.attendees[i][1] == "OPTIONAL") {
						tr = document.createElement("TR"); at.appendChild(tr);
						td = document.createElement("TD"); tr.appendChild(td);
						td.innerHTML = t.attendees[i][0];
						td = document.createElement("TD"); tr.appendChild(td);
						switch (t.attendees[i][2]) {
						case "YES": td.innerHTML = locale.get_string("Yes"); break;
						case "NO": td.innerHTML = locale.get_string("No"); break;
						case "TENTATIVE": td.innerHTML = locale.get_string("calendar:Tentative"); break;
						case "DELEGATE": td.innerHTML = locale.get_string("calendar:Delegated"); break;
						case "UNKNOWN": td.innerHTML = locale.get_string("calendar:Wait answer"); break;
						default: td.innerHTML = "?"; break;
						}
					}
				// for info
				tr = document.createElement("TR"); at.appendChild(tr);
				td = document.createElement("TH"); td.colSpan = 2; tr.appendChild(td);
				td.innerHTML = locale.get_string("calendar:For information");
				for (var i = 0; i < t.attendees.length; ++i)
					if (t.attendees[i][1] == "FOR_INFO") {
						tr = document.createElement("TR"); at.appendChild(tr);
						td = document.createElement("TD"); tr.appendChild(td);
						td.innerHTML = t.attendees[i][0];
						td = document.createElement("TD"); tr.appendChild(td);
						switch (t.attendees[i][2]) {
						case "YES": td.innerHTML = locale.get_string("Yes"); break;
						case "NO": td.innerHTML = locale.get_string("No"); break;
						case "TENTATIVE": td.innerHTML = locale.get_string("calendar:Tentative"); break;
						case "DELEGATE": td.innerHTML = locale.get_string("calendar:Delegated"); break;
						case "UNKNOWN": td.innerHTML = locale.get_string("calendar:Wait answer"); break;
						default: td.innerHTML = "?"; break;
						}
					}
			}
		
			container.appendChild(table);
			
			var p = new popup_window(t.title, "/static/calendar/event_16.png", container);
			p.show();
		});
	};
}
function Calendar_TimedEvent(calendar,uid,start,end,modified,title,description,location,organizer,attendees) {
	Calendar_Event.call(this, calendar,uid,start,end,modified,title,description,location,organizer,attendees);
}
Calendar_TimedEvent.prototype = new Calendar_Event();
Calendar_TimedEvent.prototype.constructor = Calendar_TimedEvent;
function Calendar_RangeEvent(calendar,uid,start,end,modified,title,description,location,organizer,attendees) {
	Calendar_Event.call(this, calendar,uid,start,end,modified,title,description,location,organizer,attendees);
}
Calendar_RangeEvent.prototype = new Calendar_Event();
Calendar_RangeEvent.prototype.constructor = Calendar_RangeEvent;
function Calendar_RecurringEvent(calendar,uid,start,end,modified,title,description,location,organizer,attendees, freq, event_type) {
	Calendar_Event.call(this, calendar,uid,start,end,modified,title,description,location,organizer,attendees);
	this.event_type = event_type;
	if (freq != null) {
		this.freq = freq[0];
		this.freq_interval = freq[1];
		this.freq_valid_months = freq[2];
		this.freq_valid_hours = freq[3];
		this.freq_valid_week_days = freq[4];
		this.freq_valid_month_days = freq[5];
		this.freq_valid_year_days = freq[6];
		this.freq_valid_weeks = freq[7];
		this.freq_until = null;
		if (freq[8]) {
			var s = freq[8];
			var i = s.indexOf('-');
			var year = parseInt(s.substring(0,i));
			s = s.substring(i+1);
			i = s.indexOf('-');
			var month = parseInt(s.substring(0,i));
			s = s.substring(i+1);
			var day = parseInt(s);
			var hour = 23;
			var minute = 59;
			var second = 59;
			if (freq[9]) {
				s = freq[9];
				i = s.indexOf(':');
				hour = parseInt(s.substring(0,i));
				s = s.substring(i+1);
				i = s.indexOf(':');
				minute = parseInt(s.substring(0,i));
				s = s.substring(i+1);
				second = parseInt(s);
			}
			this.freq_until = new Date();
			this.freq_until.setUTCFullYear(year);
			this.freq_until.setUTCMonth(month-1);
			this.freq_until.setUTCDate(day);
			this.freq_until.setUTCHours(hour);
			this.freq_until.setUTCMinutes(minute);
			this.freq_until.setUTCSeconds(second);
			this.freq_until.setUTCMilliseconds(0);
		}
		this.freq_count = freq[10];
		this.freq_wkst = freq[11];
	}
	
	this.getEventsBetween = function(d1, d2) {
		if (this.start.getTime() > d2.getTime()) return []; // everything later than d2
		var d = new Date(this.start.getTime());
		var count = 0;
		var events = [];
		
		if (this.freq == "WEEKLY") {
			var ws;
			if (this.freq_wkst) ws = parse_week_day(this.freq_wkst); else ws = 0;
			// first week
			do {
				if (this.freq_count != null && count >= this.freq_count) return events; // reach count
				if (this.freq_until != null && this.freq_until.getTime() < d.getTime()) return events; // reach until
				if (d.getTime() > d2.getTime()) return events; // reach requested interval end
				if (this.is_recurring_date_compliant(d)) {
					if (d.getTime() >= d1.getTime())
						events.push(new this.event_type(this.calendar, this.uid, this._getStart(d), this._getEnd(d), this.modified, this.title, this.description, this.location, this.organizer, this.attendees));
					count++;
				}
				// next day of first week
				d.setDate(d.getDate()+1);
			} while (d.getDay() != ws);
			// all following weeks
			do {
				if (this.freq_interval > 1) d.setDate(d.getDate()+(this.freq_interval-1)*7); // move forward according to interval
				do {
					if (this.freq_count != null && count >= this.freq_count) return events; // reach count
					if (this.freq_until != null && this.freq_until.getTime() < d.getTime()) return events; // reach until
					if (d.getTime() > d2.getTime()) return events; // reach requested interval end
					if (this.is_recurring_date_compliant(d)) {
						if (d.getTime() >= d1.getTime())
							events.push(new this.event_type(this.calendar, this.uid, this._getStart(d), this._getEnd(d), this.modified, this.title, this.description, this.location, this.organizer, this.attendees));
						count++;
					}
					// next day of the week
					d.setDate(d.getDate()+1);
				} while (d.getDay() != ws);
			} while (true);
		} else if (this.freq == "MONTHLY") {
			var nb_in_month = 0;
			if (this.freq_valid_month_days) nb_in_month += this.freq_valid_month_days.length;
			if (this.freq_valid_week_days) nb_in_month += this.freq_valid_week_days.length;
			if (nb_in_month == 0) nb_in_month = 1;
			// first month
			var nb = 0;
			do {
				if (this.freq_count != null && count >= this.freq_count) return events; // reach count
				if (this.freq_until != null && this.freq_until.getTime() < d.getTime()) return events; // reach until
				if (d.getTime() > d2.getTime()) return events; // reach requested interval end
				if (this.is_recurring_date_compliant(d)) {
					if (d.getTime() >= d1.getTime())
						events.push(new this.event_type(this.calendar, this.uid, this._getStart(d), this._getEnd(d), this.modified, this.title, this.description, this.location, this.organizer, this.attendees));
					count++;
					if (++nb == nb_in_month) {
						if (nb_in_month == 1) d.setDate(this.start.getDate()); else d.setDate(1);
						d.setMonth(d.getMonth()+1);
						break;
					}
				}
				// next day of first month
				d.setDate(d.getDate()+1);
			} while (d.getDate() != 1);
			// all following months
			do {
				if (this.freq_interval > 1) d.setMonth(d.getMonth()+(this.freq_interval-1)); // move forward according to interval
				nb = 0;
				do {
					if (this.freq_count != null && count >= this.freq_count) return events; // reach count
					if (this.freq_until != null && this.freq_until.getTime() < d.getTime()) return events; // reach until
					if (d.getTime() > d2.getTime()) return events; // reach requested interval end
					if (this.is_recurring_date_compliant(d)) {
						if (d.getTime() >= d1.getTime())
							events.push(new this.event_type(this.calendar, this.uid, this._getStart(d), this._getEnd(d), this.modified, this.title, this.description, this.location, this.organizer, this.attendees));
						count++;
						if (++nb == nb_in_month) {
							if (nb_in_month == 1) d.setDate(this.start.getDate()); else d.setDate(1);
							d.setMonth(d.getMonth()+1);
							break;
						}
					}
					// next day of the month
					d.setDate(d.getDate()+1);
				} while (d.getDate() != 1);
			} while (true);
		} else if (this.freq == "YEARLY") {
			var nb_in_year = 0;
			if (this.freq_valid_months) nb_in_year += this.freq_valid_months.length;
			if (this.freq_valid_year_days) nb_in_year += this.freq_valid_year_days.length;
			if (this.freq_valid_week_days) nb_in_year += this.freq_valid_week_days.length;
			if (nb_in_year == 0) nb_in_year = 1;
			// first year
			var nb = 0;
			do {
				if (this.freq_count != null && count >= this.freq_count) return events; // reach count
				if (this.freq_until != null && this.freq_until.getTime() < d.getTime()) return events; // reach until
				if (d.getTime() > d2.getTime()) return events; // reach requested interval end
				if (this.is_recurring_date_compliant(d)) {
					if (d.getTime() >= d1.getTime())
						events.push(new this.event_type(this.calendar, this.uid, this._getStart(d), this._getEnd(d), this.modified, this.title, this.description, this.location, this.organizer, this.attendees));
					count++;
					if (++nb == nb_in_year) {
						if (nb_in_year == 1) {
							d.setDate(this.start.getDate());
							d.setMonth(this.start.getMonth());
						} else {
							d.setDate(1);
							d.setMonth(0);
						}
						d.setFullYear(d.getFullYear()+1);
						break;
					}
				}
				// next day of first year
				d.setDate(d.getDate()+1);
			} while (d.getDate() != 1 && d.getMonth() != 0);
			// all following years
			do {
				if (this.freq_interval > 1) d.setFullYear(d.getFullYear()+(this.freq_interval-1)); // move forward according to interval
				nb = 0;
				do {
					if (this.freq_count != null && count >= this.freq_count) return events; // reach count
					if (this.freq_until != null && this.freq_until.getTime() < d.getTime()) return events; // reach until
					if (d.getTime() > d2.getTime()) return events; // reach requested interval end
					if (this.is_recurring_date_compliant(d)) {
						if (d.getTime() >= d1.getTime())
							events.push(new this.event_type(this.calendar, this.uid, this._getStart(d), this._getEnd(d), this.modified, this.title, this.description, this.location, this.organizer, this.attendees));
						count++;
						if (++nb == nb_in_year) {
							if (nb_in_year == 1) {
								d.setDate(this.start.getDate());
								d.setMonth(this.start.getMonth());
							} else {
								d.setDate(1);
								d.setMonth(0);
							}
							d.setFullYear(d.getFullYear()+1);
							break;
						}
					}
					// next day of the year
					d.setDate(d.getDate()+1);
				} while (d.getDate() != 1 && d.getMonth() != 0);
			} while (true);
		} else {
			while (d.getTime() < d2.getTime() && (this.freq_until == null || this.freq_until.getTime() >= d.getTime()) && (this.freq_count == null || this.freq_count <= count)) {
				if (this.freq == "HOURLY") {
					if (this.is_recurring_date_compliant(d)) {
						if (d.getTime() >= d1.getTime())
							events.push(new this.event_type(this.calendar, this.uid, this._getStart(d), this._getEnd(d), this.modified, this.title, this.description, this.location, this.organizer, this.attendees));
						count++;
					}
					d.setUTCHours(d.getUTCHours()+this.freq_interval);
				} else if (this.freq == "DAILY") {
					if (this.is_recurring_date_compliant(d)) {
						if (d.getTime() >= d1.getTime())
							events.push(new this.event_type(this.calendar, this.uid, this._getStart(d), this._getEnd(d), this.modified, this.title, this.description, this.location, this.organizer, this.attendees));
						count++;
					}
					d.setDate(d.getDate()+this.freq_interval);
				} else
					break; // unknown frequency
			}
		}
		return events;
	};
	
	this._getStart = function(d) {
		var s = new Date();
		s.setTime(d.getTime());
		return s;
	}
	this._getEnd = function(d) {
		var e = new Date();
		e.setTime(d.getTime()+(this.end.getTime()-this.start.getTime()));
		return e;
	}
	
	this.is_recurring_date_compliant = function(d) {
		if (this.freq_valid_hours) {
			var valid = false;
			for (var i = 0; i < this.freq_valid_hours.length; ++i)
				if (d.getHour() == this.freq_valid_hours[i]) { valid = true; break; }
			if (!valid) return false;
		}
		if (this.freq_valid_months) {
			var valid = false;
			for (var i = 0; i < this.freq_valid_months.length; ++i)
				if (d.getMonth()+1 == this.freq_valid_months[i]) { valid = true; break; }
			if (!valid) return false;
		}
		if (this.freq_valid_month_days) {
			var valid = false;
			for (var i = 0; i < this.freq_valid_month_days.length; ++i) {
				var md = parseInt(this.freq_valid_month_days[i]);
				if (md > 0 && d.getDate() == md) { valid = true; break; }
				else if (md < 0 && d.getDate() == (get_days_in_month(d.getMonth()+1, d.getFullYear())+md+1)) { valid = true; break; }
			}
			if (!valid) return false;
		}
		if (this.freq_valid_year_days) {
			var valid = false;
			var dyd = get_year_day(d);
			for (var i = 0; i < this.freq_valid_year_days.length; ++i) {
				var yd = parseInt(this.freq_valid_year_days[i]);
				if (yd > 0 && dyd == yd) { valid = true; break; }
				else if (md < 0 && dyd == (get_days_in_year(d.getFullYear())+yd+1)) { valid = true; break; }
			}
			if (!valid) return false;
		}
		if (this.freq_valid_week_days) {
			var valid = false;
			for (var i = 0; i < this.freq_valid_week_days.length; ++i) {
				var wds = this.freq_valid_week_days[i];
				if (wds.length == 2) {
					var wd = parse_week_day(wds);
					if (wd == d.getDay()) { valid = true; break; }
				} else {
					var wd = parse_week_day(wds.substring(wds.length-2));
					var or = parseInt(wds.substring(0, wds.length-2));
					if (this.freq == "WEEKLY" || this.freq == "MONTHLY") {
						if (d.getDate() == get_month_day_to_match(or, wd, d.getMonth()+1, d.getFullYear())) { valid = true; break; }
					} else if (this.freq == "YEARLY") {
						if (d.getDate() == get_year_day_to_match(or, wd, d.getFullYear())) { valid = true; break; }
					}
				}
			}			
			if (!valid) return false;
		}
		if (this.freq_valid_weeks) {
			var valid = false;
			var ws;
			if (this.freq_wkst) ws = parse_week_day(this.freq_wkst); else ws = 0;
			var dw = get_week_number(d, ws);
			var nb_weeks = get_nb_weeks(d.getFullYear(), ws);
			for (var i = 0; i < this.freq_valid_weeks.length; ++i) {
				var w = parseInt(this.freq_valid_weeks[i]);
				if (w > 0 && dw == w) { valid = true; break; }
				else if (w < 0 && dw == nb_weeks+w+1) { valid = true; break; }
			}
			if (!valid) return false;
		}
		return true;
	};
}
Calendar_RecurringEvent.prototype = new Calendar_Event();
Calendar_RecurringEvent.prototype.constructor = Calendar_RecurringEvent;

function Calendar_RecurringTimedEvent(calendar,uid,start,end,modified,title,description,location,organizer,attendees, freq) {
	Calendar_RecurringEvent.call(this, calendar,uid,start,end,modified,title,description,location,organizer,attendees,freq,Calendar_TimedEvent);
}
Calendar_RecurringTimedEvent.prototype = new Calendar_RecurringEvent();
Calendar_RecurringTimedEvent.prototype.constructor = Calendar_RecurringTimedEvent;
function Calendar_RecurringRangeEvent(calendar,uid,start,end,modified,title,description,location,organizer,attendees,freq) {
	Calendar_RecurringEvent.call(this, calendar,uid,start,end,modified,title,description,location,organizer,attendees,freq,Calendar_RangeEvent);
}
Calendar_RecurringRangeEvent.prototype = new Calendar_RecurringEvent();
Calendar_RecurringRangeEvent.prototype.constructor = Calendar_RecurringRangeEvent;

function get_days_in_month(month, year) {
	switch (month) {
	case 1: case 3: case 5: case 7: case 8: case 10: case 12: return 31;
	case 4: case 6: case 9: case 11: return 30;
	case 2:
		if ((year % 4) != 0) return 28;
		if ((year % 100) != 0) return 28;
		return 29;
	}
}
function get_year_day(d) {
	var m = 1;
	var yd = 0;
	while (m < d.getMonth()+1) { yd += get_days_in_month(m, d.getFullYear()); m++; }
	yd += d.getDate();
	return yd;
}
function get_days_in_year(year) {
	if ((year % 4) != 0) return 365;
	if ((year % 100) != 0) return 365;
	return 366;
}
function parse_week_day(s) {
	switch (s) {
	case "SU": return 0;
	case "MO": return 1;
	case "TU": return 2;
	case "WE": return 3;
	case "TH": return 4;
	case "FR": return 5;
	case "SA": return 6;
	}
}
function get_month_day_to_match(num, week_day, month, year) {
	var d = new Date();
	d.setMonth(month-1);
	d.setFullYear(year);
	if (num > 0) {
		d.setDate(1);
		do {
			if (d.getDay() == week_day) {
				if (--num == 0) return d.getDate();
			}
			d.setDate(d.getDate()+1);
			if (d.getMonth() != month-1) return -1;
		} while (true);
	} else {
		d.setDate(get_days_in_month(month, year));
		do {
			if (d.getDay() == week_day) {
				if (--num == 0) return d.getDate();
			}
			d.setDate(d.getDate()-1);
			if (d.getMonth() != month-1) return -1;
		} while (true);
	}
}
function get_year_day_to_match(num, week_day, year) {
	var d = new Date();
	d.setFullYear(year);
	if (num > 0) {
		d.setDate(1);
		d.setMonth(0);
		do {
			if (d.getDay() == week_day) {
				if (--num == 0) return d.getDate();
			}
			d.setDate(d.getDate()+1);
			if (d.getFullYear() != year) return -1;
		} while (true);
	} else {
		d.setDate(31);
		d.setMonth(11);
		do {
			if (d.getDay() == week_day) {
				if (--num == 0) return d.getDate();
			}
			d.setDate(d.getDate()-1);
			if (d.getFullYear() != year) return -1;
		} while (true);
	}
}
function get_week_number(date, wkst) {
	var w = 1;
	var d = new Date();
	d.setTime(date.getTime());
	d.setDate(1);
	d.setMonth(0);
	if (d.getDay() == wkst) {
		if (d.getDate() == date.getDate() && d.getMonth() == date.getMonth()) return 1;
		d.setDate(2);
	}
	while (d.getDate() != date.getDate() || d.getMonth() != date.getMonth()) {
		d.setDate(d.getDate()+1);
		if (d.getDay() == wkst) w++;
	}
	return w;
}
function get_nb_weeks(year, wkst) {
	var d = new Date();
	d.setDate(31);
	d.setMonth(11);
	d.setFullYear(year);
	return get_week_number(d, wkst);
}