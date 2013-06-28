window.pn_database_locks = {
	add_lock: function(id) {
		for (var i = 0; i < this._locks.length; ++i)
			if (this._locks[i].id == id) return;
		if (window != window.top) {
			this._locks.push({id:id});
			for (var i = 0; i < window.top.pn_database_locks._locks.length; ++i)
				if (window.top.pn_database_locks._locks[i].id == id)
					return;
		}
		window.top.pn_database_locks._locks.push({
			id: id,
			time: new Date().getTime()
		});
	},
	remove_lock: function(id) {
		for (var i = 0; i < window.top.pn_database_locks._locks.length; ++i)
			if (window.top.pn_database_locks._locks[i].id == id)
				window.top.pn_database_locks._locks.splice(i,1);
		for (var i = 0; i < this._locks.length; ++i)
			if (this._locks[i].id == id)
				this._locks.splice(i,1);
	},
	
	_locks: [],
	_check_time: 30000,
	_timeout_time: 120000,
	_check: function() {
		var now = new Date().getTime();
		var popup = false;
		var t = this;
		for (var i = 0; i < this._locks.length; ++i) {
			if (now - this._locks[i].time > this._timeout_time) {
				add_javascript("/static/common/js/configuration.js",function() {
					add_javascript("/static/common/js/popup_window/popup_window.js",function() {
						var p = new popup_window("",null);
						p.setContentFrame("/static/application/databaselock_inactivity.html");
						p.onclose = function() {
							setTimeout("window.pn_database_locks._check();", t._check_time);
						};
						p.show();
					});
				});
				popup = true;
				break;
			} else
				ajax.post_parse_result("/dynamic/application/service/update_db_lock","id="+this._locks[i].id,function(result){});
		}
		if (!popup)
			setTimeout("window.pn_database_locks._check();", this._check_time);		
	},
	_last_activity: new Date().getTime(),
	_user_active: function() {
		for (var i = 0; i < this._locks.length; ++i)
			this._locks[i].time = new Date().getTime();
		this._last_activity = new Date().getTime();
		window.databaselock_update_inactivity();
	},
	_user_inactive: function() {
		var remaining = this._locks.length;
		if (remaining == 0) return;
		var closed = function() {
			if (--remaining == 0)
				window.top.frames[0].location.href = "/dynamic/application/page/enter";
		}
		for (var i = 0; i < this._locks.length; ++i)
			ajax.post_parse_result("/dynamic/application/service/close_db_lock","id="+this._locks[i].id,function(result){
				setTimeout(closed,1);
			});
	},
	_close_lock: function(id,foreground) {
		ajax.post_parse_result("/dynamic/application/service/close_db_lock","id="+id,function(result){
		},foreground);
		this.remove_lock(id);
	},
	_close_window: function() {
		while (this._locks.length > 0)
			this._close_lock(this._locks[0].id, true);
	}
}

if (window.top == window)
	setTimeout("window.pn_database_locks._check();",pn_database_locks._check_time);

function init_databaselock() {
	if (typeof listenEvent == 'undefined')
		setTimeout(init_databaselock, 10);
	else {
		listenEvent(window,'click',function() { window.top.pn_database_locks._user_active(); });
		listenEvent(window,'mousemove',function() { window.top.pn_database_locks._user_active(); });
		window.onbeforeunload = function() {
			window.pn_database_locks._close_window();
		}
		if (window == window.top)
			window.databaselock_update_inactivity_interval = setInterval(window.databaselock_update_inactivity, 2000);
	}
}
if (window == window.top)
window.databaselock_update_inactivity = function() {
	var time = new Date().getTime();
	time -= window.pn_database_locks._last_activity;
	var status = document.getElementById('inactivity_status');
	if (status == null) return;
	clearInterval(window.databaselock_update_inactivity_interval);
	if (time < 10000) {
		status.style.visibility = 'hidden';
		status.style.position = 'absolute';
		window.databaselock_update_inactivity_interval = setInterval(window.databaselock_update_inactivity, 2000);
	} else if (time > 30*60*1000) {
		window.top.frames[0].location = "/dynamic/application/page/logout?from=inactivity";
	} else {
		status.style.visibility = 'visible';
		var t = document.getElementById('inactivity_time');
		var s = "";
		if (time >= 60*1000) {
			s += Math.floor(time/(60*1000))+"m";
			time = time % (60*1000);
		}
		s += Math.floor(time/1000)+"s";
		t.innerHTML = s;
		window.databaselock_update_inactivity_interval = setInterval(window.databaselock_update_inactivity, 1000);
	}
}

init_databaselock();
