function Point2D(x,y) {
	this.x = x;
	this.y = y;
	
	this.getDistanceSquared = function(pt) {
		return (pt.x-this.x)*(pt.x-this.x)+(pt.y-this.y)*(pt.y-this.y);
	};
	this.getDistance = function(pt) {
		return Math.sqrt(this.getDistanceSquared(pt));
	};
}

function Line2D(pt1,pt2) {
	this.pt1 = pt1;
	this.pt2 = pt2;
	
	this.getEquation = function() {
		var equation = [0,0,0];
		if (this.pt1.x == this.pt2.x) {
			if (this.pt1.y == this.pt2.y)
				return equation;
			equation[0]=1;
			equation[1]=0;
			equation[2]=this.pt1.x;
			return equation;
		}
		
		equation[0]=(this.pt1.y-this.pt2.y)/(this.pt2.x-this.pt1.x);
		equation[1]=1.0;
		equation[2]=this.pt2.y+equation[0]*this.pt2.x;
		return equation;
	};
	this.getDistanceSquared = function(pt) {
		  var l2 = this.pt1.getDistanceSquared(this.pt2);
		  if (l2 == 0) return pt.getDistanceSquared(this.pt1);
		  var t = ((pt.x - this.pt1.x) * (this.pt2.x - this.pt1.x) + (pt.y - this.pt1.y) * (this.pt2.y - this.pt1.y)) / l2;
		  if (t < 0) return pt.getDistanceSquared(this.pt1);
		  if (t > 1) return pt.getDistanceSquared(this.pt2);
		  return pt.getDistanceSquared(new Point2D(this.pt1.x + t * (this.pt2.x - this.pt1.x), this.pt1.y + t * (this.pt2.y - this.pt1.y)));
	}
	this.getDistance = function(pt) {
		//var eq = this.getEquation();
		//var dist = eq[0]*pt.x + eq[1]*pt.y + eq[2];
		//if (dist < 0) dist = -dist;
		//dist = dist / Math.sqrt(eq[0]*eq[0] + eq[1]*eq[1]);
		//return dist;
		return Math.sqrt(this.getDistanceSquared(pt));
	};
}

drawing = {
	horizontal_connector: function(e1,e2) {
		if (typeof e1 == 'string') e1 = document.getElementById(e1);
		if (typeof e2 == 'string') e2 = document.getElementById(e2);
		var parent = getAbsoluteParent(e1);
		var conn = document.createElement("DIV");
		var table = document.createElement("TABLE"); conn.appendChild(table);
		table.style.borderCollapse = 'collapse';
		table.style.borderSpacing = "0px";
		table.appendChild(table = document.createElement("TBODY"));
		conn.style.position = 'absolute';
		parent.appendChild(conn);
		var refresh = function() {
			var x1 = absoluteLeft(e1, parent)+e1.offsetWidth;
			var y1 = absoluteTop(e1, parent)+e1.offsetHeight/2;
			var x2 = absoluteLeft(e2, parent);
			var y2 = absoluteTop(e2, parent)+e2.offsetHeight/2;
			var start_y = y1, end_y = y2;
			if (y1 > y2) { var y = y1; y1 = y2; y2 = y; }
			conn.style.top = y1+"px";
			conn.style.left = x1+"px";
			conn.style.width = (x2-x1+1)+"px";
			conn.style.height = (y2-y1+1)+"px";
			while (table.childNodes.length > 0) table.removeChild(table.childNodes[0]);
			var line = new Line2D(new Point2D(x1,start_y),new Point2D(x2,end_y));
			for (var y = 0; y < y2-y1+1; y++) {
				var tr = document.createElement("TR"); table.appendChild(tr);
				tr.style.height = "1px";
				for (var x = 0; x < x2-x1+1; x++) {
					var td = document.createElement("TD"); tr.appendChild(td);
					td.style.width = "1px";
					var dist = line.getDistance(new Point2D(x1+x,y1+y));
					td.style.padding = "0px";
					td.data = dist;
					var col = 256;
					if (dist < 0.25)
						col = 0;
					else if (dist < 1)
						col = Math.floor(255*(dist));
					if (col < 256)
						td.style.backgroundColor = "rgb("+col+","+col+","+col+")";
				}
			}
		};
		refresh();
		addLayoutEvent(parent,function() { setTimeout(refresh,1); });
	}
};