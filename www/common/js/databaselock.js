window.pn_database_locks = {
	add_lock: function(id) {
		window.top.pn_database_locks._locks.push({
			id: id,
			time: new Date().getTime()
		});
	},
	remove_lock: function(id) {
		for (var i = 0; i < window.top.pn_database_locks._locks.length; ++i)
			if (window.top.pn_database_locks._locks[i].id == id)
				window.top.pn_database_locks._locks.splice(i,1);
	},
	
	_locks: [],
	_check_time: 30000,
	_timeout_time: 120000,
	_check: function() {
		var now = new Date().getTime();
		var popup = false;
		for (var i = 0; i < this._locks.length; ++i) {
			if (now - this._locks[i].time > this._timeout_time) {
				pn.popup("",null,"<iframe src='/static/common/js/databaselock_inactivity.html' frameborder=0 style='width:100%;height:100%'></iframe>",function() {
					setTimeout("window.pn_database_locks._check();", this._check_time);
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
	}
}

if (window.top == window)
	setTimeout("window.pn_database_locks._check();",pn_database_locks._check_time);

listenEvent(window,'click',function() { window.top.pn_database_locks._user_active(); });
listenEvent(window,'mousemove',function() { window.top.pn_database_locks._user_active(); });
