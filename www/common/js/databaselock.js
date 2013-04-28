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
		for (var i = 0; i < this._locks.length; ++i) {
			if (now - this._locks[i].time > this._timeout_time) {
				pn.add_javascript("/static/common/js/component/popup_window.js",function() {
					var p = new popup_window("",null);
					p.setContentFrame("/static/common/js/databaselock_inactivity.html");
					p.onclose = function() {
						setTimeout("window.pn_database_locks._check();", this._check_time);
					};
					p.show();
				});
				popup = true;
				break;
			} else
				pn.ajax_service_xml("/dynamic/application/service/update_db_lock","id="+this._locks[i].id,function(xml){});
		}
		if (!popup)
			setTimeout("window.pn_database_locks._check();", this._check_time);		
	},
	_user_active: function() {
		for (var i = 0; i < this._locks.length; ++i)
			this._locks[i].time = new Date().getTime();
	},
	_user_inactive: function() {
		var remaining = this._locks.length;
		if (remaining == 0) return;
		var closed = function() {
			if (--remaining == 0)
				window.top.location = "/";
		}
		for (var i = 0; i < this._locks.length; ++i)
			pn.ajax_service_xml("/dynamic/application/service/close_db_lock","id="+this._locks[i].id,function(xml){
				setTimeout(closed,1);
			});
	},
	_close_lock: function(id,foreground) {
		pn.ajax_service_xml("/dynamic/application/service/close_db_lock","id="+id,function(xml){
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
	}
}

init_databaselock();
