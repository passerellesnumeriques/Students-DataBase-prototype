function Calendar() {
	var t=this;
	
	t.onerror = null;
	t.onloading = null;
	t.onloaded = null;
	
	t.on_new_timed_event = null;
	t.on_timed_event_changed = null;
	t.on_timed_event_removed = null;
	
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
					if (t._timedEvents[j].calendar == call)
						t._timedEvents[j].removed = true;
				
				for (var i = 0; i < output.result.length; ++i) {
					var ev = output.result[i];
					if (typeof ev.start["hour"] == 'undefined') {
						// this is a range event
						// TODO
						continue;
					}
					// this is a timed event
					ev = new Calendar_TimedEvent(cal, ev.uid,ev.start,ev.end,ev.modified,ev.title,ev.description);
					var found = false;
					for (var j = 0; j < t._timedEvents.length; j++) {
						if (t._timedEvents[j].calendar == cal && t._timedEvents[j].uid == ev.uid) {
							t._timedEvents[j].removed = false;
							if (!t._timedEvents[j].modifed || !ev.modified || t._timedEvents[j].modifed.getTime() < ev.modified.getTime()) {
								// update
								t._timedEvents[j] = ev;
								if (t.on_timed_event_changed) t.on_timed_event_changed(ev);
							} else {
								// no change
								found = true;
								break;
							}
						}
					}
					if (found) continue;
					t._timedEvents.push(ev);
					if (t.on_new_timed_event) t.on_new_timed_event(ev);
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
				
				if (t.onloaded) t.onloaded(t, cal);
			}
		);
	};
	
	//setTimeout(function(){t.refresh_calendars();},60000); // refresh every minute
}

function URLCalendar(name,url,color) {
	this.name = name;
	this.url = url;
	if (!color) color = "#A0A0FF";
	this.color = color;
}
function Calendar_Date(date) {
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
function Calendar_TimedEvent(calendar,uid,start,end,modified,title,description) {
	this.calendar = calendar;
	this.uid = uid;
	this.title = title;
	this.description = description;
	this.start = Calendar_Date(start);
	this.end = Calendar_Date(end);
	this.modified = modified ? Calendar_Date(modified) : null;
}
function Calendar_RecurringTimedEvent() {
	
}
function Calendar_RangeEvent() {
	
}
function Calendar_RecurringRangeEvent() {
	
}