locale = {
	get_string: function(str,namedValues) {
		key = str.toLowerCase();
		var l = null;
		for (var i = 0; i < locale._strings.length; ++i)
			if (locale._strings[i].key == key) {
				l = locale._strings[i];
				break;
			}
		if (l == null) return "??"+str+"??";
		var s = l.traduction;
		if (l.word_pos.length == 0) {
			if (str.substr(0,1)==str.substr(0,1).toUpperCase()) 
				s = s.substr(0,1).toUpperCase()+s.substr(1);
		} else {
			var word = 0;
			i = 0;
			while ((i = s.indexOf('~', i)) >= 0) {
				var ss = s.substr(0, i);
				if (word < l.word_pos.length) {
					if (str.substr(l.word_pos[word], 1)==str.substr(l.word_pos[word], 1).toUpperCase())
						ss += s.substr(i+1, 1).toUpperCase()+s.substr(i+2);
					else
						ss += s.substr(i+1);
					word++;
				} else
					ss += s.substr(i+1);
				s = ss;
			}
		}
		if (namedValues != null && namedValues.length > 0) {
			for (i = 0; i < namedValues.length; i += 2) {
				var name = "%"+namedValues[i]+"%";
				var j = 0;
				while ((j = s.indexOf(name, j)) >= 0) {
					s = s.substr(0, j) + namedValues[i+1] + s.substr(j+name.length);
					j += namedValues[i+1].length;
				}
			}
		}
		return s;
	},
	_strings: [],
	load: function(url) {
		ajax.call("GET", url, null, null, function(error) {
			if (typeof error_dialog != 'undefined')
				error_dialog("Error loading localized strings '"+url+"': "+error);
			else
				alert("Error loading localized strings '"+url+"': "+error);
		}, function(r) {
			var strings = new Object();
			var t = r.responseText;
			var i = 0;
			while(i<t.length){
				j=t.indexOf('\n',i);
				if (j<0){
					s=t.substring(i);
					i=t.length;
				}else{
					s=t.substring(i,j);
					i=j+1;
				}
				s=s.trim();
				if(s.length==0)continue;
				j=s.indexOf('=');
				if(j<0)continue;
				var name = s.substring(0,j).trim();
				var value = s.substring(j+1).trim();
				strings[name]=value;
			}
			for (var str in strings) {
				var l = new Object();
				l.word_pos = [];
				var i = 0;
				var s = str;
				while ((i = s.indexOf('¤', i)) >= 0) {
					l.word_pos.push(i);
					s = s.substr(0, i)+s.substr(i+1);
				}
				l.traduction = strings[str].toLowerCase();
				l.key = s.toLowerCase();
				locale._strings.push(l);
			}
		}, true);
	}
};