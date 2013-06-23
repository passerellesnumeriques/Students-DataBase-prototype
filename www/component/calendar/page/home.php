<?php
$this->add_javascript("/static/common/js/Status.js");
$this->add_javascript("/static/common/js/StatusUI_Top.js");
$this->add_javascript("/static/common/js/splitter_vertical/splitter_vertical.js");
$this->add_stylesheet("/static/common/js/splitter_vertical/splitter_vertical.css");
$this->add_javascript("/static/common/js/vertical_layout/vertical_layout.js");
$this->add_javascript("/static/calendar/calendar.js");
$this->add_javascript("/static/calendar/calendar_view_week.js");
$this->add_stylesheet("/static/calendar/calendar_view_week.css");
$this->onload("new vertical_layout('calendar_page');new splitter_vertical('calendar_split',0.3);");

require_once("common/SQLQuery.inc");
$calendars = SQLQuery::create()->select("UserCalendar")->where("username",PNApplication::$instance->user_management->username)->join("UserCalendar","Calendar",array("calendar"=>"id"))->execute();
foreach ($calendars as &$cal)
	$cal["color"] = "#A0F0A0";
?>
<div id='calendar_page' style='width:100%;height:100%'>
	<div id='calendar_header' layout='fixed'>
		<div class='button' onclick='import_calendar();'><img src='/static/common/images/import.png'/> <?php locale("Import Calendar")?></div>
	</div>
	<div id='calendar_split' layout='fill'>
		<div id='calendar_left'>
<?php
foreach ($calendars as &$cal) {
	echo "<div>";
	echo "<div style='width:10px;height:10px;border:1px solid black;background-color:".$cal["color"].";display:inline-block;vertical-align:bottom'></div>";
	echo $cal["name"];
	echo "</div>";
}
?>
		</div>
		<div id='calendar_content'>
			Content
		</div>
	</div>
</div>
<script type='text/javascript'>
var status_mgr = new StatusManager();
new StatusUI_Top(status_mgr, 0);
var loading_status = new StatusMessage(Status_TYPE_PROCESSING, "<?php locale("Loading calendars")?>");
loading_status.counter = 0;

var cal = new Calendar();
cal.onloading = function(calendars, calendar) {
	loading_status.counter++;
	if (loading_status.counter == 1)
		status_mgr.add_status(loading_status);
};
cal.onloaded = function(calendars, calendar) {
	loading_status.counter--;
	if (loading_status.counter == 0)
		status_mgr.remove_status(loading_status);
};
cal.onerror = function(calendars, calendar, error) {
	loading_status.counter--;
	if (loading_status.counter == 0)
		status_mgr.remove_status(loading_status);
	status_mgr.add_status(new StatusMessageError(null, calendar.name+": "+error, 5000));
};
cal.onaction = function(id, message) {
	var s = new StatusMessage(Status_TYPE_PROCESSING, message);
	s.calid = id;
	status_mgr.add_status(s);
};
cal.onactiondone = function(id) {
	for (var i = 0; i < status_mgr.status.length; ++i)
		if (status_mgr.status[i].calid && status_mgr.status[i].calid == id) {
			status_mgr.remove_status(status_mgr.status[i]);
			return;
		}
};
var view = new calendar_view_week('calendar_content',cal);
<?php
foreach ($calendars as &$cal)
	echo "cal.add_calendar(".json_encode($cal["name"]).",'/dynamic/calendar/service/get?id=".$cal["calendar"]."','".$cal["color"]."');";
?>

function import_calendar() {
	add_javascript("/static/common/js/wizard/wizard.js",function() {
		var w = new wizard();
		w.title = "<?php locale("Import Calendar")?>";
		var page_type_of_calendar;
		var page_internet_url;

		// page 1: type and name of calendar
		var div = document.createElement("DIV");
		var type_internet = document.createElement("INPUT");
		type_internet.type = 'radio';
		type_internet.onchange = function() {
			if (this.checked) {
				w.removePagesFrom(1);
				w.addPage(page_internet_url);
			}
			w.validate();
		};
		div.appendChild(type_internet);
		div.appendChild(document.createTextNode("<?php locale("From internet")?>"));
		div.appendChild(document.createElement("BR"));
		div.appendChild(document.createElement("BR"));
		div.appendChild(document.createTextNode("<?php locale("common","Name")?>"));
		var calendar_name = document.createElement("INPUT");
		calendar_name.type = "text";
		calendar_name.size=30;
		calendar_name.max_length=100;
		calendar_name.onkeyup = function() { w.validate(); };
		div.appendChild(calendar_name);
		page_type_of_calendar = {
			icon: "/static/common/images/import_32.png",
			title: "<?php locale("Type of calendar to import")?>",
			content: div,
			validate: function(w,handler) {
				if (calendar_name.value.length == 0)
					handler(false);
				else {
					if (type_internet.checked)
						handler(true);
					else
						handler(false);
				}
			}
		};

		// type internet: page to enter URL
		div = document.createElement("DIV");
		var internet_url = document.createElement("INPUT");
		internet_url.type = "text";
		internet_url.size = 50;
		internet_url.max_length = 1024;
		internet_url.onkeyup = function() {
			w.validate();
		};
		div.appendChild(internet_url);
		page_internet_url = {
			icon: "/static/common/images/import_32.png",
			title: "<?php locale("Internet Calendar Address")?>",
			content: div,
			validate: function(w,handler) {
				if (internet_url.value.length == 0)
					handler(false);
				else
					handler(true);
			}
		};

		// onfinish
		w.onfinish = function() {
			var name = calendar_name.value;
			var type, data;
			if (type_internet.checked) {
				type = "internet";
				data = internet_url.value;
			}
			var s = new StatusMessage(Status_TYPE_PROCESSING, "<?php locale("Import calendar")?>");
			status_mgr.add_status(s);
			ajax.post_parse_result("/dynamic/calendar/service/import_calendar",{name:name,type:type,data:data},function(result){
				status_mgr.remove_status(s);
				if (result && result.id)
					cal.add_calendar(name, "/dynamic/calendar/service/get?id="+result.id, "#A0FFA0");
			});
		};

		// start wizard
		w.addPage(page_type_of_calendar);
		w.launch();
	});
}
</script>