function Animation(element,from,to,duration,start_time,handler) {
	this.element = element;
	this.from = from;
	this.to = to;
	this.duration = duration;
	this.start_time = start_time;
	this.handler = handler;
	this.stopped = false;
}

animation = {
	animations: [],
	create: function(element, from, to, duration, handler) {
		var anim = new Animation(element,from,to,duration,new Date().getTime(),handler);
		this.animations.push(anim);
		handler(from, element);
		if (this.animations.length == 1) setTimeout("animation.evolve()",1);
		return anim;
	},
	stop: function(anim) {
		anim.stopped = true;
	},
	evolve: function() {
		var now = new Date().getTime();
		for (var i = 0; i < this.animations.length; ++i) {
			var anim = this.animations[i];
			if (anim.stopped) {
				this.animations.splice(i,1);
				i--;
				continue;
			}
			if (now - anim.start_time >= anim.duration) {
				this.animations.splice(i,1);
				i--;
				anim.handler(anim.to, anim.element);
				continue;
			}
			var time = now-anim.start_time;
			var new_value;
			if (anim.from && anim.from.length) {
				new_value = new Array();
				for (var vi = 0; vi < anim.from.length; ++vi) {
					var amount = anim.to[vi]-anim.from[vi];
					new_value.push(anim.from[vi]+(time*amount/anim.duration));
				}
			} else {
				var amount = anim.to-anim.from;
				new_value = anim.from+(time*amount/anim.duration);
			}
			anim.handler(new_value, anim.element);
		}
//		var now2 = new Date().getTime();
//		var next = 50 - (now2-now);
//		if (next < 0) next = 0;
		if (this.animations.length > 0) setTimeout("animation.evolve()",1);
	},
	
	fadeIn: function(element, duration, end_handler, start, end) {
		if (start == null) start = 0;
		if (end == null) end = 100; else end = Math.floor(end);
		return animation.create(element, start, end, duration, function(value, element) {
			value = Math.floor(value);
			if (value == 0)
				element.style.visibility = 'hidden';
			else {
				element.style.visibility = 'visible';
				setOpacity(element,value/100);
				if (value == end && end_handler != null) { end_handler(element); end_handler = null; }
			}
		});
	},
	fadeOut: function(element, duration, end_handler, start, end) {
		if (start == null) start = 100;
		if (end == null) end = 0;
		return animation.create(element, start, end, duration, function(value, element) {
			value = Math.floor(value);
			if (value == 0) {
				element.style.visibility = 'hidden';
				if (end_handler != null) { end_handler(element); end_handler = null; }
			} else {
				element.style.visibility = 'visible';
				setOpacity(element,value/100);
			}
		});
	},
	fadeColor: function(element, from, to, duration) {
		return animation.create(element, from, to, duration, function(value, element){
			element.style.color = "rgb("+Math.round(value[0])+","+Math.round(value[1])+","+Math.round(value[2])+")";
		});
	},
	fadeBackgroundColor: function(element, from, to, duration) {
		return animation.create(element, from, to, duration, function(value, element){
			element.style.backgroundColor = "rgb("+Math.round(value[0])+","+Math.round(value[1])+","+Math.round(value[2])+")";
		});
	}
};
